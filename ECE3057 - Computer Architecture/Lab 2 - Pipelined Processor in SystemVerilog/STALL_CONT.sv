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
//  Module:     STALL_CONT
//  Functionality:
//      implements the Stall Controller for the MIPS pipelined processor
//
//  Inputs:
//      ip_opcode: The opcode from the fetch stage
//    
//  Outputs:

//    
//  Version History:
//      2020.04.27      Brothers, T. Document Created
//***********************************************************

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Module Declaration
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
module STALL_CONT(
    //Inputs
    input logic [31:0] ip_instruction,
    
    //Control inputs
    input logic ip_R_format,
    input logic ip_I_format,
    input logic ip_Lw      ,
    input logic ip_Sw      ,
    input logic ip_Beq     ,
    input logic [5:0] lw_opcode,
    
    //RegWrite flags from the different stages
    input logic ip_RegWrite_EX ,
    input logic ip_RegWrite_MEM,
    input logic ip_RegWrite_WB ,
    
    //The destination register from the different stages
    input logic [4:0] ip_dest_EX  ,
    input logic [4:0] ip_dest_MEM ,
    input logic [4:0] ip_dest_WB  ,
  
    //Outputs
    output logic op_stall
    );

    //  MOST OF CHANGES********************
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Pull out information from the instruction
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //local signals
    logic [5:0] sig_opcode;
    logic [4:0] sig_RS;
    logic [4:0] sig_RT; 
    assign sig_opcode = ip_instruction[31:26];
    assign sig_RS = ip_instruction[25:21];
    assign sig_RT = ip_instruction[20:16];
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Determine if RS or RT are used
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    
    //R type uses RS and RT as input.
    //I type uses RS as an input.
    //BEQ uses RS and RT as input.
    //LW uses RS as input
    //SW uses RS and RT as input
    
    logic use_RS;
    logic use_RT;
    assign use_RS = ip_R_format | ip_I_format | ip_Lw | ip_Sw | ip_Beq;
    assign use_RT = ip_R_format | ip_Sw | ip_Beq;
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Check if there is a hazard on RS
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //(rs(IRID)==destEX)  && use_rs(IRID) && RegWriteEX 	or
    //(rs(IRID)==destMEM) && use_rs(IRID) && RegWriteMEM 	or
    //(rs(IRID)==destWB)  && use_rs(IRID) && RegWriteWB
    
    logic RS_EX_hazard  ;
    logic RS_MEM_hazard ;
    logic RS_WB_hazard  ;
    // this is the code I added for the RS  (for load to use stall)
    assign RS_EX_hazard  = (sig_RS == ip_dest_EX) && use_RS && ip_RegWrite_EX;
    assign RS_MEM_hazard = (sig_RS == ip_dest_MEM) && use_RS && ip_RegWrite_MEM;
    assign RS_WB_hazard  = (sig_RS == ip_dest_WB) && use_RS && ip_RegWrite_WB;
    
    //Check to see if any of the stages have RS hazards
    logic RS_hazard;
    assign RS_hazard = RS_EX_hazard | RS_MEM_hazard | RS_WB_hazard;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Check if there is a hazard on RT
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //(rt(IRID)==destEX)  && use_rt(IRID) && RegWriteEX 	or
    //(rt(IRID)==destMEM) && use_rt(IRID) && RegWriteMEM 	or
    //(rt(IRID)==destWB)  && use_rt(IRID) && RegWriteWB
    logic RT_EX_hazard  ;
    logic RT_MEM_hazard ;
    logic RT_WB_hazard  ;
    // this is the code I added for the RT for load to use stall
    assign RT_EX_hazard  = (sig_RT == ip_dest_EX) && use_RT && ip_RegWrite_EX;
    assign RT_MEM_hazard = (sig_RT == ip_dest_MEM) && use_RT && ip_RegWrite_MEM;
    assign RT_WB_hazard  = (sig_RT == ip_dest_WB) && use_RT && ip_RegWrite_WB;
    
    //Check to see if any of the stages have RS hazards
    logic RT_hazard;
    assign RT_hazard = RT_EX_hazard | RT_MEM_hazard | RT_WB_hazard;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Check Combine RS and RT hazards
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    assign load = (lw_opcode == 6'b100011) ? 1 : 0;
    assign op_stall = (RS_hazard | RT_hazard) && load ; //&& (//if its a load word);

endmodule
