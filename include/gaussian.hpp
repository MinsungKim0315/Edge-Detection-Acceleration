#ifndef EDGE_DETECTOR_GAUSSIAN_HPP
#define EDGE_DETECTOR_GAUSSIAN_HPP

#include <vector>

#include "image.hpp"

std::vector<float> make_gaussian_kernel_1d(int kernel_size, float sigma);
ImageF32 gaussian_blur(const ImageF32& input, int kernel_size, float sigma);

#endif
