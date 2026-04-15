#*********************************************************************************************
# run.tcl 
# @brief: A Tcl script for synthesizing the baseline digit recongnition design.
# @author: Francesco Ciraolo
#*********************************************************************************************

# Get test path from environment variable TEST_PATH, default to "./test/" directory if not set

set test_path "./test/"
if {[info exists ::env(TEST_PATH)]} {
    set test_path $::env(TEST_PATH)
}

set input_path "./input/"
if {[info exists ::env(TEST_PATH)]} {
    set input_path $::env(TEST_PATH)
}

# Get src path from environment variable SRC_PATH, default to "./src/" directory if not set
set src_path "./src/"
if {[info exists ::env(SRC_PATH)]} {
    set src_path $::env(SRC_PATH)
}

# Get project parent path from environment variable PROJ_PATH, default to current directory if not set
set proj_path "."
if {[info exists ::env(PROJ_PATH)]} {
    set proj_path $::env(PROJ_PATH)
}

# Add include path for headers
set include_path "./include/"
if {[info exists ::env(INCLUDE_PATH)]} {
    set include_path $::env(INCLUDE_PATH)
}

# Verify is environment variable SKIP_CSIM is set to "1" or not
set skip_csim "0"
if {[info exists ::env(SKIP_CSIM)]} {
    set skip_csim $::env(SKIP_CSIM)
}

# Verify is environment variable SKIP_COSIM is set to "1" or not
set skip_cosim "0"
if {[info exists ::env(SKIP_COSIM)]} {
    set skip_cosim $::env(SKIP_COSIM)
}

# Combine C++ standard and include path
set cflags "-std=c++17 -I$include_path"

# Open/reset the project
open_project -reset "$proj_path/edge_detector.prj"

# Top function of the design is "dut"
set_top dut

# Add source files with correct include path
add_files -cflags $cflags $src_path/gaussian.cpp
add_files -cflags $cflags $src_path/hysteresis.cpp
# add_files -cflags $cflags $src_path/sobel.cpp
# add_files -cflags $cflags $src_path/threshold.cpp
# add_files -cflags $cflags $src_path/nms.cpp
# add_files -cflags $cflags $src_path/dut.cpp
# === TESTBENCH FILES (C-sim/cosim only, NOT synthesized) ===
add_files -cflags $cflags -tb $src_path/image_io.cpp
add_files -cflags $cflags -tb $src_path/main.cpp
add_files -tb $input_path/bird.pgm
add_files -tb $input_path/mona_lisa.pgm

open_solution "solution1"

# use KV260 device (Kria K26 SOM - Zynq UltraScale+ MPSoC)
set_part xck26-sfvc784-2LV-c

# Target clock period is 10ns
create_clock -period 10 -name default

### You can insert your own directives here ###

############################################

# Simulate the C++ design
if {$skip_csim == "1"} {
    # Skip C simulation if SKIP_CSIM is set to "1"
    puts "Skipping C simulation as SKIP_CSIM is set to 1"
} else {
    # C simulation
    csim_design -O
}

# Synthesize the design
csynth_design

if {$skip_cosim == "1"} {
    # Skip C-RTL cosimulation if SKIP_COSIM is set to "1"
    puts "Skipping C-RTL cosimulation as SKIP_COSIM is set to 1"
} else {
    # C-RTL cosimulation
    cosim_design
}

export_design -format ip_catalog

exit

