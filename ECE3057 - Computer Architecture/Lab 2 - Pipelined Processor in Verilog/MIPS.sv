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
//  Module:     MIPS
//  Functionality:
//      This is the top level function for a pipelined MIPS processor
//  Inputs:
//      clock: These need to be generated in your testbed file. 
//      reset: Active High reset. These need to be generated in your testbed file. 
//
//  Outputs:
//      PC, the current PC location
//      ALU_result_out, The output of the ALU
//      read_data_1_out, The register that feeds the ALU
//      read_data_2_out, The registers that feed the ALU
//      write_data_out, The output data from the system
//      Instruction_out, The instruction that is being executed
//      Branch_out, Flag for branch
//      Zero_out,  Flag for zero output of the ALU
//      Memwrite_out, Flag for memory write operation
//      Regwrite_out, Flag for register write in the decode stage.
//
//  Version History:
//      2020.04.09      Brothers, T. Code converted from VHDL to SV
//      2020.04.27      Brothers, T. Converted the single cycle to multi-cycle
//***********************************************************

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Module Declaration
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
module MIPS (
    //clock and reset signals
    input logic clock,
    input logic reset,
    
    
    // Output important signals to pins for easy display in Simulator
    output logic [9:0] PC,
    output logic [31:0] ALU_result_out, 
    output logic [31:0] read_data_1_out, 
    output logic [31:0] read_data_2_out, 
    output logic [31:0] write_data_out, 
    output logic [31:0] Instruction_out,
    output logic Branch_out,
    output logic Zero_out, 
    output logic MemWrite_out, 
    output logic RegWrite_out,
    output logic stall_out
    );

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Declare signals and outputs
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //Local Variables
    logic [9:0]  PC_plus_4     ;
    logic [9:0]  PC_plus_4_ID  ;
    logic [5:0]  function_opcode;
    logic [31:0] read_data_1   ;
    logic [31:0] read_data_2   ;
    logic [31:0] sign_extend   ;
    logic [7:0]  Add_result    ;
    logic [31:0] ALU_result    ;
    logic [31:0] ALU_result_MEM;
    logic [31:0] read_data     ;
    logic ALUSrc               ;
    logic Branch               ;
    logic branch_EX            ;
    logic RegDst               ;
    logic RegWrite             ;
    logic Zero                 ;
    logic MemWrite             ;
    logic MemWrite_EX          ;
    logic MemtoReg             ;
    logic MemtoReg_EX          ;
    logic MemtoReg_MEM         ;
    
    logic MemRead              ;
    logic MemRead_EX           ;
    logic [1:0]  ALUop         ;
    logic [31:0] Instruction   ;
    logic stall                ;
    logic R_format             ;
    logic I_format             ;
    logic Lw                   ;
    logic Sw                   ;
    logic Beq                  ;
    logic [4:0] dest_reg_R_type;
    logic [4:0] dest_reg_I_type;
    
    logic sig_RegWrite_EX ;
    logic sig_RegWrite_MEM;
    logic sig_RegWrite_WB ;
    
    logic [4:0] dest_EX  ;
    logic [4:0] dest_MEM ;
    logic [4:0] dest_WB  ;
    
    logic [31:0] memory_write_data;
    logic [31:0] write_data_WB    ;

	logic [4:0] dec_rt;
	logic [4:0] dec_rs;
	logic [1:0] op_FA;
	logic [1:0] op_FB;
    //ADDED
     logic [4:0] dest;
     logic [5:0]  opcode_ex;
    assign dest = RegDst? dest_reg_R_type : dest_reg_I_type;
    // copy important signals to output pins for easy 
    // display in Simulator
    assign Instruction_out  = Instruction;
    assign ALU_result_out   = ALU_result;
    assign read_data_1_out  = read_data_1;
    assign read_data_2_out  = read_data_2;
    assign write_data_out   = write_data_WB;
    assign Branch_out       = Branch;
    assign Zero_out         = Zero;
    assign RegWrite_out     = RegWrite;
    assign MemWrite_out     = MemWrite;  
    assign stall_out        = stall;
    
    
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Create the Controller and Stall Controller
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
STALL_CONT my_STALL_CONT(
    //Inputs
    .ip_instruction(Instruction),
    .lw_opcode(opcode_ex),
    
    //Control inputs
    .ip_R_format(R_format),
    .ip_I_format(I_format), 
    .ip_Lw      (Lw      ),
    .ip_Sw      (Sw      ),
    .ip_Beq     (Beq     ),
    
     //RegWrite flags from the different stages
    .ip_RegWrite_EX (RegWrite ),
    .ip_RegWrite_MEM(sig_RegWrite_EX),
    .ip_RegWrite_WB (sig_RegWrite_MEM),
    
    //The destination register from the different stages
    .ip_dest_EX  (dest  ),
    .ip_dest_MEM (dest_EX ),
    .ip_dest_WB  (dest_MEM  ),
    //Outputs
    .op_stall(stall)
    );
    
CONTROL my_CONTROL(
    //Inputs
    //  --from fetch--
    .ip_instruction (Instruction),

    //Outputs
    //  --MUX Controls--
    .op_RegDst      (RegDst),
    .op_MemtoReg    (MemtoReg),
    
    //  --Register Control--
    .op_RegWrite    (RegWrite),
    
    //  --Memory Controls--
    .op_read_en     (MemRead),
    .op_write_en    (MemWrite),
    
    //  --Fetch Controls--
    .op_branch      (Branch),

    //  --Execute Controls--
    .op_ALU_src     (ALUSrc),
    .op_ALU_op      (ALUop),
    
    //  --Flags to drive the stall controller
    //local signals
    .op_R_format(R_format),
    .op_I_format(I_format), 
    .op_Lw      (Lw      ),
    .op_Sw      (Sw      ),
    .op_Beq     (Beq     ),
    
    //clock and reset signals
    .clock  (clock),
    .reset  (reset)
    );   
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Connect the 5 MIPS components 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
IFECTCH my_IFETCH( 
    //Inputs
    //  --control inputs--
    .ip_branch  (branch_EX), 
    
    //  --from Hazard Detection--
    .ip_stall   (stall),
    
    //  --from execute--
    .ip_add_result  (Add_result ),
    .ip_zero        (Zero       ),
    
    //Outputs
    .op_instruction (Instruction),
    .op_PC          (PC         ),
    .op_PC_plus_4   (PC_plus_4  ),

    //clock and reset signals
    .clock  (clock),
    .reset  (reset)
);


IDECODE my_IDECODE(
    //Outputs
    .op_function_opcode (function_opcode),
    .op_PC_plus_4       (PC_plus_4_ID),
    .op_read_data_1  (read_data_1),
    .op_read_data_2  (read_data_2),
    .op_immediate    (sign_extend),
    .op_dest_reg_R_type(dest_reg_R_type),
    .op_dest_reg_I_type(dest_reg_I_type),
    .op_dec_rs (dec_rs),
	.op_dec_rt (dec_rt),
	.op_opcode (opcode_ex),
    // ADDED**********************************************************
    .ip_branch  (branch_EX),
    .ip_zero (Zero),
    //Inputs
     //pass through signals
    .ip_PC_plus_4(PC_plus_4)       ,
    
    //  --from fetch--
    .ip_instruction  (Instruction),
    
    //  --from Hazard Detection--
    .ip_stall   (stall),
    //  --from memory--
    .ip_write_reg_addr(dest_WB), 
    .ip_write_data    (write_data_WB),
    .ip_RegWrite      (sig_RegWrite_WB),

    //clock and reset signals
    .clock  (clock),
    .reset  (reset)
);

FWD_CONT FWD_CONT(

	.ip_EX_MEM_RegWrite(sig_RegWrite_EX),
	.ip_MEM_WB_RegWrite(sig_RegWrite_MEM),
	.ip_EX_MEM_dest(dest_EX),
	.ip_MEM_WB_dest(dest_MEM),
	.ip_DEC_DEST_RS(dec_rs),
	.ip_DEC_DEST_RT(dec_rt),
	.op_FA(op_FA),
	.op_FB(op_FB)

);


EXECUTE my_EXECUTE(
    //Inputs
    //  --from decode-- 
    .ip_function_opcode  (function_opcode),
    .ip_PC_plus_4        (PC_plus_4_ID),
    .ip_read_data_1      (read_data_1),
    .ip_read_data_2      (read_data_2),
    .ip_immediate        (sign_extend),
    .ip_dest_reg_R_type  (dest_reg_R_type), 
    .ip_dest_reg_I_type  (dest_reg_I_type),
    
    //  --from control--  
    //    ==These signals are for the EXE stage==
    .ip_ALU_op   (ALUop),
    .ip_ALU_src  (ALUSrc),
    .ip_RegDst   (RegDst),
    
    //    ==These signals are pass through==    
    .ip_MemtoReg    (MemtoReg),
    .ip_RegWrite    (RegWrite),
    .ip_read_en     (MemRead),
    .ip_write_en    (MemWrite),
    .ip_branch      (Branch),

    
    //Outputs
    //  --Output Control Signal--        
    .op_zero        (Zero),
    .op_MemtoReg    (MemtoReg_EX),
    .op_RegWrite    (sig_RegWrite_EX),
    .op_read_en     (MemRead_EX),
    .op_write_en    (MemWrite_EX),
    .op_branch      (branch_EX),
    
    //  --Output Data Signals--        
    .op_ALU_result   (ALU_result),
    .op_Add_result   (Add_result),
    .op_memory_write_data(memory_write_data),
    .op_dest_reg         (dest_EX),

    //clock and reset signals
    .clock  (clock),
    .reset  (reset),
	//forwarding Signals
	.ALU_result_MEM(ALU_result_MEM),
	.read_data_wb(read_data),
	.MemtoReg_MEM(MemtoReg_MEM),
	.FA(op_FA),
	.FB(op_FB)
	//
);


DMEMORY my_DMEMORY(
    //Inputs
    //  --Input Control Signal--        
    .ip_MemtoReg    (MemtoReg_EX), //pass through signal
    .ip_RegWrite    (sig_RegWrite_EX), //pass through signal
	.ip_read_en     (MemRead_EX), //read enable
    .ip_write_en    (MemWrite_EX),//write enable
    
    //  --Data and Address from Execute--  
    .ip_data        (memory_write_data),
    .ip_ALU_output  (ALU_result),
    .ip_dest_reg    (dest_EX), //pass through signal
    
    //Outputs
    //  --Output Control Signal--        
    .op_MemtoReg     (MemtoReg_MEM), //pass through signal
    .op_RegWrite     (sig_RegWrite_MEM), //pass through signal
    
    //  --Output Data Signal--        
    .op_data        (read_data),
    .op_ALU_output  (ALU_result_MEM),
    .op_dest_reg    (dest_MEM), //pass through signal
    
    //clock and reset signals
    .clock  (clock),
    .reset  (reset)
);

WRITE_BACK my_WRITE_BACK(
    //Inputs
    //  --Input Control Signal--        
    .ip_MemtoReg     (MemtoReg_MEM), //Mux control
    .ip_RegWrite     (sig_RegWrite_MEM), //Register write enable
    
    //  --Input Data Signal--        
    .ip_memory_data(read_data), //data from the memory
    .ip_ALU_result (ALU_result_MEM), //data from the ALU
    .ip_dest_reg   (dest_MEM), //destination register address
    
    //Output
    //  --Output Control Signal--        
    .op_RegWrite         (sig_RegWrite_WB), //write-back Register write enable
    
    //  --Output Data Signal--        
    .op_write_data(write_data_WB), //write-back data
    .op_dest_reg  (dest_WB), //write-back register address
    
    //clock and reset signals
    .clock  (clock),
    .reset  (reset)
);
    
endmodule

