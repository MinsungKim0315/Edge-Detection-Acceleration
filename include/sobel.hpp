#ifndef EDGE_DETECTOR_SOBEL_HPP
#define EDGE_DETECTOR_SOBEL_HPP

#include <cstdint>

#include "image.hpp"

struct GradientData {
    ImageF32 magnitude;
    ImageF32 grad_x;
    ImageF32 grad_y;
    ImageU8 direction;
};

GradientData compute_sobel_gradients(const ImageF32& input);

#endif
