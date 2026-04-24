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

// Copy a single logical image row between fixed-width local buffers.
void copy_row(const Pixel src[kMaxImageWidth], Pixel dst[kMaxImageWidth], int width) {
copy_row_cols:
    for (int x = 0; x < width; ++x) {
#pragma HLS PIPELINE II=1
        dst[x] = src[x];
    }
}

// Read one row of pixels from the AXIS input stream.
void read_row(DutStream& strmIn, Pixel row[kMaxImageWidth], int width) {
read_row_cols:
    for (int x = 0; x < width; ++x) {
#pragma HLS PIPELINE II=1
        row[x] = static_cast<Pixel>(static_cast<std::uint32_t>(strmIn.read()) & 0xffu);
    }
}

// Compute one blurred output row from the current 5-row Gaussian neighborhood.
void gaussian_blur_row(const Pixel row_m2[kMaxImageWidth],
                       const Pixel row_m1[kMaxImageWidth],
                       const Pixel row_0[kMaxImageWidth],
                       const Pixel row_p1[kMaxImageWidth],
                       const Pixel row_p2[kMaxImageWidth],
                       BlurPixel blurred_row[kMaxImageWidth],
                       int width) {
    const Pixel* rows[kGaussianKernelSize] = {row_m2, row_m1, row_0, row_p1, row_p2};

blur_cols:
    for (int x = 0; x < width; ++x) {
#pragma HLS PIPELINE II=1
        float sum = 0.0f;
    blur_ky:
        for (int ky = 0; ky < kGaussianKernelSize; ++ky) {
        blur_kx:
            for (int kx = 0; kx < kGaussianKernelSize; ++kx) {
#pragma HLS UNROLL
                const int sx = clamp_int(x + kx - 2, 0, width - 1);
                sum += static_cast<float>(rows[ky][sx]) * kGaussianKernel2D[ky][kx];
            }
        }
        const float scaled = sum * static_cast<float>(kBlurScale);
        blurred_row[x] = static_cast<BlurPixel>(scaled + 0.5f);
    }
}

void sobel_row_hw(const BlurPixel prev_row[kMaxImageWidth],
                  const BlurPixel curr_row[kMaxImageWidth],
                  const BlurPixel next_row[kMaxImageWidth],
                  int width,
                  Magnitude magnitude_row[kMaxImageWidth],
                  Direction direction_row[kMaxImageWidth]) {
    const BlurPixel* rows[3] = {prev_row, curr_row, next_row};

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
                const int sx = clamp_int(x + kx - 1, 0, width - 1);
                const float sample = static_cast<float>(rows[ky][sx]) / static_cast<float>(kBlurScale);
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

// Fill a zero-valued threshold row for image borders.
void fill_zero_row(Pixel row[kMaxImageWidth], int width) {
fill_zero_cols:
    for (int x = 0; x < width; ++x) {
#pragma HLS PIPELINE II=1
        row[x] = kNoEdgeLocal;
    }
}

// Run NMS and thresholding for one row once the neighboring Sobel rows exist.
void fill_threshold_row(Pixel row[kMaxImageWidth],
                        const Magnitude prev_mag[kMaxImageWidth],
                        const Magnitude curr_mag[kMaxImageWidth],
                        const Magnitude next_mag[kMaxImageWidth],
                        const Direction curr_dir[kMaxImageWidth],
                        int width,
                        float low_threshold,
                        float high_threshold) {
fill_threshold_cols:
    for (int x = 0; x < width; ++x) {
#pragma HLS PIPELINE II=1
        Pixel pixel = kNoEdgeLocal;
        if (x > 0 && x < width - 1) {
            pixel = nms_threshold_pixel(prev_mag, curr_mag, next_mag, curr_dir, x, low_threshold, high_threshold);
        }
        row[x] = pixel;
    }
}

// Write the buffered thresholded image after all input pixels have been consumed.
void write_output_stream(DutStream& strmOut,
                         const Pixel thresholded[kMaxImageHeight][kMaxImageWidth],
                         int width,
                         int height) {
    strmOut.write(static_cast<DutWord>(kDutProtocolVersion));
    strmOut.write(static_cast<DutWord>(width));
    strmOut.write(static_cast<DutWord>(height));

write_rows:
    for (int y = 0; y < height; ++y) {
    write_cols:
        for (int x = 0; x < width; ++x) {
#pragma HLS PIPELINE II=1
            strmOut.write(static_cast<DutWord>(thresholded[y][x]));
        }
    }
}

// Stream stages 1-4 end-to-end using rolling row buffers instead of full frames.
void write_thresholded_stream(DutStream& strmIn,
                              DutStream& strmOut,
                              int width,
                              int height,
                              float low_threshold,
                              float high_threshold) {
    Pixel input_rows[5][kMaxImageWidth];
    Pixel last_actual_row[kMaxImageWidth];
    Pixel thresholded[kMaxImageHeight][kMaxImageWidth];
    BlurPixel blurred_rows[3][kMaxImageWidth];
    Magnitude mag_rows[3][kMaxImageWidth];
    Direction dir_rows[3][kMaxImageWidth];

    if (height <= 0 || width <= 0) {
        return;
    }

    read_row(strmIn, input_rows[2], width);
    copy_row(input_rows[2], input_rows[0], width);
    copy_row(input_rows[2], input_rows[1], width);
    copy_row(input_rows[2], last_actual_row, width);

    if (height > 1) {
        read_row(strmIn, input_rows[3], width);
        copy_row(input_rows[3], last_actual_row, width);
    } else {
        copy_row(input_rows[2], input_rows[3], width);
    }

    if (height > 2) {
        read_row(strmIn, input_rows[4], width);
        copy_row(input_rows[4], last_actual_row, width);
    } else {
        copy_row(last_actual_row, input_rows[4], width);
    }

    int next_input_row = 3;

    fill_zero_row(thresholded[0], width);
    if (height == 1) {
        write_output_stream(strmOut, thresholded, width, height);
        return;
    }

blur_rows:
    for (int y = 0; y < height; ++y) {
        gaussian_blur_row(input_rows[0], input_rows[1], input_rows[2], input_rows[3], input_rows[4], blurred_rows[y % 3], width);

        if (y > 0) {
            const int sobel_y = y - 1;
            const int prev_slot = (sobel_y == 0) ? 0 : (sobel_y - 1) % 3;
            const int curr_slot = sobel_y % 3;
            const int next_slot = (sobel_y + 1) % 3;
            sobel_row_hw(blurred_rows[prev_slot], blurred_rows[curr_slot], blurred_rows[next_slot], width, mag_rows[curr_slot], dir_rows[curr_slot]);

            if (sobel_y > 1) {
                const int output_y = sobel_y - 1;
                fill_threshold_row(thresholded[output_y],
                                   mag_rows[(output_y - 1) % 3],
                                   mag_rows[output_y % 3],
                                   mag_rows[(output_y + 1) % 3],
                                   dir_rows[output_y % 3],
                                   width,
                                   low_threshold,
                                   high_threshold);
            }
        }

        if (y != height - 1) {
            copy_row(input_rows[1], input_rows[0], width);
            copy_row(input_rows[2], input_rows[1], width);
            copy_row(input_rows[3], input_rows[2], width);
            copy_row(input_rows[4], input_rows[3], width);

            if (next_input_row < height) {
                read_row(strmIn, input_rows[4], width);
                copy_row(input_rows[4], last_actual_row, width);
                ++next_input_row;
            } else {
                copy_row(last_actual_row, input_rows[4], width);
            }
        }
    }

    if (height > 2) {
        const int last_row = height - 1;
        sobel_row_hw(blurred_rows[(last_row - 1) % 3],
                     blurred_rows[last_row % 3],
                     blurred_rows[last_row % 3],
                     width,
                     mag_rows[last_row % 3],
                     dir_rows[last_row % 3]);

        fill_threshold_row(thresholded[height - 2],
                           mag_rows[(height - 3) % 3],
                           mag_rows[(height - 2) % 3],
                           mag_rows[(height - 1) % 3],
                           dir_rows[(height - 2) % 3],
                           width,
                           low_threshold,
                           high_threshold);
    }

    fill_zero_row(thresholded[height - 1], width);
    write_output_stream(strmOut, thresholded, width, height);
}

}  // namespace

void dut(DutStream& strmIn, DutStream& strmOut) {
#pragma HLS INTERFACE ap_ctrl_hs port=return
#pragma HLS INTERFACE axis port=strmIn
#pragma HLS INTERFACE axis port=strmOut

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

    write_thresholded_stream(strmIn, strmOut, width, height, low_threshold, high_threshold);
}
