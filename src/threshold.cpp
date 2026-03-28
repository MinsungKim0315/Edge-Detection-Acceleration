#include "threshold.hpp"

#include <stdexcept>

ImageU8 double_threshold(const ImageF32& input, float low_threshold, float high_threshold) {
    if (low_threshold < 0.0f || high_threshold < 0.0f) {
        throw std::runtime_error("Thresholds must be non-negative.");
    }
    if (low_threshold > high_threshold) {
        throw std::runtime_error("Low threshold must be less than or equal to high threshold.");
    }

    ImageU8 output(input.width, input.height, kNoEdge);

    for (int y = 0; y < input.height; ++y) {
        for (int x = 0; x < input.width; ++x) {
            const float value = input.at(x, y);
            if (value >= high_threshold) {
                output.at(x, y) = kStrongEdge;
            } else if (value >= low_threshold) {
                output.at(x, y) = kWeakEdge;
            }
        }
    }

    return output;
}
