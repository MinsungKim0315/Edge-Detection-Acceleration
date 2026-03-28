#include "nms.hpp"

#include <stdexcept>

ImageF32 non_maximum_suppression(const ImageF32& magnitude, const ImageU8& direction) {
    if (magnitude.width != direction.width || magnitude.height != direction.height) {
        throw std::runtime_error("Magnitude and direction images must have matching dimensions.");
    }

    ImageF32 output(magnitude.width, magnitude.height, 0.0f);

    for (int y = 1; y < magnitude.height - 1; ++y) {
        for (int x = 1; x < magnitude.width - 1; ++x) {
            const float center = magnitude.at(x, y);
            float neighbor_a = 0.0f;
            float neighbor_b = 0.0f;

            switch (direction.at(x, y)) {
                case 0:
                    neighbor_a = magnitude.at(x - 1, y);
                    neighbor_b = magnitude.at(x + 1, y);
                    break;
                case 45:
                    neighbor_a = magnitude.at(x - 1, y + 1);
                    neighbor_b = magnitude.at(x + 1, y - 1);
                    break;
                case 90:
                    neighbor_a = magnitude.at(x, y - 1);
                    neighbor_b = magnitude.at(x, y + 1);
                    break;
                case 135:
                    neighbor_a = magnitude.at(x - 1, y - 1);
                    neighbor_b = magnitude.at(x + 1, y + 1);
                    break;
                default:
                    throw std::runtime_error("Unexpected quantized gradient direction.");
            }

            if (center >= neighbor_a && center >= neighbor_b) {
                output.at(x, y) = center;
            }
        }
    }

    return output;
}
