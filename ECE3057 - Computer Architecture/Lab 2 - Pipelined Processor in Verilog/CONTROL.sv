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
//  Module:     CONTROL
//  Functionality:
//      implements the data memory for the MIPS computer
//  Inputs:
//      sig_opcode: The opcode from the fetch stage
//    
//  Outputs:
//      //  --MUX Controls--
//      op_RegDst  : Select between I type and R type messages for register writes
//      op_MemtoReg: Write-back mux select to switch between Exe output or Mem output.
//      
//      //  --Register Control--
//      op_RegWrite: Register write flag in the Decode stage
//      
//      //  --Memory Controls--
//      op_read_en : MemRead flag for the memory stage
//      op_write_en: MemWrite flag for the memory stage
//      
//      //  --Fetch Controls--
//      op_branch  : This is ANDed with the zero output from EXE to switch the PC between PC+4 and the EXE Stage output.
//  
//      //  --Execute Controls--
//      op_ALU_src : Sets the ALU source
//      op_ALU_op  : Selects the ALU operation
//    
//  Version History:
//      2020.04.10      Brothers, T. Code converted from VHDL to SV
//      2020.04.27      Brothers, T. Converted to pipeline from single stage
//***********************************************************

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Module Declaration
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
module CONTROL(
    //Inputs
    //  --from fetch--
    input logic [31:0] ip_instruction,
    
    //Outputs
    //  --MUX Controls--
    output logic op_RegDst      ,
    output logic op_MemtoReg    ,
    
    //  --Register Control--
    output logic op_RegWrite    ,
    
    //  --Memory Controls--
    output logic op_read_en     ,
    output logic op_write_en    ,
    
    //  --Fetch Controls--
    output logic op_branch      ,

    //  --Execute Controls--
    output logic op_ALU_src     ,
    output logic [1:0] op_ALU_op,
    
    //  --Flags to drive the stall controller
        //local signals
    output logic  op_R_format,
    output logic  op_I_format, 
    output logic  op_Lw, 
    output logic  op_Sw, 
    output logic  op_Beq,
    
    //clock and reset signals
    input logic clock,
    input logic reset
    );

    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Set the Control Signals Based on Op Code
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //local signals
    logic  R_format, I_format, Lw, Sw, Beq, NOP;
    
    logic sig_RegDst      ;
    logic sig_MemtoReg    ;
    logic sig_RegWrite    ;
    logic sig_read_en     ;
    logic sig_write_en    ;
    logic sig_branch      ;
    logic sig_ALU_src     ;
    logic [1:0] sig_ALU_op;
    logic [5:0] sig_opcode;
    
    // pull out the opcode
    assign sig_opcode = ip_instruction[31:26];

    //check to see if it is a NOP
    assign NOP      =  ~|(ip_instruction);  //NOR of the instruction
          
    // Use the opcode to generate some local signals
    assign I_format =  (sig_opcode[5:3] == 3'b001) ? 1 : 0; //check the opcode to see if it is an I type signal
    assign R_format =  (sig_opcode == 6'b000000) ? 1 : 0;
    assign Lw       =  (sig_opcode == 6'b100011) ? 1 : 0;
    assign Sw       =  (sig_opcode == 6'b101011) ? 1 : 0;
    assign Beq      =  (sig_opcode == 6'b000100) ? 1 : 0;
    
    
    //assign the outputs
    //  --MUX Controls--
    assign sig_RegDst   =  R_format & (~NOP);
    assign sig_MemtoReg =  Lw;
    
    //  --Register Control--
    assign sig_RegWrite =  (R_format | Lw) & (~NOP);
    
    //  --Memory Controls--
    assign sig_read_en  =  Lw;
    assign sig_write_en =  Sw; 
    
    //  --Fetch Controls--
    assign sig_branch   =  Beq;
    
    //  --Execute Controls--
    assign sig_ALU_src   =  Lw | Sw;
    assign sig_ALU_op[1] =  R_format;
    assign sig_ALU_op[0] =  Beq; 
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Output Flags for Stall Controller
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    
    assign op_R_format = R_format;
    assign op_I_format = I_format;
    assign op_Lw       = Lw      ;
    assign op_Sw       = Sw      ;
    assign op_Beq      = Beq     ;
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Pipeline Register
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    //Register Signals
    logic reg_RegDst      ;
    logic reg_MemtoReg    ;
    logic reg_RegWrite    ;
    logic reg_read_en     ;
    logic reg_write_en    ;
    logic reg_branch      ;
    logic reg_ALU_src     ;
    logic [1:0] reg_ALU_op;
    
    //Register block
    always @ (posedge clock) begin
        reg_RegDst   <= sig_RegDst   ;    
        reg_MemtoReg <= sig_MemtoReg ;
        reg_RegWrite <= sig_RegWrite ;
        reg_read_en  <= sig_read_en  ;
        reg_write_en <= sig_write_en ;
        reg_branch   <= sig_branch   ;
        reg_ALU_src  <= sig_ALU_src  ;
        reg_ALU_op   <= sig_ALU_op   ;
    end
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Assign outputs
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    assign op_RegDst   = reg_RegDst   ;
    assign op_MemtoReg = reg_MemtoReg ;   
    assign op_RegWrite = reg_RegWrite ;   
    assign op_read_en  = reg_read_en  ;   
    assign op_write_en = reg_write_en ;   
    assign op_branch   = reg_branch   ;   
    assign op_ALU_src  = reg_ALU_src  ;   
    assign op_ALU_op   = reg_ALU_op   ;
        
endmodule
    
    
