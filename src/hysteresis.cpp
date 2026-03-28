#include "hysteresis.hpp"

#include <array>
#include <queue>

#include "threshold.hpp"

ImageU8 hysteresis(const ImageU8& thresholded) {
    ImageU8 output = thresholded;
    std::queue<std::pair<int, int>> frontier;

    for (int y = 0; y < output.height; ++y) {
        for (int x = 0; x < output.width; ++x) {
            if (output.at(x, y) == kStrongEdge) {
                frontier.emplace(x, y);
            }
        }
    }

    constexpr std::array<int, 8> dx = {-1, 0, 1, -1, 1, -1, 0, 1};
    constexpr std::array<int, 8> dy = {-1, -1, -1, 0, 0, 1, 1, 1};

    while (!frontier.empty()) {
        const auto [x, y] = frontier.front();
        frontier.pop();

        for (std::size_t i = 0; i < dx.size(); ++i) {
            const int nx = x + dx[i];
            const int ny = y + dy[i];
            if (nx < 0 || ny < 0 || nx >= output.width || ny >= output.height) {
                continue;
            }
            if (output.at(nx, ny) == kWeakEdge) {
                output.at(nx, ny) = kStrongEdge;
                frontier.emplace(nx, ny);
            }
        }
    }

    for (std::uint8_t& pixel : output.pixels) {
        if (pixel != kStrongEdge) {
            pixel = kNoEdge;
        }
    }

    return output;
}
