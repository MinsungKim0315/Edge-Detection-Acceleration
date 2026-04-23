#include <chrono>
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
        using Clock = std::chrono::high_resolution_clock;
        const auto total_start = Clock::now();

        const Options options = parse_args(argc, argv);

        std::cout << "[host] Reading input image...\n";
        const auto input_read_start = Clock::now();
        const ImageU8 input = read_pgm(options.input_path);
        const auto input_read_end = Clock::now();
        std::cout << "[host] Input image loaded: " << input.width << "x" << input.height << "\n";
        if (input.width > kMaxImageWidth || input.height > kMaxImageHeight) {
            throw std::runtime_error("Input image exceeds FPGA maximum supported size.");
        }

        std::cout << "[host] Opening CiFra OpenBus device files...\n";
        const int fdr = open("/dev/cifra_openbus_read_32", O_RDONLY);
        const int fdw = open("/dev/cifra_openbus_write_32", O_WRONLY);
        if (fdr < 0 || fdw < 0) {
            throw std::runtime_error("Failed to open /dev/cifra_openbus_* device files.");
        }
        std::cout << "[host] Device files opened successfully.\n";

        std::cout << "[host] Writing request header to FPGA...\n";
        const auto fpga_write_start = Clock::now();
        write_word(fdw, kDutProtocolVersion);
        write_word(fdw, static_cast<std::uint32_t>(input.width));
        write_word(fdw, static_cast<std::uint32_t>(input.height));
        write_word(fdw, options.low_threshold);
        write_word(fdw, options.high_threshold);
        std::cout << "[host] Streaming " << input.pixels.size() << " input pixels to FPGA...\n";
        for (std::uint8_t pixel : input.pixels) {
            write_word(fdw, static_cast<std::uint32_t>(pixel));
        }
        const auto fpga_write_end = Clock::now();
        std::cout << "[host] Finished sending all input pixels.\n";

        std::cout << "[host] Reading response header from FPGA...\n";
        const auto fpga_read_start = Clock::now();
        const std::uint32_t version = read_word(fdr);
        const int width = static_cast<int>(read_word(fdr));
        const int height = static_cast<int>(read_word(fdr));
        std::cout << "[host] Response header received: version=" << version
                  << ", width=" << width << ", height=" << height << "\n";
        if (version != kDutProtocolVersion || width != input.width || height != input.height) {
            throw std::runtime_error("FPGA returned an invalid response header.");
        }

        ImageU8 thresholded(width, height);
        std::cout << "[host] Reading " << thresholded.pixels.size() << " output pixels from FPGA...\n";
        for (auto& pixel : thresholded.pixels) {
            pixel = static_cast<std::uint8_t>(read_word(fdr) & 0xffu);
        }
        const auto fpga_read_end = Clock::now();
        std::cout << "[host] Finished receiving FPGA output.\n";

        std::cout << "[host] Running hysteresis on ARM CPU...\n";
        const auto hysteresis_start = Clock::now();
        const ImageU8 edges = hysteresis(thresholded);
        const auto hysteresis_end = Clock::now();
        std::cout << "[host] Hysteresis complete.\n";

        std::cout << "[host] Writing output image...\n";
        const auto output_write_start = Clock::now();
        write_pgm(options.output_path, edges);
        const auto output_write_end = Clock::now();
        std::cout << "[host] Output image written.\n";

        close(fdr);
        close(fdw);

        const auto total_end = Clock::now();
        const auto to_us = [](const auto& duration) {
            return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
        };

        const auto input_read_us = to_us(input_read_end - input_read_start);
        const auto fpga_write_us = to_us(fpga_write_end - fpga_write_start);
        const auto fpga_read_us = to_us(fpga_read_end - fpga_read_start);
        const auto fpga_total_us = to_us(fpga_read_end - fpga_write_start);
        const auto hysteresis_us = to_us(hysteresis_end - hysteresis_start);
        const auto output_write_us = to_us(output_write_end - output_write_start);
        const auto end_to_end_us = to_us(total_end - total_start);

        std::cout << "Timing breakdown:\n";
        std::cout << "  Input read:         " << input_read_us << " us\n";
        std::cout << "  Host -> FPGA write: " << fpga_write_us << " us\n";
        std::cout << "  FPGA -> Host read:  " << fpga_read_us << " us\n";
        std::cout << "  FPGA transaction:   " << fpga_total_us << " us\n";
        std::cout << "  CPU hysteresis:     " << hysteresis_us << " us\n";
        std::cout << "  Output write:       " << output_write_us << " us\n";
        std::cout << "  End-to-end total:   " << end_to_end_us << " us\n";
        std::cout << "FPGA edge detection complete. Output written to " << options.output_path << "\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n";
        return 1;
    }
}
