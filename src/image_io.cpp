#include "image_io.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace {

std::string next_token(std::istream& input) {
    std::string token;
    while (input >> token) {
        if (!token.empty() && token[0] == '#') {
            input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        return token;
    }
    throw std::runtime_error("Unexpected end of file while parsing PGM.");
}

std::uint8_t clamp_to_u8(float value) {
    const float rounded = std::round(value);
    if (rounded < 0.0f) {
        return 0;
    }
    if (rounded > 255.0f) {
        return 255;
    }
    return static_cast<std::uint8_t>(rounded);
}

}

ImageU8 read_pgm(const std::string& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("Failed to open input image: " + path);
    }

    const std::string magic = next_token(input);
    if (magic != "P2" && magic != "P5") {
        throw std::runtime_error("Unsupported PGM format. Only P2 and P5 are supported.");
    }

    const int width = std::stoi(next_token(input));
    const int height = std::stoi(next_token(input));
    const int max_value = std::stoi(next_token(input));

    if (width <= 0 || height <= 0) {
        throw std::runtime_error("Invalid image dimensions.");
    }
    if (max_value <= 0 || max_value > 255) {
        throw std::runtime_error("Only 8-bit PGM images are supported.");
    }

    ImageU8 image(width, height);
    const std::size_t count = image.pixels.size();

    if (magic == "P2") {
        for (std::size_t i = 0; i < count; ++i) {
            const int sample = std::stoi(next_token(input));
            if (sample < 0 || sample > max_value) {
                throw std::runtime_error("PGM sample out of range.");
            }
            image.pixels[i] = static_cast<std::uint8_t>((sample * 255) / max_value);
        }
    } else {
        input.get();
        std::vector<char> buffer(count);
        input.read(buffer.data(), static_cast<std::streamsize>(count));
        if (input.gcount() != static_cast<std::streamsize>(count)) {
            throw std::runtime_error("Unexpected end of file while reading P5 data.");
        }
        for (std::size_t i = 0; i < count; ++i) {
            const std::uint8_t sample = static_cast<std::uint8_t>(buffer[i]);
            image.pixels[i] = static_cast<std::uint8_t>((static_cast<int>(sample) * 255) / max_value);
        }
    }

    return image;
}

void write_pgm(const std::string& path, const ImageU8& image) {
    std::ofstream output(path, std::ios::binary);
    if (!output) {
        throw std::runtime_error("Failed to open output image: " + path);
    }

    output << "P5\n" << image.width << " " << image.height << "\n255\n";
    output.write(reinterpret_cast<const char*>(image.pixels.data()), static_cast<std::streamsize>(image.pixels.size()));
    if (!output) {
        throw std::runtime_error("Failed while writing output image: " + path);
    }
}

ImageF32 u8_to_f32(const ImageU8& image) {
    ImageF32 output(image.width, image.height);
    for (std::size_t i = 0; i < image.pixels.size(); ++i) {
        output.pixels[i] = static_cast<float>(image.pixels[i]);
    }
    return output;
}

ImageU8 f32_to_u8_clamped(const ImageF32& image) {
    ImageU8 output(image.width, image.height);
    for (std::size_t i = 0; i < image.pixels.size(); ++i) {
        output.pixels[i] = clamp_to_u8(image.pixels[i]);
    }
    return output;
}

ImageU8 f32_to_u8_normalized(const ImageF32& image) {
    ImageU8 output(image.width, image.height);
    if (image.pixels.empty()) {
        return output;
    }

    const auto minmax = std::minmax_element(image.pixels.begin(), image.pixels.end());
    const float min_value = *minmax.first;
    const float max_value = *minmax.second;

    if (std::abs(max_value - min_value) < 1e-6f) {
        std::fill(output.pixels.begin(), output.pixels.end(), 0);
        return output;
    }

    const float scale = 255.0f / (max_value - min_value);
    for (std::size_t i = 0; i < image.pixels.size(); ++i) {
        output.pixels[i] = clamp_to_u8((image.pixels[i] - min_value) * scale);
    }
    return output;
}
