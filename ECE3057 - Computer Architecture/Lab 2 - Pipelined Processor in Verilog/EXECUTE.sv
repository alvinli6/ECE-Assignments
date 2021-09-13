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
//  Module:     EXECUTE
//  Functionality:
//      Implements the data ALU and Branch Address Adder
//  Inputs:
//    --from decode-- 
//    ip_function_opcode: the opcode for the input function
//    ip_PC_plus_4: PC plus 4. This is the next PC location without branch/jump
//    ip_read_data_1: The data from decode stage
//    ip_read_data_2: The data from decode stage
//    ip_immediate: The data from decode stage
//    ip_dest_reg_R_type: The destination address for the register write for R type messages
//    ip_dest_reg_I_type: The destination address for the register write for I type messages
//    
//    --from control--        
//    ip_ALU_op: The ALU operation
//    ip_ALU_src: The source of the ALU data
//    ip_RegDst: Mux select of the destination address for the register write
//
//      ==These are signals that will be passed to the MEM and WB stages
//    ip_MemtoReg    
//    ip_RegWrite    
//    ip_read_en     
//    ip_write_en    
//    ip_branch      
//
//  Outputs:
//    op_ALU_result: The ALU result
//    op_Add_result: The result from the adder for the PC.
//    op_memory_write_data: The register data for a memory write
//    op_dest_reg: The destination register for the write-back stage         
//
//    --Output Control Signal--        
//    op_zero: If the ALU result is zero. This flag is used to determine the next PC value
//
//      ==These are signals were registered inside this block but not modified
//    op_MemtoReg    
//    op_RegWrite    
//    op_read_en     
//    op_write_en    
//    op_branch      
//
//  Version History:
//      2020.04.09      Brothers, T. Code converted from VHDL to SV
//***********************************************************

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Module Declaration
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
module EXECUTE (
    //Inputs
    //  --from decode--
	input logic [5:0] ip_opcode,
    input logic [5:0]  ip_function_opcode ,
    input logic [9:0]  ip_PC_plus_4       ,
    input logic [31:0] ip_read_data_1     ,
    input logic [31:0] ip_read_data_2     ,
    input logic [31:0] ip_immediate       ,
    input logic [4:0]  ip_dest_reg_R_type , 
    input logic [4:0]  ip_dest_reg_I_type ,
    
    //  --from control--  
    //    ==These signals are for the EXE stage==
    input logic [1:0] ip_ALU_op,
    input logic ip_ALU_src,
    input logic ip_RegDst,
    
    //    ==These signals are pass through==    
    input logic ip_MemtoReg    ,
    input logic ip_RegWrite    ,
    input logic ip_read_en     ,
    input logic ip_write_en    ,
    input logic ip_branch      ,

    
    //Outputs
    //  --Output Control Signal--        
    output logic op_zero,
    output logic op_MemtoReg    ,
    output logic op_RegWrite    ,
    output logic op_read_en     ,
    output logic op_write_en    ,
    output logic op_branch      ,
    
    //  --Output Data Signals--        
    output logic signed [31:0] op_ALU_result,
    output logic [7:0]  op_Add_result       ,
    output logic [31:0] op_memory_write_data,
    output logic [4:0]  op_dest_reg         ,
    
    //clock and reset signals
    input logic clock,
    input logic reset,

	//forwarding Signals
	input logic [31:0] ALU_result_MEM,
	input logic [31:0] read_data_wb,
	input logic MemtoReg_MEM,
	input logic [1:0] FA,
	input logic [1:0] FB
);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Form the Data for the ALU
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //variables
    logic signed [31:0] A_input, B_input;
    
    //form the input dat0
	////Add forwarding******
	
   // assign A_input = ip_read_data_1;
   // assign B_input = ip_ALU_src ? ip_immediate : ip_read_data_2;
    always @ (*)
// 00, 10, and 2 for 01
    if (FA == 2'b00)
        A_input <= ip_read_data_1;
    else if(FA == 2'b10)
        A_input <= op_ALU_result;
    else if (FA == 2'b01 && MemtoReg_MEM)
        A_input <= read_data_wb;
    else
        A_input <= ALU_result_MEM;
    
    always @ (*)
    if(ip_ALU_src)
        B_input <= ip_immediate;
    else if(FB == 2'b00)
        B_input <= ip_read_data_2;
    else if(FB == 2'b10)
        B_input <= op_ALU_result;
    else if(FB == 2'b01 && MemtoReg_MEM)
        B_input <= read_data_wb;
    else
        B_input <= ALU_result_MEM;
    // read immediate or register value for non-forwarding case



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//ALU Control
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    logic [2:0] ALU_ctl;
    
    assign ALU_ctl[0] = (ip_function_opcode[0] | ip_function_opcode[3]) & ip_ALU_op[1];
    assign ALU_ctl[1] = (~ip_function_opcode[2]) | (~ip_ALU_op[1]);
    assign ALU_ctl[2] = (ip_function_opcode[1] & ip_ALU_op[1]) | ip_ALU_op[0];
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Do the ALU operations
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    logic signed [31:0] sig_ALU_result;
    
    always @ (*) begin
        // Select ALU operation
        case (ALU_ctl)
            // ALU performs ALUresult = A_input AND B_input
            3'b000  : sig_ALU_result  = A_input & B_input; 
            
            // ALU performs ALUresult = A_input OR B_input
            3'b001  : sig_ALU_result  = A_input | B_input; 
            
            // ALU performs ALUresult = A_input + B_input
            3'b010  : sig_ALU_result  = A_input + B_input;
            
            // ALU performs ALUresult = A_input - B_input
            3'b110  : sig_ALU_result  = A_input - B_input;
            
            // ALU performs Set on Less Than. Does a subtraction, then check for negative
            3'b111  : sig_ALU_result  = ((A_input - B_input) < 0) ? 1 : 0;
            default : sig_ALU_result  = 0;
        endcase
    end


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Generate the Zero flag
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    logic sig_zero;
    assign sig_zero = ~|(sig_ALU_result); //Reduction NOR to check if all the bits are zero.

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Mux for the Destination Register
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Mux for Register Write Address
    logic [4:0] sig_dest_reg;
    assign sig_dest_reg = ip_RegDst ? ip_dest_reg_R_type : ip_dest_reg_I_type;
    

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Generate the Branch with the PC
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //The PC is in bytes. Each instruction is 32 bits. So we drop the 
    //two lower bits. So the branch add is counting number of instructions
    logic [7:0]  sig_Add_result;
    assign sig_Add_result  = ip_PC_plus_4 +  $signed(ip_immediate[7:0]<<2) ;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Pipeline Register
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //Register Signals
    logic signed [31:0] reg_ALU_result;
    logic [7:0]  reg_Add_result       ;
    logic [31:0] reg_memory_write_data;
    logic [4:0]  reg_dest_reg         ;
    
    //Control Registers
    logic reg_zero     ;
    logic reg_MemtoReg ;
    logic reg_RegWrite ;
    logic reg_read_en  ;
    logic reg_write_en ;
    logic reg_branch   ;
    
    //Register block
    always @ (posedge clock) begin    
        if (reset) begin 
            reg_ALU_result        <= 0;
            reg_Add_result        <= 0;
            reg_memory_write_data <= 0;    //this is a pass through
            reg_dest_reg          <= 0;
            
            //Control Registers
            reg_zero     <= 0 ;
            reg_MemtoReg <= 0 ;
            reg_RegWrite <= 0 ;
            reg_read_en  <= 0 ;
            reg_write_en <= 0 ;
            reg_branch   <= 0 ;
            end
        
        // code below implements my flush
        else if(op_branch && reg_zero) begin
            reg_ALU_result        <= 0;
           // reg_Add_result        <= 0;
            reg_memory_write_data <= 0;   
            reg_dest_reg          <= 0;

            //Control Registers
            reg_zero     <= 0 ;
            reg_MemtoReg <= 0 ;
            reg_RegWrite <= 0 ;
            reg_read_en  <= 0 ;
            reg_write_en <= 0 ;
            reg_branch   <= 0 ;

        end
        
			
        else begin
            reg_ALU_result        <= sig_ALU_result;
            reg_Add_result        <= sig_Add_result;
            reg_memory_write_data <= ip_read_data_2;    //this is a pass through
            reg_dest_reg          <= sig_dest_reg;
            
            //Control Registers
            reg_zero     <= sig_zero    ;
            reg_MemtoReg <= ip_MemtoReg ;
            reg_RegWrite <= ip_RegWrite ;
            reg_read_en  <= ip_read_en  ;
            reg_write_en <= ip_write_en ;
            reg_branch   <= ip_branch   ;
            end
    end
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Assign the Outputs
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    
    //data outputs
    assign op_ALU_result         = reg_ALU_result        ;
    assign op_Add_result         = reg_Add_result        ;
    assign op_memory_write_data  = reg_memory_write_data ;
    assign op_dest_reg           = reg_dest_reg          ;
    
    //control outputs
    assign op_zero     = reg_zero     ;
    assign op_MemtoReg = reg_MemtoReg ;
    assign op_RegWrite = reg_RegWrite ;
    assign op_read_en  = reg_read_en  ;
    assign op_write_en = reg_write_en ;
    assign op_branch   = reg_branch   ;

endmodule

//NEW
//assign A_input = (FA == 2'b00)? ip_read_data_1 : (FA == 2'b10) ? op_ALU_result : (FA == 2'b01 && MemtoReg_MEM) ? read_data_wb : ALU_result_MEM;
//  assign B_input = ip_ALU_src ? ip_immediate : (FB == 2'b00)? ip_read_data_2 : (FB == 2'b10) ? op_ALU_result : (FB == 2'b01 && MemtoReg_MEM) ? read_data_wb : ALU_result_MEM;

