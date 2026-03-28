#ifndef EDGE_DETECTOR_THRESHOLD_HPP
#define EDGE_DETECTOR_THRESHOLD_HPP

#include <cstdint>

#include "image.hpp"

constexpr std::uint8_t kNoEdge = 0;
constexpr std::uint8_t kWeakEdge = 75;
constexpr std::uint8_t kStrongEdge = 255;

ImageU8 double_threshold(const ImageF32& input, float low_threshold, float high_threshold);

#endif
