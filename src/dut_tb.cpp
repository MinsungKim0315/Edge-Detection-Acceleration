#include "dut.hpp"

#include <algorithm>
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

#ifndef INPUT_DIR
#define INPUT_DIR "input"
#endif

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

    if (expected.width != actual.width || expected.height != actual.height) {
        std::cerr << "Dimension mismatch for " << path << "\n";
        std::cerr << "Expected: " << expected.width << "x" << expected.height
                  << ", Actual: " << actual.width << "x" << actual.height << "\n";
        throw std::runtime_error("DUT output dimensions did not match the software reference.");
    }

    if (expected.pixels != actual.pixels) {
        int mismatch_count = 0;
        int first_x = -1;
        int first_y = -1;
        for (int y = 0; y < expected.height; ++y) {
            for (int x = 0; x < expected.width; ++x) {
                const int idx = y * expected.width + x;
                if (expected.pixels[idx] != actual.pixels[idx]) {
                    ++mismatch_count;
                    if (first_x < 0) {
                        first_x = x;
                        first_y = y;
                    }
                }
            }
        }

        const int row_start = std::max(0, first_y - 4);
        const int row_end = std::min(expected.height, first_y + 5);
        const int col_start = std::max(0, first_x - 4);
        const int col_end = std::min(expected.width, first_x + 5);

        std::cerr << "Mismatch for " << path << "\n";
        std::cerr << "Total mismatches: " << mismatch_count << " / "
                  << expected.pixels.size() << "\n";
        std::cerr << "First mismatch at (" << first_x << ", " << first_y << ")"
                  << " expected="
                  << static_cast<int>(expected.pixels[first_y * expected.width + first_x])
                  << " actual="
                  << static_cast<int>(actual.pixels[first_y * expected.width + first_x])
                  << "\n";

        std::cerr << "Expected window:\n";
        for (int y = row_start; y < row_end; ++y) {
            for (int x = col_start; x < col_end; ++x) {
                std::cerr << static_cast<int>(expected.pixels[y * expected.width + x]) << ' ';
            }
            std::cerr << "\n";
        }

        std::cerr << "Actual window:\n";
        for (int y = row_start; y < row_end; ++y) {
            for (int x = col_start; x < col_end; ++x) {
                std::cerr << static_cast<int>(actual.pixels[y * expected.width + x]) << ' ';
            }
            std::cerr << "\n";
        }

        std::cerr << "Diff window (1 = mismatch, 0 = match):\n";
        for (int y = row_start; y < row_end; ++y) {
            for (int x = col_start; x < col_end; ++x) {
                const int idx = y * expected.width + x;
                std::cerr << (expected.pixels[idx] != actual.pixels[idx] ? 1 : 0) << ' ';
            }
            std::cerr << "\n";
        }

        throw std::runtime_error("DUT output did not match the software reference.");
    }

    std::cout << "Validated " << path << " (" << input.width << "x" << input.height << ")\n";
}

}  // namespace

int main() {
    const std::string input_dir = INPUT_DIR;
    check_image(input_dir + "/bird.pgm", 20.0f, 60.0f);
    check_image(input_dir + "/mona_lisa.pgm", 20.0f, 60.0f);
    std::cout << "All DUT tests passed.\n";
    return 0;
}
