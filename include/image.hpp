#ifndef EDGE_DETECTOR_IMAGE_HPP
#define EDGE_DETECTOR_IMAGE_HPP

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

struct ImageU8 {
    int width = 0;
    int height = 0;
    std::vector<std::uint8_t> pixels;

    ImageU8() = default;
    ImageU8(int w, int h, std::uint8_t value = 0)
        : width(w), height(h), pixels(static_cast<std::size_t>(w) * static_cast<std::size_t>(h), value) {}

    std::uint8_t& at(int x, int y) {
        return pixels.at(static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x));
    }

    const std::uint8_t& at(int x, int y) const {
        return pixels.at(static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x));
    }
};

struct ImageF32 {
    int width = 0;
    int height = 0;
    std::vector<float> pixels;

    ImageF32() = default;
    ImageF32(int w, int h, float value = 0.0f)
        : width(w), height(h), pixels(static_cast<std::size_t>(w) * static_cast<std::size_t>(h), value) {}

    float& at(int x, int y) {
        return pixels.at(static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x));
    }

    const float& at(int x, int y) const {
        return pixels.at(static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x));
    }
};

inline void validate_same_shape(const ImageF32& a, const ImageF32& b, const char* message) {
    if (a.width != b.width || a.height != b.height) {
        throw std::runtime_error(message);
    }
}

#endif
