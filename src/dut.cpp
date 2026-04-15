#include "dut.hpp"

#include <cmath>
#include <cstdint>


namespace {

using Pixel = std::uint8_t;
using Direction = std::uint8_t;

constexpr Pixel kNoEdgeLocal = 0;
constexpr Pixel kWeakEdgeLocal = 75;
constexpr Pixel kStrongEdgeLocal = 255;

constexpr float kGaussianKernel1D[kGaussianKernelSize] = {
    0.11020945604615712f,
    0.23691201406344845f,
    0.30575705978078893f,
    0.23691201406344845f,
    0.11020945604615712f,
};

constexpr int kSobelX[3][3] = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1},
};

constexpr int kSobelY[3][3] = {
    {1, 2, 1},
    {0, 0, 0},
    {-1, -2, -1},
};

int clamp_int(int value, int low, int high) {
    if (value < low) {
        return low;
    }
    if (value > high) {
        return high;
    }
    return value;
}

Direction quantize_direction(float gy, float gx) {
    const float angle_degrees = std::atan2(gy, gx) * 57.29577951308232f;
    float wrapped = angle_degrees;
    if (wrapped < 0.0f) {
        wrapped += 180.0f;
    }

    if ((wrapped >= 0.0f && wrapped < 22.5f) || (wrapped >= 157.5f && wrapped <= 180.0f)) {
        return 0;
    }
    if (wrapped < 67.5f) {
        return 45;
    }
    if (wrapped < 112.5f) {
        return 90;
    }
    return 135;
}

void read_header(DutStream& strmIn, int& width, int& height, float& low_threshold, float& high_threshold) {
    const DutWord version = strmIn.read();
    (void)version;
    width = static_cast<int>(strmIn.read());
    height = static_cast<int>(strmIn.read());
    low_threshold = static_cast<float>(static_cast<std::uint32_t>(strmIn.read()));
    high_threshold = static_cast<float>(static_cast<std::uint32_t>(strmIn.read()));
}

void read_image(DutStream& strmIn, Pixel image[kMaxImageHeight][kMaxImageWidth], int width, int height) {
read_rows:
    for (int y = 0; y < height; ++y) {
    read_cols:
        for (int x = 0; x < width; ++x) {
#pragma HLS PIPELINE II=1
            image[y][x] = static_cast<Pixel>(static_cast<std::uint32_t>(strmIn.read()) & 0xffu);
        }
    }
}

void gaussian_blur_hw(const Pixel input[kMaxImageHeight][kMaxImageWidth], float temp[kMaxImageHeight][kMaxImageWidth], float output[kMaxImageHeight][kMaxImageWidth], int width, int height) {
blur_h_rows:
    for (int y = 0; y < height; ++y) {
    blur_h_cols:
        for (int x = 0; x < width; ++x) {
#pragma HLS PIPELINE II=1
            float sum = 0.0f;
        blur_h_k:
            for (int k = 0; k < kGaussianKernelSize; ++k) {
#pragma HLS UNROLL
                const int sx = clamp_int(x + k - 2, 0, width - 1);
                sum += static_cast<float>(input[y][sx]) * kGaussianKernel1D[k];
            }
            temp[y][x] = sum;
        }
    }

blur_v_rows:
    for (int y = 0; y < height; ++y) {
    blur_v_cols:
        for (int x = 0; x < width; ++x) {
#pragma HLS PIPELINE II=1
            float sum = 0.0f;
        blur_v_k:
            for (int k = 0; k < kGaussianKernelSize; ++k) {
#pragma HLS UNROLL
                const int sy = clamp_int(y + k - 2, 0, height - 1);
                sum += temp[sy][x] * kGaussianKernel1D[k];
            }
            output[y][x] = sum;
        }
    }
}

void sobel_hw(const float input[kMaxImageHeight][kMaxImageWidth], float magnitude[kMaxImageHeight][kMaxImageWidth], Direction direction[kMaxImageHeight][kMaxImageWidth], int width, int height) {
sobel_rows:
    for (int y = 0; y < height; ++y) {
    sobel_cols:
        for (int x = 0; x < width; ++x) {
#pragma HLS PIPELINE II=1
            float gx = 0.0f;
            float gy = 0.0f;
        sobel_ky:
            for (int ky = 0; ky < 3; ++ky) {
            sobel_kx:
                for (int kx = 0; kx < 3; ++kx) {
#pragma HLS UNROLL
                    const int sy = clamp_int(y + ky - 1, 0, height - 1);
                    const int sx = clamp_int(x + kx - 1, 0, width - 1);
                    const float sample = input[sy][sx];
                    gx += sample * static_cast<float>(kSobelX[ky][kx]);
                    gy += sample * static_cast<float>(kSobelY[ky][kx]);
                }
            }
            magnitude[y][x] = std::sqrt((gx * gx) + (gy * gy));
            direction[y][x] = quantize_direction(gy, gx);
        }
    }
}

void nms_hw(const float magnitude[kMaxImageHeight][kMaxImageWidth], const Direction direction[kMaxImageHeight][kMaxImageWidth], float output[kMaxImageHeight][kMaxImageWidth], int width, int height) {
clear_rows:
    for (int y = 0; y < height; ++y) {
    clear_cols:
        for (int x = 0; x < width; ++x) {
#pragma HLS PIPELINE II=1
            output[y][x] = 0.0f;
        }
    }

nms_rows:
    for (int y = 1; y < height - 1; ++y) {
    nms_cols:
        for (int x = 1; x < width - 1; ++x) {
#pragma HLS PIPELINE II=1
            const float center = magnitude[y][x];
            float neighbor_a = 0.0f;
            float neighbor_b = 0.0f;

            switch (direction[y][x]) {
                case 0:
                    neighbor_a = magnitude[y][x - 1];
                    neighbor_b = magnitude[y][x + 1];
                    break;
                case 45:
                    neighbor_a = magnitude[y + 1][x - 1];
                    neighbor_b = magnitude[y - 1][x + 1];
                    break;
                case 90:
                    neighbor_a = magnitude[y - 1][x];
                    neighbor_b = magnitude[y + 1][x];
                    break;
                default:
                    neighbor_a = magnitude[y - 1][x - 1];
                    neighbor_b = magnitude[y + 1][x + 1];
                    break;
            }

            if (center >= neighbor_a && center >= neighbor_b) {
                output[y][x] = center;
            }
        }
    }
}

void threshold_hw(const float input[kMaxImageHeight][kMaxImageWidth], Pixel output[kMaxImageHeight][kMaxImageWidth], int width, int height, float low_threshold, float high_threshold) {
thresh_rows:
    for (int y = 0; y < height; ++y) {
    thresh_cols:
        for (int x = 0; x < width; ++x) {
#pragma HLS PIPELINE II=1
            const float value = input[y][x];
            Pixel pixel = kNoEdgeLocal;
            if (value >= high_threshold) {
                pixel = kStrongEdgeLocal;
            } else if (value >= low_threshold) {
                pixel = kWeakEdgeLocal;
            }
            output[y][x] = pixel;
        }
    }
}

void write_image(DutStream& strmOut, const Pixel image[kMaxImageHeight][kMaxImageWidth], int width, int height) {
    strmOut.write(static_cast<DutWord>(kDutProtocolVersion));
    strmOut.write(static_cast<DutWord>(width));
    strmOut.write(static_cast<DutWord>(height));
write_rows:
    for (int y = 0; y < height; ++y) {
    write_cols:
        for (int x = 0; x < width; ++x) {
#pragma HLS PIPELINE II=1
            strmOut.write(static_cast<DutWord>(image[y][x]));
        }
    }
}

}  // namespace

void dut(DutStream& strmIn, DutStream& strmOut) {
#pragma HLS INTERFACE ap_ctrl_hs port=return
#pragma HLS INTERFACE axis port=strmIn
#pragma HLS INTERFACE axis port=strmOut

    static Pixel input[kMaxImageHeight][kMaxImageWidth];
    static float gaussian_temp[kMaxImageHeight][kMaxImageWidth];
    static float blurred[kMaxImageHeight][kMaxImageWidth];
    static float magnitude[kMaxImageHeight][kMaxImageWidth];
    static Direction direction[kMaxImageHeight][kMaxImageWidth];
    static float suppressed[kMaxImageHeight][kMaxImageWidth];
    static Pixel thresholded[kMaxImageHeight][kMaxImageWidth];

    int width = 0;
    int height = 0;
    float low_threshold = 0.0f;
    float high_threshold = 0.0f;

    read_header(strmIn, width, height, low_threshold, high_threshold);
    if (width <= 0 || width > kMaxImageWidth || height <= 0 || height > kMaxImageHeight) {
        strmOut.write(static_cast<DutWord>(0));
        strmOut.write(static_cast<DutWord>(0));
        strmOut.write(static_cast<DutWord>(0));
        return;
    }

    read_image(strmIn, input, width, height);
    gaussian_blur_hw(input, gaussian_temp, blurred, width, height);
    sobel_hw(blurred, magnitude, direction, width, height);
    nms_hw(magnitude, direction, suppressed, width, height);
    threshold_hw(suppressed, thresholded, width, height, low_threshold, high_threshold);
    write_image(strmOut, thresholded, width, height);
}
