#include "gaussian.hpp"

#include <cmath>
#include <stdexcept>

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

ImageF32 convolve_horizontal(const ImageF32& input, const std::vector<float>& kernel) {
    const int radius = static_cast<int>(kernel.size() / 2);
    ImageF32 output(input.width, input.height);

    for (int y = 0; y < input.height; ++y) {
        for (int x = 0; x < input.width; ++x) {
            float sum = 0.0f;
            for (int k = -radius; k <= radius; ++k) {
                const int sample_x = clamp_index(x + k, 0, input.width - 1);
                sum += input.at(sample_x, y) * kernel[static_cast<std::size_t>(k + radius)];
            }
            output.at(x, y) = sum;
        }
    }

    return output;
}

ImageF32 convolve_vertical(const ImageF32& input, const std::vector<float>& kernel) {
    const int radius = static_cast<int>(kernel.size() / 2);
    ImageF32 output(input.width, input.height);

    for (int y = 0; y < input.height; ++y) {
        for (int x = 0; x < input.width; ++x) {
            float sum = 0.0f;
            for (int k = -radius; k <= radius; ++k) {
                const int sample_y = clamp_index(y + k, 0, input.height - 1);
                sum += input.at(x, sample_y) * kernel[static_cast<std::size_t>(k + radius)];
            }
            output.at(x, y) = sum;
        }
    }

    return output;
}

}

std::vector<float> make_gaussian_kernel_1d(int kernel_size, float sigma) {
    if (kernel_size <= 0 || (kernel_size % 2) == 0) {
        throw std::runtime_error("Gaussian kernel size must be a positive odd integer.");
    }
    if (sigma <= 0.0f) {
        throw std::runtime_error("Gaussian sigma must be positive.");
    }

    std::vector<float> kernel(static_cast<std::size_t>(kernel_size), 0.0f);
    const int radius = kernel_size / 2;
    const float sigma_sq = sigma * sigma;
    float sum = 0.0f;

    for (int i = -radius; i <= radius; ++i) {
        const float x = static_cast<float>(i);
        const float value = std::exp(-(x * x) / (2.0f * sigma_sq));
        kernel[static_cast<std::size_t>(i + radius)] = value;
        sum += value;
    }

    for (float& value : kernel) {
        value /= sum;
    }

    return kernel;
}

ImageF32 gaussian_blur(const ImageF32& input, int kernel_size, float sigma) {
    const std::vector<float> kernel = make_gaussian_kernel_1d(kernel_size, sigma);
    return convolve_vertical(convolve_horizontal(input, kernel), kernel);
}
