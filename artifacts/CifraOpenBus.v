// Generator : SpinalHDL v1.12.3    git head : 591e64062329e5e2e2b81f4d52422948053edb97
// Component : CifraOpenBus

`timescale 1ns/1ps

module CifraOpenBus (
  (* X_INTERFACE_INFO = "XIL_INTERFACENAME io_axiLite, PROTOCOL AXI4LITE, MODE Slave" , X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 io_axiLite AWVALID" *) input  wire          io_axiLite_aw_valid,
  (* X_INTERFACE_INFO = "XIL_INTERFACENAME io_axiLite, PROTOCOL AXI4LITE, MODE Slave" , X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 io_axiLite AWREADY" *) output wire          io_axiLite_aw_ready,
  (* X_INTERFACE_INFO = "XIL_INTERFACENAME io_axiLite, PROTOCOL AXI4LITE, MODE Slave" , X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 io_axiLite AWADDR" *) input  wire [11:0]   io_axiLite_aw_payload_addr,
  (* X_INTERFACE_INFO = "XIL_INTERFACENAME io_axiLite, PROTOCOL AXI4LITE, MODE Slave" , X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 io_axiLite AWPROT" *) input  wire [2:0]    io_axiLite_aw_payload_prot,
  (* X_INTERFACE_INFO = "XIL_INTERFACENAME io_axiLite, PROTOCOL AXI4LITE, MODE Slave" , X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 io_axiLite WVALID" *) input  wire          io_axiLite_w_valid,
  (* X_INTERFACE_INFO = "XIL_INTERFACENAME io_axiLite, PROTOCOL AXI4LITE, MODE Slave" , X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 io_axiLite WREADY" *) output wire          io_axiLite_w_ready,
  (* X_INTERFACE_INFO = "XIL_INTERFACENAME io_axiLite, PROTOCOL AXI4LITE, MODE Slave" , X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 io_axiLite WDATA" *) input  wire [31:0]   io_axiLite_w_payload_data,
  (* X_INTERFACE_INFO = "XIL_INTERFACENAME io_axiLite, PROTOCOL AXI4LITE, MODE Slave" , X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 io_axiLite WSTRB" *) input  wire [3:0]    io_axiLite_w_payload_strb,
  (* X_INTERFACE_INFO = "XIL_INTERFACENAME io_axiLite, PROTOCOL AXI4LITE, MODE Slave" , X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 io_axiLite BVALID" *) output wire          io_axiLite_b_valid,
  (* X_INTERFACE_INFO = "XIL_INTERFACENAME io_axiLite, PROTOCOL AXI4LITE, MODE Slave" , X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 io_axiLite BREADY" *) input  wire          io_axiLite_b_ready,
  (* X_INTERFACE_INFO = "XIL_INTERFACENAME io_axiLite, PROTOCOL AXI4LITE, MODE Slave" , X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 io_axiLite BRESP" *) output wire [1:0]    io_axiLite_b_payload_resp,
  (* X_INTERFACE_INFO = "XIL_INTERFACENAME io_axiLite, PROTOCOL AXI4LITE, MODE Slave" , X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 io_axiLite ARVALID" *) input  wire          io_axiLite_ar_valid,
  (* X_INTERFACE_INFO = "XIL_INTERFACENAME io_axiLite, PROTOCOL AXI4LITE, MODE Slave" , X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 io_axiLite ARREADY" *) output wire          io_axiLite_ar_ready,
  (* X_INTERFACE_INFO = "XIL_INTERFACENAME io_axiLite, PROTOCOL AXI4LITE, MODE Slave" , X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 io_axiLite ARADDR" *) input  wire [11:0]   io_axiLite_ar_payload_addr,
  (* X_INTERFACE_INFO = "XIL_INTERFACENAME io_axiLite, PROTOCOL AXI4LITE, MODE Slave" , X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 io_axiLite ARPROT" *) input  wire [2:0]    io_axiLite_ar_payload_prot,
  (* X_INTERFACE_INFO = "XIL_INTERFACENAME io_axiLite, PROTOCOL AXI4LITE, MODE Slave" , X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 io_axiLite RVALID" *) output wire          io_axiLite_r_valid,
  (* X_INTERFACE_INFO = "XIL_INTERFACENAME io_axiLite, PROTOCOL AXI4LITE, MODE Slave" , X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 io_axiLite RREADY" *) input  wire          io_axiLite_r_ready,
  (* X_INTERFACE_INFO = "XIL_INTERFACENAME io_axiLite, PROTOCOL AXI4LITE, MODE Slave" , X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 io_axiLite RDATA" *) output wire [31:0]   io_axiLite_r_payload_data,
  (* X_INTERFACE_INFO = "XIL_INTERFACENAME io_axiLite, PROTOCOL AXI4LITE, MODE Slave" , X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 io_axiLite RRESP" *) output wire [1:0]    io_axiLite_r_payload_resp,
  (* X_INTERFACE_PARAMETER = "ASSOCIATED_CLKEN resetn, ASSOCIATED_BUSIF io_axiLite" *) input  wire          clk,
  input  wire          resetn
);

  reg                 hlsOutFifo_io_pop_ready;
  wire                hlsInFifo_io_push_ready;
  wire                hlsInFifo_io_pop_valid;
  wire       [31:0]   hlsInFifo_io_pop_payload;
  wire       [12:0]   hlsInFifo_io_occupancy;
  wire       [12:0]   hlsInFifo_io_availability;
  wire                hlsOutFifo_io_push_ready;
  wire                hlsOutFifo_io_pop_valid;
  wire       [31:0]   hlsOutFifo_io_pop_payload;
  wire       [12:0]   hlsOutFifo_io_occupancy;
  wire       [12:0]   hlsOutFifo_io_availability;
  wire                dut_1_io_ap_done;
  wire                dut_1_io_ap_idle;
  wire                dut_1_io_ap_ready;
  wire                dut_1_io_strmIn_ready;
  wire                dut_1_io_strmOut_valid;
  wire       [31:0]   dut_1_io_strmOut_payload;
  wire                slaveFactory_readErrorFlag;
  wire                slaveFactory_writeErrorFlag;
  wire                slaveFactory_readHaltRequest;
  wire                slaveFactory_writeHaltRequest;
  wire                slaveFactory_writeJoinEvent_valid;
  wire                slaveFactory_writeJoinEvent_ready;
  wire                slaveFactory_writeOccur;
  reg        [1:0]    slaveFactory_writeRsp_resp;
  wire                slaveFactory_writeJoinEvent_translated_valid;
  wire                slaveFactory_writeJoinEvent_translated_ready;
  wire       [1:0]    slaveFactory_writeJoinEvent_translated_payload_resp;
  wire                _zz_slaveFactory_writeJoinEvent_translated_ready;
  wire                slaveFactory_writeJoinEvent_translated_haltWhen_valid;
  wire                slaveFactory_writeJoinEvent_translated_haltWhen_ready;
  wire       [1:0]    slaveFactory_writeJoinEvent_translated_haltWhen_payload_resp;
  wire                slaveFactory_writeJoinEvent_translated_haltWhen_halfPipe_valid;
  wire                slaveFactory_writeJoinEvent_translated_haltWhen_halfPipe_ready;
  wire       [1:0]    slaveFactory_writeJoinEvent_translated_haltWhen_halfPipe_payload_resp;
  reg                 slaveFactory_writeJoinEvent_translated_haltWhen_rValid;
  wire                slaveFactory_writeJoinEvent_translated_haltWhen_halfPipe_fire;
  reg        [1:0]    slaveFactory_writeJoinEvent_translated_haltWhen_rData_resp;
  wire                slaveFactory_readDataStage_valid;
  wire                slaveFactory_readDataStage_ready;
  wire       [11:0]   slaveFactory_readDataStage_payload_addr;
  wire       [2:0]    slaveFactory_readDataStage_payload_prot;
  reg                 io_axiLite_ar_rValid;
  wire                slaveFactory_readDataStage_fire;
  reg        [11:0]   io_axiLite_ar_rData_addr;
  reg        [2:0]    io_axiLite_ar_rData_prot;
  reg        [31:0]   slaveFactory_readRsp_data;
  reg        [1:0]    slaveFactory_readRsp_resp;
  wire                _zz_slaveFactory_readDataStage_ready;
  wire                slaveFactory_readDataStage_haltWhen_valid;
  wire                slaveFactory_readDataStage_haltWhen_ready;
  wire       [11:0]   slaveFactory_readDataStage_haltWhen_payload_addr;
  wire       [2:0]    slaveFactory_readDataStage_haltWhen_payload_prot;
  wire                slaveFactory_readDataStage_haltWhen_translated_valid;
  wire                slaveFactory_readDataStage_haltWhen_translated_ready;
  wire       [31:0]   slaveFactory_readDataStage_haltWhen_translated_payload_data;
  wire       [1:0]    slaveFactory_readDataStage_haltWhen_translated_payload_resp;
  wire       [11:0]   slaveFactory_readAddressMasked;
  wire       [11:0]   slaveFactory_writeAddressMasked;
  wire                slaveFactory_readOccur;
  reg                 readFlow_valid;
  wire       [31:0]   readFlow_payload;
  wire                readFlow_toStream_valid;
  wire                readFlow_toStream_ready;
  wire       [31:0]   readFlow_toStream_payload;
  reg                 _zz_io_pop_ready;

  StreamFifoLowLatency hlsInFifo (
    .io_push_valid   (readFlow_toStream_valid        ), //i
    .io_push_ready   (hlsInFifo_io_push_ready        ), //o
    .io_push_payload (readFlow_toStream_payload[31:0]), //i
    .io_pop_valid    (hlsInFifo_io_pop_valid         ), //o
    .io_pop_ready    (dut_1_io_strmIn_ready          ), //i
    .io_pop_payload  (hlsInFifo_io_pop_payload[31:0] ), //o
    .io_flush        (1'b0                           ), //i
    .io_occupancy    (hlsInFifo_io_occupancy[12:0]   ), //o
    .io_availability (hlsInFifo_io_availability[12:0]), //o
    .clk             (clk                            ), //i
    .resetn          (resetn                         )  //i
  );
  StreamFifoLowLatency hlsOutFifo (
    .io_push_valid   (dut_1_io_strmOut_valid          ), //i
    .io_push_ready   (hlsOutFifo_io_push_ready        ), //o
    .io_push_payload (dut_1_io_strmOut_payload[31:0]  ), //i
    .io_pop_valid    (hlsOutFifo_io_pop_valid         ), //o
    .io_pop_ready    (hlsOutFifo_io_pop_ready         ), //i
    .io_pop_payload  (hlsOutFifo_io_pop_payload[31:0] ), //o
    .io_flush        (1'b0                            ), //i
    .io_occupancy    (hlsOutFifo_io_occupancy[12:0]   ), //o
    .io_availability (hlsOutFifo_io_availability[12:0]), //o
    .clk             (clk                             ), //i
    .resetn          (resetn                          )  //i
  );
  DutWrapper dut_1 (
    .io_ap_start        (1'b1                          ), //i
    .io_ap_done         (dut_1_io_ap_done              ), //o
    .io_ap_idle         (dut_1_io_ap_idle              ), //o
    .io_ap_ready        (dut_1_io_ap_ready             ), //o
    .io_strmIn_valid    (hlsInFifo_io_pop_valid        ), //i
    .io_strmIn_ready    (dut_1_io_strmIn_ready         ), //o
    .io_strmIn_payload  (hlsInFifo_io_pop_payload[31:0]), //i
    .io_strmOut_valid   (dut_1_io_strmOut_valid        ), //o
    .io_strmOut_ready   (hlsOutFifo_io_push_ready      ), //i
    .io_strmOut_payload (dut_1_io_strmOut_payload[31:0]), //o
    .clk                (clk                           ), //i
    .resetn             (resetn                        )  //i
  );
  assign slaveFactory_readErrorFlag = 1'b0;
  assign slaveFactory_writeErrorFlag = 1'b0;
  assign slaveFactory_readHaltRequest = 1'b0;
  assign slaveFactory_writeHaltRequest = 1'b0;
  assign slaveFactory_writeOccur = (slaveFactory_writeJoinEvent_valid && slaveFactory_writeJoinEvent_ready);
  assign slaveFactory_writeJoinEvent_valid = (io_axiLite_aw_valid && io_axiLite_w_valid);
  assign io_axiLite_aw_ready = slaveFactory_writeOccur;
  assign io_axiLite_w_ready = slaveFactory_writeOccur;
  assign slaveFactory_writeJoinEvent_translated_valid = slaveFactory_writeJoinEvent_valid;
  assign slaveFactory_writeJoinEvent_ready = slaveFactory_writeJoinEvent_translated_ready;
  assign slaveFactory_writeJoinEvent_translated_payload_resp = slaveFactory_writeRsp_resp;
  assign _zz_slaveFactory_writeJoinEvent_translated_ready = (! slaveFactory_writeHaltRequest);
  assign slaveFactory_writeJoinEvent_translated_haltWhen_valid = (slaveFactory_writeJoinEvent_translated_valid && _zz_slaveFactory_writeJoinEvent_translated_ready);
  assign slaveFactory_writeJoinEvent_translated_ready = (slaveFactory_writeJoinEvent_translated_haltWhen_ready && _zz_slaveFactory_writeJoinEvent_translated_ready);
  assign slaveFactory_writeJoinEvent_translated_haltWhen_payload_resp = slaveFactory_writeJoinEvent_translated_payload_resp;
  assign slaveFactory_writeJoinEvent_translated_haltWhen_halfPipe_fire = (slaveFactory_writeJoinEvent_translated_haltWhen_halfPipe_valid && slaveFactory_writeJoinEvent_translated_haltWhen_halfPipe_ready);
  assign slaveFactory_writeJoinEvent_translated_haltWhen_ready = (! slaveFactory_writeJoinEvent_translated_haltWhen_rValid);
  assign slaveFactory_writeJoinEvent_translated_haltWhen_halfPipe_valid = slaveFactory_writeJoinEvent_translated_haltWhen_rValid;
  assign slaveFactory_writeJoinEvent_translated_haltWhen_halfPipe_payload_resp = slaveFactory_writeJoinEvent_translated_haltWhen_rData_resp;
  assign io_axiLite_b_valid = slaveFactory_writeJoinEvent_translated_haltWhen_halfPipe_valid;
  assign slaveFactory_writeJoinEvent_translated_haltWhen_halfPipe_ready = io_axiLite_b_ready;
  assign io_axiLite_b_payload_resp = slaveFactory_writeJoinEvent_translated_haltWhen_halfPipe_payload_resp;
  assign slaveFactory_readDataStage_fire = (slaveFactory_readDataStage_valid && slaveFactory_readDataStage_ready);
  assign io_axiLite_ar_ready = (! io_axiLite_ar_rValid);
  assign slaveFactory_readDataStage_valid = io_axiLite_ar_rValid;
  assign slaveFactory_readDataStage_payload_addr = io_axiLite_ar_rData_addr;
  assign slaveFactory_readDataStage_payload_prot = io_axiLite_ar_rData_prot;
  assign _zz_slaveFactory_readDataStage_ready = (! slaveFactory_readHaltRequest);
  assign slaveFactory_readDataStage_haltWhen_valid = (slaveFactory_readDataStage_valid && _zz_slaveFactory_readDataStage_ready);
  assign slaveFactory_readDataStage_ready = (slaveFactory_readDataStage_haltWhen_ready && _zz_slaveFactory_readDataStage_ready);
  assign slaveFactory_readDataStage_haltWhen_payload_addr = slaveFactory_readDataStage_payload_addr;
  assign slaveFactory_readDataStage_haltWhen_payload_prot = slaveFactory_readDataStage_payload_prot;
  assign slaveFactory_readDataStage_haltWhen_translated_valid = slaveFactory_readDataStage_haltWhen_valid;
  assign slaveFactory_readDataStage_haltWhen_ready = slaveFactory_readDataStage_haltWhen_translated_ready;
  assign slaveFactory_readDataStage_haltWhen_translated_payload_data = slaveFactory_readRsp_data;
  assign slaveFactory_readDataStage_haltWhen_translated_payload_resp = slaveFactory_readRsp_resp;
  assign io_axiLite_r_valid = slaveFactory_readDataStage_haltWhen_translated_valid;
  assign slaveFactory_readDataStage_haltWhen_translated_ready = io_axiLite_r_ready;
  assign io_axiLite_r_payload_data = slaveFactory_readDataStage_haltWhen_translated_payload_data;
  assign io_axiLite_r_payload_resp = slaveFactory_readDataStage_haltWhen_translated_payload_resp;
  always @(*) begin
    if(slaveFactory_writeErrorFlag) begin
      slaveFactory_writeRsp_resp = 2'b10;
    end else begin
      slaveFactory_writeRsp_resp = 2'b00;
    end
  end

  always @(*) begin
    if(slaveFactory_readErrorFlag) begin
      slaveFactory_readRsp_resp = 2'b10;
    end else begin
      slaveFactory_readRsp_resp = 2'b00;
    end
  end

  always @(*) begin
    slaveFactory_readRsp_data = 32'h0;
    case(slaveFactory_readAddressMasked)
      12'h0 : begin
        slaveFactory_readRsp_data[0 : 0] = hlsInFifo_io_push_ready;
      end
      12'h008 : begin
        slaveFactory_readRsp_data[0 : 0] = hlsOutFifo_io_pop_valid;
      end
      12'h00c : begin
        slaveFactory_readRsp_data[31 : 0] = hlsOutFifo_io_pop_payload;
      end
      default : begin
      end
    endcase
  end

  assign slaveFactory_readAddressMasked = (slaveFactory_readDataStage_payload_addr & (~ 12'h003));
  assign slaveFactory_writeAddressMasked = (io_axiLite_aw_payload_addr & (~ 12'h003));
  assign slaveFactory_readOccur = (io_axiLite_r_valid && io_axiLite_r_ready);
  always @(*) begin
    readFlow_valid = 1'b0;
    case(slaveFactory_writeAddressMasked)
      12'h004 : begin
        if(slaveFactory_writeOccur) begin
          readFlow_valid = 1'b1;
        end
      end
      default : begin
      end
    endcase
  end

  assign readFlow_toStream_valid = readFlow_valid;
  assign readFlow_toStream_payload = readFlow_payload;
  assign readFlow_toStream_ready = hlsInFifo_io_push_ready;
  always @(*) begin
    _zz_io_pop_ready = 1'b0;
    case(slaveFactory_readAddressMasked)
      12'h00c : begin
        if(slaveFactory_readOccur) begin
          _zz_io_pop_ready = 1'b1;
        end
      end
      default : begin
      end
    endcase
  end

  always @(*) begin
    hlsOutFifo_io_pop_ready = _zz_io_pop_ready;
    case(slaveFactory_readAddressMasked)
      12'h00c : begin
        if(slaveFactory_readOccur) begin
          hlsOutFifo_io_pop_ready = 1'b1;
        end
      end
      default : begin
      end
    endcase
  end

  assign readFlow_payload = io_axiLite_w_payload_data[31 : 0];
  always @(posedge clk) begin
    if(!resetn) begin
      slaveFactory_writeJoinEvent_translated_haltWhen_rValid <= 1'b0;
      io_axiLite_ar_rValid <= 1'b0;
    end else begin
      if(slaveFactory_writeJoinEvent_translated_haltWhen_valid) begin
        slaveFactory_writeJoinEvent_translated_haltWhen_rValid <= 1'b1;
      end
      if(slaveFactory_writeJoinEvent_translated_haltWhen_halfPipe_fire) begin
        slaveFactory_writeJoinEvent_translated_haltWhen_rValid <= 1'b0;
      end
      if(io_axiLite_ar_valid) begin
        io_axiLite_ar_rValid <= 1'b1;
      end
      if(slaveFactory_readDataStage_fire) begin
        io_axiLite_ar_rValid <= 1'b0;
      end
    end
  end

  always @(posedge clk) begin
    if(slaveFactory_writeJoinEvent_translated_haltWhen_ready) begin
      slaveFactory_writeJoinEvent_translated_haltWhen_rData_resp <= slaveFactory_writeJoinEvent_translated_haltWhen_payload_resp;
    end
    if(io_axiLite_ar_ready) begin
      io_axiLite_ar_rData_addr <= io_axiLite_ar_payload_addr;
      io_axiLite_ar_rData_prot <= io_axiLite_ar_payload_prot;
    end
  end


endmodule

module DutWrapper (
  input  wire          io_ap_start,
  output wire          io_ap_done,
  output wire          io_ap_idle,
  output wire          io_ap_ready,
  input  wire          io_strmIn_valid,
  output wire          io_strmIn_ready,
  input  wire [31:0]   io_strmIn_payload,
  output wire          io_strmOut_valid,
  input  wire          io_strmOut_ready,
  output wire [31:0]   io_strmOut_payload,
  input  wire          clk,
  input  wire          resetn
);

  wire                hlsArea_dut_ap_done;
  wire                hlsArea_dut_ap_idle;
  wire                hlsArea_dut_ap_ready;
  wire                hlsArea_dut_strmIn_TREADY;
  wire                hlsArea_dut_strmOut_TVALID;
  wire       [31:0]   hlsArea_dut_strmOut_TDATA;

  dut hlsArea_dut (
    .ap_clk        (clk                           ), //i
    .ap_rst_n      (resetn                        ), //i
    .ap_start      (io_ap_start                   ), //i
    .ap_done       (hlsArea_dut_ap_done           ), //o
    .ap_idle       (hlsArea_dut_ap_idle           ), //o
    .ap_ready      (hlsArea_dut_ap_ready          ), //o
    .strmIn_TDATA  (io_strmIn_payload[31:0]       ), //i
    .strmIn_TVALID (io_strmIn_valid               ), //i
    .strmIn_TREADY (hlsArea_dut_strmIn_TREADY     ), //o
    .strmOut_TDATA (hlsArea_dut_strmOut_TDATA[31:0]), //o
    .strmOut_TVALID(hlsArea_dut_strmOut_TVALID    ), //o
    .strmOut_TREADY(io_strmOut_ready              )  //i
  );
  assign io_ap_done = hlsArea_dut_ap_done;
  assign io_ap_idle = hlsArea_dut_ap_idle;
  assign io_ap_ready = hlsArea_dut_ap_ready;
  assign io_strmIn_ready = hlsArea_dut_strmIn_TREADY;
  assign io_strmOut_valid = hlsArea_dut_strmOut_TVALID;
  assign io_strmOut_payload = hlsArea_dut_strmOut_TDATA;

endmodule

//StreamFifoLowLatency_1 replaced by StreamFifoLowLatency

module StreamFifoLowLatency (
  input  wire          io_push_valid,
  output wire          io_push_ready,
  input  wire [31:0]   io_push_payload,
  output wire          io_pop_valid,
  input  wire          io_pop_ready,
  output wire [31:0]   io_pop_payload,
  input  wire          io_flush,
  output wire [12:0]   io_occupancy,
  output wire [12:0]   io_availability,
  input  wire          clk,
  input  wire          resetn
);

  wire                fifo_io_push_ready;
  wire                fifo_io_pop_valid;
  wire       [31:0]   fifo_io_pop_payload;
  wire       [12:0]   fifo_io_occupancy;
  wire       [12:0]   fifo_io_availability;

  StreamFifo fifo (
    .io_push_valid   (io_push_valid             ), //i
    .io_push_ready   (fifo_io_push_ready        ), //o
    .io_push_payload (io_push_payload[31:0]     ), //i
    .io_pop_valid    (fifo_io_pop_valid         ), //o
    .io_pop_ready    (io_pop_ready              ), //i
    .io_pop_payload  (fifo_io_pop_payload[31:0] ), //o
    .io_flush        (io_flush                  ), //i
    .io_occupancy    (fifo_io_occupancy[12:0]   ), //o
    .io_availability (fifo_io_availability[12:0]), //o
    .clk             (clk                       ), //i
    .resetn          (resetn                    )  //i
  );
  assign io_push_ready = fifo_io_push_ready;
  assign io_pop_valid = fifo_io_pop_valid;
  assign io_pop_payload = fifo_io_pop_payload;
  assign io_occupancy = fifo_io_occupancy;
  assign io_availability = fifo_io_availability;

endmodule

//StreamFifo_1 replaced by StreamFifo

module StreamFifo (
  input  wire          io_push_valid,
  output wire          io_push_ready,
  input  wire [31:0]   io_push_payload,
  output reg           io_pop_valid,
  input  wire          io_pop_ready,
  output reg  [31:0]   io_pop_payload,
  input  wire          io_flush,
  output wire [12:0]   io_occupancy,
  output wire [12:0]   io_availability,
  input  wire          clk,
  input  wire          resetn
);

  wire       [31:0]   logic_ram_spinal_port1;
  reg                 _zz_1;
  reg                 logic_ptr_doPush;
  wire                logic_ptr_doPop;
  wire                logic_ptr_full;
  wire                logic_ptr_empty;
  reg        [12:0]   logic_ptr_push;
  reg        [12:0]   logic_ptr_pop;
  wire       [12:0]   logic_ptr_occupancy;
  wire       [12:0]   logic_ptr_popOnIo;
  wire                when_Stream_l1455;
  reg                 logic_ptr_wentUp;
  wire                io_push_fire;
  wire                logic_push_onRam_write_valid;
  wire       [11:0]   logic_push_onRam_write_payload_address;
  wire       [31:0]   logic_push_onRam_write_payload_data;
  wire                logic_pop_addressGen_valid;
  wire                logic_pop_addressGen_ready;
  wire       [11:0]   logic_pop_addressGen_payload;
  wire                logic_pop_addressGen_fire;
  wire       [31:0]   logic_pop_async_readed;
  wire                logic_pop_addressGen_translated_valid;
  wire                logic_pop_addressGen_translated_ready;
  wire       [31:0]   logic_pop_addressGen_translated_payload;
  (* ram_style = "distributed" *) reg [31:0] logic_ram [0:4095];

  always @(posedge clk) begin
    if(_zz_1) begin
      logic_ram[logic_push_onRam_write_payload_address] <= logic_push_onRam_write_payload_data;
    end
  end

  assign logic_ram_spinal_port1 = logic_ram[logic_pop_addressGen_payload];
  always @(*) begin
    _zz_1 = 1'b0;
    if(logic_push_onRam_write_valid) begin
      _zz_1 = 1'b1;
    end
  end

  assign when_Stream_l1455 = (logic_ptr_doPush != logic_ptr_doPop);
  assign logic_ptr_full = (((logic_ptr_push ^ logic_ptr_popOnIo) ^ 13'h1000) == 13'h0);
  assign logic_ptr_empty = (logic_ptr_push == logic_ptr_pop);
  assign logic_ptr_occupancy = (logic_ptr_push - logic_ptr_popOnIo);
  assign io_push_ready = (! logic_ptr_full);
  assign io_push_fire = (io_push_valid && io_push_ready);
  always @(*) begin
    logic_ptr_doPush = io_push_fire;
    if(logic_ptr_empty) begin
      if(io_pop_ready) begin
        logic_ptr_doPush = 1'b0;
      end
    end
  end

  assign logic_push_onRam_write_valid = io_push_fire;
  assign logic_push_onRam_write_payload_address = logic_ptr_push[11:0];
  assign logic_push_onRam_write_payload_data = io_push_payload;
  assign logic_pop_addressGen_valid = (! logic_ptr_empty);
  assign logic_pop_addressGen_payload = logic_ptr_pop[11:0];
  assign logic_pop_addressGen_fire = (logic_pop_addressGen_valid && logic_pop_addressGen_ready);
  assign logic_ptr_doPop = logic_pop_addressGen_fire;
  assign logic_pop_async_readed = logic_ram_spinal_port1;
  assign logic_pop_addressGen_translated_valid = logic_pop_addressGen_valid;
  assign logic_pop_addressGen_ready = logic_pop_addressGen_translated_ready;
  assign logic_pop_addressGen_translated_payload = logic_pop_async_readed;
  always @(*) begin
    io_pop_valid = logic_pop_addressGen_translated_valid;
    if(logic_ptr_empty) begin
      io_pop_valid = io_push_valid;
    end
  end

  assign logic_pop_addressGen_translated_ready = io_pop_ready;
  always @(*) begin
    io_pop_payload = logic_pop_addressGen_translated_payload;
    if(logic_ptr_empty) begin
      io_pop_payload = io_push_payload;
    end
  end

  assign logic_ptr_popOnIo = logic_ptr_pop;
  assign io_occupancy = logic_ptr_occupancy;
  assign io_availability = (13'h1000 - logic_ptr_occupancy);
  always @(posedge clk) begin
    if(!resetn) begin
      logic_ptr_push <= 13'h0;
      logic_ptr_pop <= 13'h0;
      logic_ptr_wentUp <= 1'b0;
    end else begin
      if(when_Stream_l1455) begin
        logic_ptr_wentUp <= logic_ptr_doPush;
      end
      if(io_flush) begin
        logic_ptr_wentUp <= 1'b0;
      end
      if(logic_ptr_doPush) begin
        logic_ptr_push <= (logic_ptr_push + 13'h0001);
      end
      if(logic_ptr_doPop) begin
        logic_ptr_pop <= (logic_ptr_pop + 13'h0001);
      end
      if(io_flush) begin
        logic_ptr_push <= 13'h0;
        logic_ptr_pop <= 13'h0;
      end
    end
  end


endmodule
