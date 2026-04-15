#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>

#include "dut.hpp"
#include "hysteresis.hpp"
#include "image_io.hpp"

namespace {

struct Options {
    std::string input_path;
    std::string output_path;
    std::uint32_t low_threshold = 20;
    std::uint32_t high_threshold = 60;
};

void usage() {
    std::cout << "Usage: edge_detector_fpga <input.pgm> <output.pgm> [--low value] [--high value]\n";
}

Options parse_args(int argc, char** argv) {
    if (argc < 3) {
        usage();
        throw std::runtime_error("Missing required arguments.");
    }

    Options options;
    options.input_path = argv[1];
    options.output_path = argv[2];
    for (int i = 3; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--low" && i + 1 < argc) {
            options.low_threshold = static_cast<std::uint32_t>(std::stoul(argv[++i]));
        } else if (arg == "--high" && i + 1 < argc) {
            options.high_threshold = static_cast<std::uint32_t>(std::stoul(argv[++i]));
        } else {
            usage();
            throw std::runtime_error("Unknown or incomplete argument: " + arg);
        }
    }
    return options;
}

void write_word(int fdw, std::uint32_t value) {
    const int nbytes = write(fdw, &value, sizeof(value));
    if (nbytes != static_cast<int>(sizeof(value))) {
        throw std::runtime_error("Failed to write to CiFra OpenBus: " + std::string(std::strerror(errno)));
    }
}

std::uint32_t read_word(int fdr) {
    std::uint32_t value = 0;
    const int nbytes = read(fdr, &value, sizeof(value));
    if (nbytes != static_cast<int>(sizeof(value))) {
        throw std::runtime_error("Failed to read from CiFra OpenBus: " + std::string(std::strerror(errno)));
    }
    return value;
}

}  // namespace

int main(int argc, char** argv) {
    try {
        const Options options = parse_args(argc, argv);
        const ImageU8 input = read_pgm(options.input_path);
        if (input.width > kMaxImageWidth || input.height > kMaxImageHeight) {
            throw std::runtime_error("Input image exceeds FPGA maximum supported size.");
        }

        const int fdr = open("/dev/cifra_openbus_read_32", O_RDONLY);
        const int fdw = open("/dev/cifra_openbus_write_32", O_WRONLY);
        if (fdr < 0 || fdw < 0) {
            throw std::runtime_error("Failed to open /dev/cifra_openbus_* device files.");
        }

        write_word(fdw, kDutProtocolVersion);
        write_word(fdw, static_cast<std::uint32_t>(input.width));
        write_word(fdw, static_cast<std::uint32_t>(input.height));
        write_word(fdw, options.low_threshold);
        write_word(fdw, options.high_threshold);
        for (std::uint8_t pixel : input.pixels) {
            write_word(fdw, static_cast<std::uint32_t>(pixel));
        }

        const std::uint32_t version = read_word(fdr);
        const int width = static_cast<int>(read_word(fdr));
        const int height = static_cast<int>(read_word(fdr));
        if (version != kDutProtocolVersion || width != input.width || height != input.height) {
            throw std::runtime_error("FPGA returned an invalid response header.");
        }

        ImageU8 thresholded(width, height);
        for (auto& pixel : thresholded.pixels) {
            pixel = static_cast<std::uint8_t>(read_word(fdr) & 0xffu);
        }

        const ImageU8 edges = hysteresis(thresholded);
        write_pgm(options.output_path, edges);

        close(fdr);
        close(fdw);
        std::cout << "FPGA edge detection complete. Output written to " << options.output_path << "\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n";
        return 1;
    }
}
