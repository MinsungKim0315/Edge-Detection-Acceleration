#ifndef EDGE_DETECTOR_IMAGE_IO_HPP
#define EDGE_DETECTOR_IMAGE_IO_HPP

#include <string>

#include "image.hpp"

ImageU8 read_pgm(const std::string& path);
void write_pgm(const std::string& path, const ImageU8& image);
ImageF32 u8_to_f32(const ImageU8& image);
ImageU8 f32_to_u8_clamped(const ImageF32& image);
ImageU8 f32_to_u8_normalized(const ImageF32& image);

#endif
