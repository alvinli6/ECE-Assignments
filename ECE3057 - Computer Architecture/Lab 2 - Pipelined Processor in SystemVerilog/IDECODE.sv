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
//  Module:     IDECODE
//  Functionality:
//      Implements the register file
//  Inputs:    
//      ip_PC_plus_4   : Pass through data that we are just registering    
//      ip_instruction: the instruction from the fetch stage
//      ip_stall : Flag from the Hazard Detection Unit
//      ip_write_reg_addr : the register location for the write-back data.
//      ip_write_data: data to write from the write-back stage
//      ip_RegWrite: write to the read_data in the instruction to the register space
//
//  Outputs:
//      op_function_opcode : Pass through data that we are just registering    
//      op_PC_plus_4   : Pass through data that we are just registering    
//      op_read_data_1 : read data from register
//      op_read_data_2 : read data from register
//      op_immediate   : data from immediate instruction
//      op_dest_reg_R_type    : the destination register for the write-back stage for R type messages.
//      op_dest_reg_I_type    : the destination register for the write-back stage for I type messages.
//
//  Version History:
//      2020.04.09      Brothers, T. Code converted from VHDL to SV
//      2020.04.27      Brothers, T. Converted the code to pipelined
//***********************************************************

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Module Declaration
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
module IDECODE(
    //Outputs
    //Inputs
    //  --from decode-- 
    output logic [5:0]  op_function_opcode , //IP = INPUT  OP = OUTPUT 
    output logic [9:0]  op_PC_plus_4       ,
    output logic [31:0] op_read_data_1  ,
    output logic [31:0] op_read_data_2  ,
    output logic [31:0] op_immediate    ,
    output logic [4:0]  op_dest_reg_R_type,
    output logic [4:0]  op_dest_reg_I_type,
	output logic [4:0]  op_dec_rs,
	output logic [4:0]  op_dec_rt,
	output logic [5:0]  op_opcode,
    //Inputs
    //pass through signals
    input logic [9:0]  ip_PC_plus_4       ,
    
    //  --from fetch--
    input logic [31:0] ip_instruction   ,
    
    //  --from Hazard Detection--
    input logic ip_stall                ,
    
    //  --from memory--
    input logic [4:0]  ip_write_reg_addr, 
    input logic [31:0] ip_write_data    ,
    input logic ip_RegWrite     ,
    //--for flushing-- NEED SPECIFIC SIGNALS 
	input ip_zero,
	input ip_branch,
    //clock and reset signals
    input logic clock,
    input logic reset
);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Decode the instruction
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //variables
    logic [4:0] read_register_1_address;
    logic [4:0] read_register_2_address;
    logic [4:0] sig_dest_reg_R_type;
    logic [4:0] sig_dest_reg_I_type;
    logic signed [31:0] Instruction_immediate_value;
    logic [5:0] sig_function_opcode;
    
    //pull the data from the instruction and place into local variables
    assign read_register_1_address      = ip_instruction[25:21];
    assign read_register_2_address      = ip_instruction[20:16];
    
    //This is the dest register for R type. 
    assign sig_dest_reg_R_type     = ip_stall ? 0 : ip_instruction[15:11]; //mux the NOP command on stall.
    
    //This is the dest register for I type. 
    assign sig_dest_reg_I_type     = ip_instruction[20:16];
    
    //Sign Extend the instruction for I type.
    assign Instruction_immediate_value  = $signed(ip_instruction[15:0]); 
    
    //pull off the function op code.
    assign sig_function_opcode = ip_instruction[5:0];
    

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Register Array
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //local variables for the register array RAM
    logic [31:0] register_array [0:31]; //generate the register array 
    
    //Initial register values on reset are register = reg number
    always @(posedge clock) begin 
        if (reset)
            for (int i = 0; i < 32; i++) //use a loop to init the array
                register_array[i] <= i; //initialize the RAM with all zeros
       
        // Write back to register - don't write to register 0
        else if(ip_RegWrite && (ip_write_reg_addr != 0))
              register_array[ip_write_reg_addr] <= ip_write_data;
    end 

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Pipeline Register
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //Register Signals
    logic [5:0]  reg_function_opcode ;
    logic [9:0]  reg_PC_plus_4       ;
    logic [31:0] reg_read_data_1  ;
    logic [31:0] reg_read_data_2  ;
    logic [31:0] reg_immediate    ;
    logic [4:0]  reg_dest_reg_R_type;
    logic [4:0]  reg_dest_reg_I_type;
    logic [4:0]  reg_dec_rs;
    logic [4:0]  reg_dec_rt;  
	logic [5:0]  reg_opcode;
    //Register block
    logic [31:0] reg_instruction;
    always @ (posedge clock) begin
	   if(reset || (ip_zero && ip_branch)) begin  // checking to see if branch edited 
        reg_function_opcode <= 0;        
		reg_read_data_1  <= 0;
        reg_read_data_2  <= 0;
        reg_immediate    <= 0;
        reg_dest_reg_R_type <= 0; 
        reg_dest_reg_I_type <= 0;
        reg_PC_plus_4 <= 0;
		reg_dec_rt <= 0;
		reg_dec_rs <= 0;
		reg_opcode <= 0;
        reg_instruction <= 0;
		end
	   else begin
        reg_function_opcode <= sig_function_opcode;
        reg_PC_plus_4    <= ip_PC_plus_4;
        //reg_read_data_1  <= register_array[read_register_1_address];
        //reg_read_data_2  <= register_array[read_register_2_address];

        if (ip_RegWrite && (ip_write_reg_addr != 0 && ip_write_reg_addr == read_register_1_address)) 
            begin
            reg_read_data_1 <= ip_write_data; 
            end
        else 
            reg_read_data_1 <= register_array[read_register_1_address];
            
        if  (ip_RegWrite && (ip_write_reg_addr != 0 && ip_write_reg_addr == read_register_2_address)) 
            begin
            reg_read_data_2 <= ip_write_data; 
            end 
        else
            reg_read_data_2 <= register_array[read_register_2_address];
            
        reg_immediate <= Instruction_immediate_value;
        reg_dest_reg_R_type <= sig_dest_reg_R_type;
        reg_dest_reg_I_type <= sig_dest_reg_I_type;
		reg_dec_rs <= read_register_1_address;
		reg_dec_rt <= read_register_2_address;
		reg_opcode <= ip_instruction[31:26];
        reg_instruction <= ip_instruction;
		end
    end
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Assign the Outputs
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    
    assign op_function_opcode = reg_function_opcode;
    assign op_PC_plus_4     = reg_PC_plus_4    ;
    assign op_read_data_1   = reg_read_data_1  ;
    assign op_read_data_2   = reg_read_data_2  ;
    assign op_immediate     = reg_immediate    ;
    assign op_dest_reg_R_type = reg_dest_reg_R_type;
    assign op_dest_reg_I_type = reg_dest_reg_I_type;
    assign op_dec_rt 		= reg_dec_rt;
	assign op_dec_rs		= reg_dec_rs;
	assign op_opcode        = reg_opcode;
    assign op_instruction   = reg_instruction;
endmodule
