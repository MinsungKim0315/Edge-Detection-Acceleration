#include "dut.hpp"

#include <cmath>
#include <cstdint>

namespace {

using Pixel = std::uint8_t;
using Direction = std::uint8_t;
using BlurPixel = std::uint16_t;
using Magnitude = float;

constexpr Pixel kNoEdgeLocal = 0;
constexpr Pixel kWeakEdgeLocal = 75;
constexpr Pixel kStrongEdgeLocal = 255;
constexpr int kBlurScale = 256;
constexpr float kGaussianKernel2D[kGaussianKernelSize][kGaussianKernelSize] = {
    {0.012146124730090273f, 0.026108117663175317f, 0.03369625012825844f, 0.026108117663175317f, 0.012146124730090273f},
    {0.026108117663175317f, 0.056127299040522325f, 0.07244443355700187f, 0.056127299040522325f, 0.026108117663175317f},
    {0.03369625012825844f, 0.07244443355700187f, 0.09348737760471192f, 0.07244443355700187f, 0.03369625012825844f},
    {0.026108117663175317f, 0.056127299040522325f, 0.07244443355700187f, 0.056127299040522325f, 0.026108117663175317f},
    {0.012146124730090273f, 0.026108117663175317f, 0.03369625012825844f, 0.026108117663175317f, 0.012146124730090273f},
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

void gaussian_blur_hw(const Pixel input[kMaxImageHeight][kMaxImageWidth], BlurPixel blurred[kMaxImageHeight][kMaxImageWidth], int width, int height) {
blur_rows:
    for (int y = 0; y < height; ++y) {
    blur_cols:
        for (int x = 0; x < width; ++x) {
#pragma HLS PIPELINE II=1
            float sum = 0.0f;
        blur_ky:
            for (int ky = 0; ky < kGaussianKernelSize; ++ky) {
            blur_kx:
                for (int kx = 0; kx < kGaussianKernelSize; ++kx) {
#pragma HLS UNROLL
                    const int sy = clamp_int(y + ky - 2, 0, height - 1);
                    const int sx = clamp_int(x + kx - 2, 0, width - 1);
                    sum += static_cast<float>(input[sy][sx]) * kGaussianKernel2D[ky][kx];
                }
            }
            const float scaled = sum * static_cast<float>(kBlurScale);
            blurred[y][x] = static_cast<BlurPixel>(scaled + 0.5f);
        }
    }
}

void sobel_row_hw(const BlurPixel blurred[kMaxImageHeight][kMaxImageWidth], int width, int height, int y, Magnitude magnitude_row[kMaxImageWidth], Direction direction_row[kMaxImageWidth]) {
sobel_row_cols:
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
                const float sample = static_cast<float>(blurred[sy][sx]) / static_cast<float>(kBlurScale);
                gx += sample * static_cast<float>(kSobelX[ky][kx]);
                gy += sample * static_cast<float>(kSobelY[ky][kx]);
            }
        }
        magnitude_row[x] = std::sqrt((gx * gx) + (gy * gy));
        direction_row[x] = quantize_direction(gy, gx);
    }
}

Pixel nms_threshold_pixel(const Magnitude prev_mag[kMaxImageWidth],
                         const Magnitude curr_mag[kMaxImageWidth],
                         const Magnitude next_mag[kMaxImageWidth],
                         const Direction curr_dir[kMaxImageWidth],
                         int x,
                         float low_threshold,
                         float high_threshold) {
    const float center = curr_mag[x];
    float neighbor_a = 0.0f;
    float neighbor_b = 0.0f;

    switch (curr_dir[x]) {
        case 0:
            neighbor_a = curr_mag[x - 1];
            neighbor_b = curr_mag[x + 1];
            break;
        case 45:
            neighbor_a = next_mag[x - 1];
            neighbor_b = prev_mag[x + 1];
            break;
        case 90:
            neighbor_a = prev_mag[x];
            neighbor_b = next_mag[x];
            break;
        default:
            neighbor_a = prev_mag[x - 1];
            neighbor_b = next_mag[x + 1];
            break;
    }

    float suppressed = 0.0f;
    if (center >= neighbor_a && center >= neighbor_b) {
        suppressed = center;
    }

    if (suppressed >= high_threshold) {
        return kStrongEdgeLocal;
    }
    if (suppressed >= low_threshold) {
        return kWeakEdgeLocal;
    }
    return kNoEdgeLocal;
}

void write_thresholded_stream(DutStream& strmOut,
                              const BlurPixel blurred[kMaxImageHeight][kMaxImageWidth],
                              int width,
                              int height,
                              float low_threshold,
                              float high_threshold) {
    static Magnitude mag_rows[3][kMaxImageWidth];
    static Direction dir_rows[3][kMaxImageWidth];

    strmOut.write(static_cast<DutWord>(kDutProtocolVersion));
    strmOut.write(static_cast<DutWord>(width));
    strmOut.write(static_cast<DutWord>(height));

    if (height <= 0 || width <= 0) {
        return;
    }

    sobel_row_hw(blurred, width, height, 0, mag_rows[0], dir_rows[0]);
    if (height > 1) {
        sobel_row_hw(blurred, width, height, 1, mag_rows[1], dir_rows[1]);
    } else {
copy_row0_mag:
        for (int x = 0; x < width; ++x) {
#pragma HLS PIPELINE II=1
            mag_rows[1][x] = mag_rows[0][x];
            dir_rows[1][x] = dir_rows[0][x];
        }
    }

write_top_row:
    for (int x = 0; x < width; ++x) {
#pragma HLS PIPELINE II=1
        strmOut.write(static_cast<DutWord>(kNoEdgeLocal));
    }

process_rows:
    for (int y = 1; y < height - 1; ++y) {
        const int next_slot = (y + 1) % 3;
        sobel_row_hw(blurred, width, height, y + 1, mag_rows[next_slot], dir_rows[next_slot]);

    process_cols:
        for (int x = 0; x < width; ++x) {
#pragma HLS PIPELINE II=1
            Pixel pixel = kNoEdgeLocal;
            if (x > 0 && x < width - 1) {
                pixel = nms_threshold_pixel(mag_rows[(y - 1) % 3], mag_rows[y % 3], mag_rows[(y + 1) % 3], dir_rows[y % 3], x, low_threshold, high_threshold);
            }
            strmOut.write(static_cast<DutWord>(pixel));
        }
    }

write_bottom_row:
    for (int x = 0; x < width; ++x) {
#pragma HLS PIPELINE II=1
        strmOut.write(static_cast<DutWord>(kNoEdgeLocal));
    }
}

}  // namespace

void dut(DutStream& strmIn, DutStream& strmOut) {
#pragma HLS INTERFACE ap_ctrl_hs port=return
#pragma HLS INTERFACE axis port=strmIn
#pragma HLS INTERFACE axis port=strmOut

    static Pixel input[kMaxImageHeight][kMaxImageWidth];
    static BlurPixel blurred[kMaxImageHeight][kMaxImageWidth];

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
    gaussian_blur_hw(input, blurred, width, height);
    write_thresholded_stream(strmOut, blurred, width, height, low_threshold, high_threshold);
}
