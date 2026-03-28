#include "sobel.hpp"

#include <cmath>

namespace {

int clamp_index(int value, int low, int high) {
    if (value < low) {
        return low;
    }
    if (value > high) {
        return high;
    }
    return value;
}

std::uint8_t quantize_direction(float angle_radians) {
    float angle_degrees = angle_radians * 180.0f / 3.14159265358979323846f;
    if (angle_degrees < 0.0f) {
        angle_degrees += 180.0f;
    }

    if ((angle_degrees >= 0.0f && angle_degrees < 22.5f) || (angle_degrees >= 157.5f && angle_degrees <= 180.0f)) {
        return 0;
    }
    if (angle_degrees >= 22.5f && angle_degrees < 67.5f) {
        return 45;
    }
    if (angle_degrees >= 67.5f && angle_degrees < 112.5f) {
        return 90;
    }
    return 135;
}

}

GradientData compute_sobel_gradients(const ImageF32& input) {
    GradientData result{
        ImageF32(input.width, input.height),
        ImageF32(input.width, input.height),
        ImageF32(input.width, input.height),
        ImageU8(input.width, input.height)
    };

    constexpr int kernel_x[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    constexpr int kernel_y[3][3] = {
        {1, 2, 1},
        {0, 0, 0},
        {-1, -2, -1}
    };

    for (int y = 0; y < input.height; ++y) {
        for (int x = 0; x < input.width; ++x) {
            float gx = 0.0f;
            float gy = 0.0f;

            for (int ky = -1; ky <= 1; ++ky) {
                for (int kx = -1; kx <= 1; ++kx) {
                    const int sx = clamp_index(x + kx, 0, input.width - 1);
                    const int sy = clamp_index(y + ky, 0, input.height - 1);
                    const float sample = input.at(sx, sy);
                    gx += sample * static_cast<float>(kernel_x[ky + 1][kx + 1]);
                    gy += sample * static_cast<float>(kernel_y[ky + 1][kx + 1]);
                }
            }

            result.grad_x.at(x, y) = gx;
            result.grad_y.at(x, y) = gy;
            result.magnitude.at(x, y) = std::sqrt((gx * gx) + (gy * gy));
            result.direction.at(x, y) = quantize_direction(std::atan2(gy, gx));
        }
    }

    return result;
}
