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
//  Module:     DMEMORY
//  Functionality:
//      implements the data memory for the MIPS computer
//  Inputs:    
//      --Input Control Signal--        
//      ip_MemtoReg: This signal is passed through the Pipeline Register 
//      ip_RegWrite: This signal is passed through the Pipeline Register 
//      ip_read_en : read the data from memory
//      ip_write_en: write the data to the memory
//
//      --Data and Address from Execute--  
//      ip_data    : Data for write
//      ip_ALU_output : The output from the ALU
//      ip_dest_reg: This signal is passed through the Pipeline Register 
//
//  Outputs:
//      --Output Control Signal--        
//      op_MemtoReg: This controls the write-back stage mux
//      op_RegWrite: Signal to indicate a register write on the write-back stage
//
//      --Output Data Signal--        
//      op_data      : Data from the memory feeding the write-back stage
//      op_ALU_output: Data from the ALU feeding the write-back stage
//      op_dest_reg  : The destination register address for write-back stage
//    
//  Version History:
//      2020.04.10      Brothers, T. Code converted from VHDL to SV
//      2020.04.27      Brothers, T. Converted to pipeline from single stage
//***********************************************************

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Module Declaration
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
module DMEMORY(
    //Inputs
    //  --Input Control Signal--        
    input logic ip_MemtoReg     , //pass through signal
    input logic ip_RegWrite     , //pass through signal
    input logic ip_read_en      , //read enable
    input logic ip_write_en     , //write enable
    
    //  --Data and Address from Execute--  
    input logic [31:0] ip_data,
    input logic [31:0] ip_ALU_output,
    input logic [4:0]  ip_dest_reg, //pass through signal
    
    //Outputs
    //  --Output Control Signal--        
    output logic op_MemtoReg     , //pass through signal
    output logic op_RegWrite     , //pass through signal
    
    //  --Output Data Signal--        
    output logic [31:0] op_data,
    output logic [31:0] op_ALU_output,
    output logic [4:0]  op_dest_reg, //pass through signal
    
    //clock and reset signals
    input logic clock,
    input logic reset
    );

//**********************************************************
//Local Params Parameters
//**********************************************************
localparam PARAM_MEM_length = 256;
localparam PARAM_RAM_addr_bits = $clog2(PARAM_MEM_length);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Instruction Memory
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //local variables for the instruction RAM
    logic [7:0] data_RAM [0:PARAM_MEM_length-1]; //RAM that is a byte wide and 256 long 
    logic [PARAM_RAM_addr_bits-1 :0] data_RAM_addr;

    //Load the initial values into the data RAM
    initial begin 
        for (int i = 0; i < PARAM_MEM_length; i++) 
            data_RAM[i] = 0; //initialize the RAM with all zeros
        
        //Load memory with a couple example pieces of data.
        //Each data location is a byte. The input data into the system
        //is 32 bits (4 bytes) so each word of data is written across 
        //4 memory locations
        data_RAM[0] = 8'h55;
        data_RAM[1] = 8'h55;
        data_RAM[2] = 8'h55;
        data_RAM[3] = 8'h55;
        data_RAM[4] = 8'hAA;
        data_RAM[5] = 8'hAA;
        data_RAM[6] = 8'hAA;
        data_RAM[7] = 8'hAA;
    end 

    //assign the address for the RAM
    assign data_RAM_addr = ip_ALU_output[PARAM_RAM_addr_bits-1 :0];
    
    
    //register the data into the RAM.
    //Each data location is a byte. The input data into the system
    //is 32 bits (4 bytes) so each word of data is written across 
    //4 memory locations
    always @ (posedge clock)
        if(ip_write_en) begin
            data_RAM[data_RAM_addr]   <= ip_data[7:0]  ; 
            data_RAM[data_RAM_addr+1] <= ip_data[15:8] ; 
            data_RAM[data_RAM_addr+2] <= ip_data[23:16]; 
            data_RAM[data_RAM_addr+3] <= ip_data[31:24]; 
            end
    
    //read controls
    //if we are reading from the RAM put the address data on the output line.
    logic [31:0] sig_data;
    assign sig_data[7:0]   = ip_read_en ? data_RAM[data_RAM_addr]   : 0;
    assign sig_data[15:8]  = ip_read_en ? data_RAM[data_RAM_addr+1] : 0;
    assign sig_data[23:16] = ip_read_en ? data_RAM[data_RAM_addr+2] : 0;
    assign sig_data[31:24] = ip_read_en ? data_RAM[data_RAM_addr+3] : 0;
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Pipeline Register
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //  --Output Control Signal--        
    logic reg_MemtoReg     ; //pass through signal
    logic reg_RegWrite     ; //pass through signal
    
    //  --Output Data Signal--        
    logic [31:0] reg_data    ;
    logic [31:0] reg_ALU_output ; 
    logic [4:0]  reg_dest_reg; //pass through signal
    
    //Register block
    always @ (posedge clock) begin        
        reg_data        <= sig_data;
        reg_ALU_output  <= ip_ALU_output;
        reg_dest_reg    <= ip_dest_reg;
        
        //Control Registers
        reg_MemtoReg <= ip_MemtoReg ;
        reg_RegWrite <= ip_RegWrite ;
    end
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Assign the Outputs
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    
    //data outputs
    assign op_data     = reg_data    ;
    assign op_ALU_output  = reg_ALU_output ;
    assign op_dest_reg = reg_dest_reg;
    
    //Control Registers
    assign op_MemtoReg = reg_MemtoReg;
    assign op_RegWrite = reg_RegWrite;

endmodule