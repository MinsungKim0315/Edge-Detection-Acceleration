#ifndef EDGE_DETECTOR_NMS_HPP
#define EDGE_DETECTOR_NMS_HPP

#include "image.hpp"

ImageF32 non_maximum_suppression(const ImageF32& magnitude, const ImageU8& direction);

#endif
