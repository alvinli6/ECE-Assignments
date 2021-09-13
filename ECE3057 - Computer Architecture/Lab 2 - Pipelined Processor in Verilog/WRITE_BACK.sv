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
//  Module:     WRITE_BACK
//  Functionality:
//      This is the write-back stage of a pipelined MIPS processor
//
//  Inputs:
//      --Input Control Signal--        
//      ip_MemtoReg: This controls mux
//      ip_RegWrite: Signal to indicate a register write
//
//      --Input Data Signal--        
//      ip_data     : Data from the memory feeding the write-back stage
//      ip_address  : Data from the ALU feeding the write-back stage
//      ip_dest_reg : The destination register address for write-back stage
//
//  Outputs:
//      --Output Control Signal--        
//      op_RegWrite: Signal to indicate a register write
//
//      --Input Data Signal--        
//      op_write_back_data  : write-back stage data
//      op_write_back_addr  : write-back stage register address
//    
//  Version History:
//      2020.04.10      Brothers, T. Code converted from VHDL to SV
//      2020.04.27      Brothers, T. Converted to pipeline from single stage
//***********************************************************

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Module Declaration
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
module WRITE_BACK(
    //Inputs
    //  --Input Control Signal--        
    input logic ip_MemtoReg     , //Mux control
    input logic ip_RegWrite     , //Register write enable
    
    //  --Input Data Signal--        
    input logic [31:0] ip_memory_data, //data from the memory
    input logic [31:0] ip_ALU_result , //data from the ALU
    input logic [4:0]  ip_dest_reg   , //destination register address
    
    //Output
    //  --Output Control Signal--        
    output logic op_RegWrite         , //write-back Register write enable
    
    //  --Output Data Signal--        
    output logic [31:0] op_write_data, //write-back data
    output logic [4:0]  op_dest_reg  , //write-back register address
    
    //clock and reset signals
    input logic clock,
    input logic reset
    );

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Write-Back Mux
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Mux to bypass data memory for R format instructions
    assign op_write_data = ip_MemtoReg ? ip_memory_data : ip_ALU_result;
        
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Assign the Outputs
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    
    assign op_RegWrite  = ip_RegWrite;
    assign op_dest_reg  = ip_dest_reg;

endmodule