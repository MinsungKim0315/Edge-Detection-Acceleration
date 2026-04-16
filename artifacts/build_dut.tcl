#*********************************************************************************************
# build_dut.tcl
# @brief: HLS build script for the FPGA edge-detection pipeline.
#*********************************************************************************************

set input_path "./input/"
if {[info exists ::env(INPUT_PATH)]} {
    set input_path $::env(INPUT_PATH)
}

set src_path "./src/"
if {[info exists ::env(SRC_PATH)]} {
    set src_path $::env(SRC_PATH)
}

set proj_path "."
if {[info exists ::env(PROJ_PATH)]} {
    set proj_path $::env(PROJ_PATH)
}

set include_path "./include/"
if {[info exists ::env(INCLUDE_PATH)]} {
    set include_path $::env(INCLUDE_PATH)
}

set skip_csim "0"
if {[info exists ::env(SKIP_CSIM)]} {
    set skip_csim $::env(SKIP_CSIM)
}

set skip_cosim "0"
if {[info exists ::env(SKIP_COSIM)]} {
    set skip_cosim $::env(SKIP_COSIM)
}

set cflags "-std=c++14 -I$include_path"
set tb_cflags [concat $cflags [format {-DINPUT_DIR=\\"%s\\"} $input_path]]

open_project -reset "$proj_path/edge_detector.prj"
set_top dut

add_files -cflags $cflags $src_path/dut.cpp
add_files -cflags $tb_cflags -tb $src_path/dut_tb.cpp
add_files -cflags $cflags -tb $src_path/image_io.cpp
add_files -cflags $cflags -tb $src_path/gaussian.cpp
add_files -cflags $cflags -tb $src_path/sobel.cpp
add_files -cflags $cflags -tb $src_path/nms.cpp
add_files -cflags $cflags -tb $src_path/threshold.cpp
add_files -tb $input_path/bird.pgm
add_files -tb $input_path/mona_lisa.pgm

open_solution "solution1"
set_part xck26-sfvc784-2LV-c
create_clock -period 10 -name default

if {$skip_csim == "1"} {
    puts "Skipping C simulation as SKIP_CSIM is set to 1"
} else {
    csim_design -O
}

csynth_design

if {$skip_cosim == "1"} {
    puts "Skipping C-RTL cosimulation as SKIP_COSIM is set to 1"
} else {
    cosim_design
}

export_design -format ip_catalog
exit
