#ifndef EDGE_DETECTOR_DUT_HPP
#define EDGE_DETECTOR_DUT_HPP

#include <cstdint>

#include <ap_int.h>
#include <hls_stream.h>

constexpr int kMaxImageWidth = 321;
constexpr int kMaxImageHeight = 481;
constexpr int kGaussianKernelSize = 5;
constexpr std::uint32_t kDutProtocolVersion = 1;

using DutWord = ap_uint<32>;
using DutStream = hls::stream<DutWord>;

void dut(DutStream& strmIn, DutStream& strmOut);

#endif
