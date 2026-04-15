#==========================================================================
# Makefile
#==========================================================================
# @brief: A makefile the compiles and synthesizes the cordic program
# @author: ECE6775 Cornell University
#          Francesco Ciraolo
#
# @desc: 1. "make" runs csim by default
#        2. "make csim" compiles & executes the fixed-point implementation
#        3. "make clean" cleans up the directory

# Define paths and sources, absolute paths required
ROOT_DIR=$(shell pwd)
SRC_PATH=$(shell pwd)/src
BUILD_PATH=$(shell pwd)/build
HW_DUT_PATH=$(BUILD_PATH)/hw_dut
INCLUDE_PATH=$(shell pwd)/include
INPUT_PATH=$(shell pwd)/input
OUTPUT_PATH=$(shell pwd)/output
# WORKSPACE_PATH=$(shell pwd)/workspace
# DUT_SRC=$(SRC_PATH)/edge_detector.cpp
# TEST_SRC=$(SRC_PATH)/edge_detector_test.cpp
# ALL_SRC=$(DUT_SRC) $(TEST_SRC) $(SRC_PATH)/edge_detector.h
SW_SRCS=$(SRC_PATH)/main.cpp $(SRC_PATH)/image_io.cpp $(SRC_PATH)/gaussian.cpp $(SRC_PATH)/sobel.cpp $(SRC_PATH)/nms.cpp $(SRC_PATH)/threshold.cpp $(SRC_PATH)/hysteresis.cpp
HLS_SRCS=$(SRC_PATH)/dut.cpp
INCLUDES=$(wildcard include/*.hpp)
# DATA_PATH=$(shell pwd)/data
KRIA_DIR=$(shell pwd)/kria_dir
KRIA_SRC=$(shell pwd)/src/kria
RESULTS_DIR=$(shell pwd)/result

# Extract Vivado HLS include path
XILINX_VITIS?=/scratch/Xilinx/2025.1/Vitis
XILINX_VIVADO?=/scratch/Xilinx/2025.1/Vivado
XIL_HLS=source $(XILINX_VITIS)/settings64.sh; vitis-run --mode hls --tcl 	
XIL_VIVADO=source $(XILINX_VIVADO)/settings64.sh; vivado -mode batch -source
VHLS_INC=$(XILINX_VITIS)/include

# CFLAGS = -g -I${VHLS_INC} -I$(SRC_PATH) -DHLS_NO_XIL_FPO_LIB -std=c++11 -O3
# CFLAG for compiling the edge detector
CFLAGS = -std=c++17 -Iinclude -Wall -Wextra -fPIC -O2

# Arguments for csim
CSIM_TARGET  ?= bird
CSIM_INPUT  ?= input/$(CSIM_TARGET).pgm
CSIM_OUTPUT ?= output/$(CSIM_TARGET)_edge.pgm
CSIM_ARGS   ?= --low 20 --high 60 --sigma 1.5

TCL_NAME?=build_dut.tcl

.PHONY: all edge_detector csim bitstream clean

all: bitstream deploy

edge_detector: ${BUILD_PATH}/edge_detector

csim: ${RESULTS_DIR}/edge_detector_csim.txt

dut: ${HW_DUT_PATH}/dut.v

bitstream: ${BUILD_PATH}/bitstream.bit

deploy: ${BUILD_PATH}/bitstream.bit
	mkdir -p ${KRIA_DIR}
	cp ${BUILD_PATH}/bitstream.bit ${KRIA_DIR}/
	cp -r ${SRC_PATH}/* ${KRIA_DIR}/
	cp -r ${KRIA_SRC}/* ${KRIA_DIR}/
	cp -r ${INCLUDE_PATH}/* ${KRIA_DIR}/

clean:
	rm -rf ${BUILD_PATH}
	rm -rf *.dat *.prj *.log *.jou logs/ .Xil/ ${KRIA_DIR}/ ${RESULTS_DIR}

# === Internal targets ===

${BUILD_PATH}/edge_detector: ${SW_SRCS}
	mkdir -p ${BUILD_PATH}
	g++ ${CFLAGS} $^ -o $@ -lrt

${RESULTS_DIR}/edge_detector_csim.txt: ${BUILD_PATH}/edge_detector
# 	@echo "Running edge_detector sim..."
# 	mkdir -p ${RESULTS_DIR}
# 	$< | tee $@
	@if [ ! -f "$(CSIM_INPUT)" ]; then \
		echo "Missing input file: $(CSIM_INPUT)";\
		exit 1; \
	fi
	@echo "Running edge_detector sim... $(CSIM_INPUT) -> $(CSIM_OUTPUT)"
	@mkdir -p ${RESULTS_DIR} $(dir $(CSIM_OUTPUT))
	$< $(CSIM_INPUT) $(CSIM_OUTPUT) $(CSIM_ARGS) 2>&1 | tee $@

${HW_DUT_PATH}/dut.v: ${HLS_SRCS} ${INCLUDES}
	@echo "================================================================="
	@echo "Synthesizing edge_detector with $(TCL_NAME)..."
	@echo "================================================================="
	mkdir -p ${HW_DUT_PATH}
	SRC_PATH=${SRC_PATH} INPUT_PATH=${INPUT_PATH} PROJ_PATH=${ROOT_DIR} INCLUDE_PATH=${INCLUDE_PATH} SKIP_CSIM=1 SKIP_COSIM=1 $(XIL_HLS) artifacts/${TCL_NAME}
	cp edge_detector.prj/solution1/impl/ip/hdl/verilog/* ${HW_DUT_PATH}/


${BUILD_PATH}/bitstream.bit: ${HW_DUT_PATH}/dut.v
	@echo "================================================================="
	@echo "Creating bitstream with Vivado..."
	@echo "================================================================="
	${XIL_VIVADO} artifacts/build_bitstream.tcl
	cp kria.prj/kria.prj.runs/impl_1/design_1_wrapper.bit $@

# digitrec: ${BUILD_PATH}/digitrec

# ${BUILD_PATH}/digitrec: ${DUT_SRC} ${TEST_SRC}
# 	mkdir -p ${BUILD_PATH}
# 	g++ ${CFLAGS} $^ -o $@ -lrt

# ${RESULTS_DIR}/digitrec_csim.txt: ${BUILD_PATH}/digitrec
# 	@echo "Running digitrec sim..."
# 	mkdir -p ${RESULTS_DIR}
# 	$< | tee $@

# # csim: ${RESULTS_DIR}/digitrec_csim.txt
# # 	@echo "Result reorded to $<"

# hw_dut: ${HW_DUT_PATH}
# 	@echo "Synthesizing CORDIC on amdpool ..."
# 	SRC_PATH=${SRC_PATH} TEST_PATH=${SRC_PATH} SKIP_COSIM=1 $(XIL_HLS) artifacts/${TCL_NAME}
# 	cp digitrec.prj/solution1/impl/verilog/* ${HW_DUT_PATH}/

# ${HW_DUT_PATH}:
# 	mkdir -p $@

# ${HW_DUT_PATH}/dut.v: ${DUT_SRC}
# 	@echo "Synthesizing CORDIC on amdpool ..."
# 	SRC_PATH=${SRC_PATH} TEST_PATH=${SRC_PATH} SKIP_COSIM=1 $(XIL_HLS) artifacts/${TCL_NAME}
# 	cp digitrec.prj/solution1/impl/verilog/* ${HW_DUT_PATH}/

# bitstream: hw_dut
# 	${XIL_VIVADO} artifacts/build_bitstream.tcl
# 	cp kria.prj/kria.prj.runs/impl_1/design_1_wrapper.bit ${BUILD_PATH}/bitstream.bit

# ${BUILD_PATH}/bitstream.bit: bitstream

# ${KRIA_DIR}:
# 	mkdir -p $@

# deploy:
# 	mkdir -p ${KRIA_DIR}
# 	cp ${BUILD_PATH}/bitstream.bit ${KRIA_DIR}/
# 	cp -r ${SRC_PATH}/* ${KRIA_DIR}/
# 	cp -r ${KRIA_SRC}/* ${KRIA_DIR}/
# #	cp src/cordic.cpp src/cordic.h src/cordic_test.cpp kria_dir/
# #	cp ${SRC_PATH}/cordic.cpp ${KRIA_DIR}/cordic.cpp
# #	cp ${SRC_PATH}/cordic.h ${KRIA_DIR}/cordic.h
# #	cp ${SRC_PATH}/cordic_test.cpp ${KRIA_DIR}/cordic_test.cpp


	
# # @echo "Synthesizing CORDIC and creating bitstream on amdpool ..."
# # $(XIL_HLS) run.tcl
# # ./run_bitstream.sh

# clean:
# 	rm -rf ${BUILD_PATH}
# 	rm -rf *.dat *.prj *.log *.jou logs/ .Xil/ ${KRIA_DIR}/ ${RESULTS_DIR}
# #	rm -rf zedboard_project* xillydemo.bit
