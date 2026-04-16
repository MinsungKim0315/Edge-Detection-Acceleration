#*********************************************************************************************
# File: build_bitstream.tcl
# @brief: A Tcl script for building bitstream for the HLS generated DUT.
# @author: Francesco Ciraolo
#*********************************************************************************************

# List all staged RTL/include files and generated HLS subcore IPs
set hw_dut_v_files [glob -nocomplain build/hw_dut/*.v]
set hw_dut_vh_files [glob -nocomplain build/hw_dut/*.vh]
set hw_dut_files [concat $hw_dut_v_files $hw_dut_vh_files]
set hls_subcore_xci_files [glob -nocomplain edge_detector.prj/solution1/impl/ip/hdl/ip/*/*.xci]

# Check if the RTL list is empty
if {[llength $hw_dut_files] == 0} {
    puts "Error: No files found in build/hw_dut/ directory. Please run build_dut.tcl first to generate the HLS output."
    exit 1
}

# Check if the artifacts/CifraOpenBus.v file exists
if {![file exists "artifacts/CifraOpenBus.v"]} {
    puts "Error: artifacts/CifraOpenBus.v file not found. Please make sure it exists before running this script."
    exit 1
}

set _xil_proj_name_ "kria.prj"

# Create a new Vivado project
create_project -force ${_xil_proj_name_} ./${_xil_proj_name_} -part xck26-sfvc784-2LV-c
# xck26-sfvc784-2LV-c

set proj_dir [get_property directory [current_project]]

# Set project properties
set obj [current_project]
set_property -name "board_part" -value "xilinx.com:kv260_som:part0:1.4" -objects $obj
set_property -name "default_lib" -value "xil_defaultlib" -objects $obj
set_property -name "enable_resource_estimation" -value "0" -objects $obj
set_property -name "enable_vhdl_2008" -value "1" -objects $obj
set_property -name "ip_cache_permissions" -value "read write" -objects $obj
set_property -name "ip_output_repo" -value "$proj_dir/${_xil_proj_name_}.cache/ip" -objects $obj
set_property -name "mem.enable_memory_map_generation" -value "1" -objects $obj
set_property -name "platform.board_id" -value "kv260_som" -objects $obj
set_property -name "revised_directory_structure" -value "1" -objects $obj
set_property -name "sim.central_dir" -value "$proj_dir/${_xil_proj_name_}.ip_user_files" -objects $obj
set_property -name "sim.ip.auto_export_scripts" -value "1" -objects $obj
set_property -name "simulator_language" -value "Mixed" -objects $obj
set_property -name "sim_compile_state" -value "1" -objects $obj
set_property -name "use_inline_hdl_ip" -value "1" -objects $obj

# # Create 'sources_1' fileset
# create_fileset -srcset sources_1

# Set 'sources_1' fileset object
set obj [get_filesets sources_1]
add_files -norecurse -fileset $obj $hw_dut_files
if {[llength $hls_subcore_xci_files] > 0} {
    add_files -norecurse -fileset $obj $hls_subcore_xci_files
}
add_files -norecurse -fileset $obj artifacts/CifraOpenBus.v
puts "Added files to sources_1 fileset: $hw_dut_files $hls_subcore_xci_files and artifacts/CifraOpenBus.v"

# Create block design
create_bd_design "design_1"

set bd_design [get_bd_designs design_1]
set bd_file [get_property FILE_NAME $bd_design]

startgroup
create_bd_cell -type ip -vlnv xilinx.com:ip:zynq_ultra_ps_e:3.5  zynq_ultra_ps_e_0
apply_bd_automation -rule xilinx.com:bd_rule:zynq_ultra_ps_e -config {apply_board_preset "1" }  [get_bd_cells zynq_ultra_ps_e_0]
set_property CONFIG.PSU__FPGA_PL1_ENABLE {0} [get_bd_cells zynq_ultra_ps_e_0]
set_property -dict [list \
  CONFIG.PSU__USE__IRQ0 {0} \
  CONFIG.PSU__USE__M_AXI_GP0 {1} \
  CONFIG.PSU__USE__M_AXI_GP1 {0} \
  CONFIG.PSU__USE__M_AXI_GP2 {0} \
] [get_bd_cells zynq_ultra_ps_e_0]
create_bd_cell -type module -reference CifraOpenBus CifraOpenBus_0
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {Auto} Clk_slave {Auto} Clk_xbar {Auto} Master {/zynq_ultra_ps_e_0/M_AXI_HPM0_FPD} Slave {/CifraOpenBus_0/io_axiLite} ddr_seg {Auto} intc_ip {New AXI SmartConnect} master_apm {0}}  [get_bd_intf_pins CifraOpenBus_0/io_axiLite]
set_property offset 0x400000000 [get_bd_addr_segs {zynq_ultra_ps_e_0/Data/SEG_CifraOpenBus_0_reg0}]
endgroup

set wrapper_file [make_wrapper -files [get_files $bd_file] -top]
add_files -norecurse $wrapper_file

# Set 'sources_1' fileset properties
set obj [get_filesets sources_1]
set_property -name "dataflow_viewer_settings" -value "min_width=16" -objects $obj
set_property top design_1_wrapper [get_filesets sources_1]


# # Create 'sim_1' fileset
# create_fileset -simset sim_1

# Set 'sim_1' fileset properties
set obj [get_filesets sim_1]
set_property -name "sim_wrapper_top" -value "1" -objects $obj
set_property top design_1_wrapper [get_filesets sim_1]
set_property top_lib xil_defaultlib [get_filesets sim_1]

set cores [exec nproc]
puts "Running implementation with $cores parallel jobs..."
launch_runs impl_1 -to_step write_bitstream -jobs $cores  -quiet
wait_on_run impl_1
if {[get_property PROGRESS [get_runs impl_1]] != "100%"} {
   error "ERROR: synth_1 failed"
}