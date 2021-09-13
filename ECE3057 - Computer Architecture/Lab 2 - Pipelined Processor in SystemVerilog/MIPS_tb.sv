//***********************************************************
// ECE 3058 Architecture Concurrency and Energy in Computation
//
// MIPS Processor System Verilog Behavioral Model
//
// School of Electrical & Computer Engineering
// Georgia Institute of Technology
// Atlanta, GA 30332
//
//  Engineer:   Brothers, Tim
//  Module:     MIPS_tb
//  Functionality:
//      This is the testbed for the Pipelined MIPS processor
//
//  Version History:
//      2020.04.09      Brothers, T. File Created
//***********************************************************

`timescale 1ns / 1ps

module MIPS_pipelined_tb;


//**********************************************************
// Generate the clocks and Reset
//**********************************************************
//generate the clock
logic aclk = 1;
always 
    #1 aclk <= aclk +1;

//generate the reset
logic reset;
integer i;
initial begin
    // do simulation
    $dumpfile("sim.vcd");
    // dump main signals into VCD
    $dumpvars(0, MIPS_pipelined_tb);
    
    // memories
    for(i = 0; i < 32; i = i + 1)
      $dumpvars(0, MIPS_pipelined_tb.my_MIPS_processor.my_IDECODE.register_array[i]);
    // only dumping 32 of the 64 entries in the
    // iram
    for(i = 0; i < 32; i = i + 1)
      $dumpvars(0, MIPS_pipelined_tb.my_MIPS_processor.my_IFETCH.instr_RAM[i]);

    // only dumping 32 of the 64 entries in the
    // iram
    for(i = 0; i < 32; i = i + 1)
      $dumpvars(0, MIPS_pipelined_tb.my_MIPS_processor.my_DMEMORY.data_RAM[i]);
    reset = 1'b1;
    #5 reset = 1'b0;

    #50 $finish;
    end


//**********************************************************
// Local Signals for Data Dispaly
//**********************************************************
logic [9:0]  PC              ;
logic [31:0] ALU_result_out  ;
logic [31:0] read_data_1_out ;
logic [31:0] read_data_2_out ;
logic [31:0] write_data_out  ;
logic [31:0] Instruction_out ;
logic Branch_out             ;
logic Zero_out               ;
logic MemWrite_out           ;
logic RegWrite_out           ;
logic stall_out              ;
logic [6:0] cycle_cnt       ;

initial
  cycle_cnt = 0;
always @(posedge aclk)
  if(~reset)
    cycle_cnt <= cycle_cnt + 1; 
    
    
    
//**********************************************************
// MPIS instantiation
//**********************************************************
MIPS my_MIPS_processor
(
    .reset  (reset      ),  
    .clock  (aclk       ),
    
    .PC               (PC               ),
    .ALU_result_out   (ALU_result_out   ),
    .read_data_1_out  (read_data_1_out  ),
    .read_data_2_out  (read_data_2_out  ),
    .write_data_out   (write_data_out   ),
    .Instruction_out  (Instruction_out  ),
    .Branch_out       (Branch_out       ),
    .Zero_out         (Zero_out         ),
    .MemWrite_out     (MemWrite_out     ),
    .RegWrite_out     (RegWrite_out     ),
    .stall_out        (stall_out        )
);

    
endmodule
