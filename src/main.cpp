#include <exception>
#include <iostream>
#include <string>

#include "gaussian.hpp"
#include "hysteresis.hpp"
#include "image_io.hpp"
#include "nms.hpp"
#include "sobel.hpp"
#include "threshold.hpp"

namespace {

struct Options {
    std::string input_path;
    std::string output_path;
    float low_threshold = 40.0f;
    float high_threshold = 100.0f;
    int kernel_size = 5;
    float sigma = 1.4f;
    std::string dump_prefix;
};

void print_usage() {
    std::cout
        << "Usage:\n"
        << "  edge_detector <input.pgm> <output.pgm> [--low value] [--high value] [--kernel odd] [--sigma value] [--dump-prefix prefix]\n";
}

Options parse_args(int argc, char* argv[]) {
    if (argc < 3) {
        print_usage();
        throw std::runtime_error("Missing required arguments.");
    }

    Options options;
    options.input_path = argv[1];
    options.output_path = argv[2];

    for (int i = 3; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--low" && i + 1 < argc) {
            options.low_threshold = std::stof(argv[++i]);
        } else if (arg == "--high" && i + 1 < argc) {
            options.high_threshold = std::stof(argv[++i]);
        } else if (arg == "--kernel" && i + 1 < argc) {
            options.kernel_size = std::stoi(argv[++i]);
        } else if (arg == "--sigma" && i + 1 < argc) {
            options.sigma = std::stof(argv[++i]);
        } else if (arg == "--dump-prefix" && i + 1 < argc) {
            options.dump_prefix = argv[++i];
        } else {
            print_usage();
            throw std::runtime_error("Unknown or incomplete argument: " + arg);
        }
    }

    return options;
}

void maybe_dump_image(const std::string& prefix, const std::string& suffix, const ImageU8& image) {
    if (!prefix.empty()) {
        write_pgm(prefix + "_" + suffix + ".pgm", image);
    }
}

}

int main(int argc, char* argv[]) {
    try {
        const Options options = parse_args(argc, argv);

        const ImageU8 input_u8 = read_pgm(options.input_path);
        const ImageF32 input_f32 = u8_to_f32(input_u8);
        const ImageF32 blurred = gaussian_blur(input_f32, options.kernel_size, options.sigma);
        const GradientData gradients = compute_sobel_gradients(blurred);
        const ImageF32 suppressed = non_maximum_suppression(gradients.magnitude, gradients.direction);
        const ImageU8 thresholded = double_threshold(suppressed, options.low_threshold, options.high_threshold);
        const ImageU8 edges = hysteresis(thresholded);

        write_pgm(options.output_path, edges);
        maybe_dump_image(options.dump_prefix, "blur", f32_to_u8_clamped(blurred));
        maybe_dump_image(options.dump_prefix, "gradient", f32_to_u8_normalized(gradients.magnitude));
        maybe_dump_image(options.dump_prefix, "nms", f32_to_u8_normalized(suppressed));
        maybe_dump_image(options.dump_prefix, "threshold", thresholded);

        std::cout << "Edge detection complete.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n";
        return 1;
    }
}
