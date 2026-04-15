#include "dut.hpp"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <stdexcept>

#include "gaussian.hpp"
#include "image_io.hpp"
#include "nms.hpp"
#include "sobel.hpp"
#include "threshold.hpp"

namespace {

ImageU8 run_reference(const ImageU8& input, float low, float high) {
    const ImageF32 input_f32 = u8_to_f32(input);
    const ImageF32 blurred = gaussian_blur(input_f32, 5, 1.4f);
    const GradientData gradients = compute_sobel_gradients(blurred);
    const ImageF32 suppressed = non_maximum_suppression(gradients.magnitude, gradients.direction);
    return double_threshold(suppressed, low, high);
}

ImageU8 run_dut_model(const ImageU8& input, float low, float high) {
    DutStream in_stream;
    DutStream out_stream;

    in_stream.write(static_cast<DutWord>(kDutProtocolVersion));
    in_stream.write(static_cast<DutWord>(input.width));
    in_stream.write(static_cast<DutWord>(input.height));
    in_stream.write(static_cast<DutWord>(static_cast<std::uint32_t>(std::lround(low))));
    in_stream.write(static_cast<DutWord>(static_cast<std::uint32_t>(std::lround(high))));
    for (std::uint8_t pixel : input.pixels) {
        in_stream.write(static_cast<DutWord>(pixel));
    }

    dut(in_stream, out_stream);

    const std::uint32_t version = static_cast<std::uint32_t>(out_stream.read());
    const int width = static_cast<int>(static_cast<std::uint32_t>(out_stream.read()));
    const int height = static_cast<int>(static_cast<std::uint32_t>(out_stream.read()));
    if (version != kDutProtocolVersion || width != input.width || height != input.height) {
        throw std::runtime_error("DUT returned an unexpected header.");
    }

    ImageU8 output(width, height);
    for (auto& pixel : output.pixels) {
        pixel = static_cast<std::uint8_t>(static_cast<std::uint32_t>(out_stream.read()) & 0xffu);
    }
    return output;
}

void check_image(const std::string& path, float low, float high) {
    const ImageU8 input = read_pgm(path);
    const ImageU8 expected = run_reference(input, low, high);
    const ImageU8 actual = run_dut_model(input, low, high);

    if (expected.width != actual.width || expected.height != actual.height || expected.pixels != actual.pixels) {
        std::cerr << "Mismatch for " << path << "\n";
        throw std::runtime_error("DUT output did not match the software reference.");
    }

    std::cout << "Validated " << path << " (" << input.width << "x" << input.height << ")\n";
}

}  // namespace

int main() {
    check_image("input/bird.pgm", 20.0f, 60.0f);
    check_image("input/mona_lisa.pgm", 20.0f, 60.0f);
    std::cout << "All DUT tests passed.\n";
    return 0;
}
