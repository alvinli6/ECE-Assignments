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
//  Module:     IFECTCH
//  Functionality:
//      This module provides the PC and instruction memory
//  Inputs:
//      ip_branch: flag from the controller to indicate a branch operation
//      ip_stall : Flag from the Hazard Detection Unit
//      ip_zero: flag from the execute stage to indicate a zero result
//      ip_add_result: 8 bit result from the execute stage adder
//
//  Outputs:
//      op_instruction: the instruction code
//      op_PC:  the current PC location
//      op_PC_plus_4: the next PC location (PC + 4)
//
//  Version History:
//      2020.04.09      Brothers, T. Code converted from VHDL to SV
//      2020.04.27      Brothers, T. Converted to pipeline from single stage
//***********************************************************

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Module Declaration
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
module IFECTCH( 
    //Inputs
    //  --from control--
    input logic ip_branch, 
    
    //  --from Hazard Detection--
    input logic ip_stall,
    
    //  --from execute--
    input logic  [7:0]  ip_add_result,
    input logic ip_zero, // **** says if your branch is taken or not taken 
    
    //Outputs
    output logic [31:0] op_instruction,
    output logic [9:0]  op_PC,
    output logic [9:0]  op_PC_plus_4,

    
    //clock and reset signals
    input logic clock,
    input logic reset
);

//**********************************************************
//Local Params Parameters
//**********************************************************
localparam PARAM_RAM_length = 64;
localparam PARAM_RAM_addr_bits = $clog2(PARAM_RAM_length);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Instruction Memory
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //local variables for the instruction RAM
    logic [31:0] instr_RAM [0:PARAM_RAM_length-1]; //generate a RAM that is 64 entries long and each entry is 4 bytes
    logic [PARAM_RAM_addr_bits-1 :0] instr_mem_addr;

    //Load the program into the memory
    initial begin 
     for (int i = 0; i < PARAM_RAM_length; i++)
        
        // this code belows proves that my flush works
        instr_RAM[i] = 0; //initialize the RAM with all zeros  
        instr_RAM[0] = 32'h00000000;   // nop fill pipeline
        instr_RAM[1] = 32'h00000000;   // nop fill pipeline
        instr_RAM[2] = 32'h10210004;   // beq $1, $1, PC + 12  // +20
        instr_RAM[3] = 32'h00000000;   // nop fill pipeline
        instr_RAM[4] = 32'h8C020000;   // lw $2,0 ;memory(00)=55555555
        instr_RAM[5] = 32'h8C030004;   // lw $3,4 ;memory(04)=AAAAAAAA
        instr_RAM[6] = 32'h00430820;   // add $1,$2,$3
        instr_RAM[7] = 32'h00000000;   // sw $1,4 ;memory(08)=FFFFFFFF
        instr_RAM[8] = 32'h1022FFFF;   // beq $1,$2,-4  
        instr_RAM[9] = 32'h1021FFFA;   // beq $1,$1,-24 (Assume delay slot present, so it  
                                       // New PC = PC+4-24 = PC-20
            

        /*

        // more test cases that work
        instr_RAM[0] = 32'h8C020000;    // lw $2,0 ;memory(00)=55555555
        instr_RAM[1] = 32'h8C030004;    // lw $3,4 ;memory(04)=AAAAAAAA
        instr_RAM[2] = 32'h00430820;    //add $1, $2, $3
        instr_RAM[3] = 32'h00210820;    //add $1, $1, $1
        instr_RAM[4] = 32'h10000001;    //beq $0, $0, 1
        instr_RAM[5] = 32'h00210820;    //add $1, $1, $1
        instr_RAM[6] = 32'h8C040000;    //lw $4, 0($0) ;memory(00)=55555555
        instr_RAM[7] = 32'h00852820;    //add $5, $4, $5

        // test cases 
        instr_RAM[0] = 32'h00000000;     //   nop fill pipeline
        instr_RAM[1] = 32'h00000000;     //   nop fill pipeline
       instr_RAM[2] = 32'h00000000;     //   nop fill pipeline
        instr_RAM[0]  = 32'h0C000005; // BEG: jal 0x5 // jump to iram index 5 and store return address in $31
        instr_RAM[1]  = 32'h00804020; // add $8 $4 $0
        instr_RAM[2]  = 32'h08000009; // j 0x9 // jump to instruction indexed in 9
        instr_RAM[3]  = 32'h00804020; // add $8 $4 $0
        instr_RAM[4]  = 32'h00804020; // add $8 $4 $0
        instr_RAM[5]  = 32'h08000008; // j 0x8 // jump to iram index 8
        instr_RAM[9]  = 32'h00804020; // add $8 $4 $0 
        instr_RAM[10]  = 32'h00804020; // add $8 $4 $0
        instr_RAM[11]  = 32'h03E00008; // jr $31 // jump register back to iram index 1
        instr_RAM[12] = 32'h1000FFF6; // beq $t0 $t0 BEG // branch back to the beginning
        */

        

        // following code proves stall 
        //instr_RAM[0] = 32'h00000000;   // nop fill pipeline
        //instr_RAM[1] = 32'h8C030004;   // lw $3,4 ;memory(04)=AAAAAAAA
        //instr_RAM[2] = 32'h8C020000;   // lw $2,0 ;memory(00)=55555555
       // instr_RAM[3] = 32'h00430820;   // add $1,$2,$3
      

       
        /*
        instr_RAM[0] = 32'h00000000;   // nop fill pipeline
        instr_RAM[1] = 32'h00000000;   // nop fill pipeline
        instr_RAM[2] = 32'h00000000;   // nop fill pipeline
        instr_RAM[3] = 32'h8C020000;   // lw $2,0 ;memory(00)=55555555
        instr_RAM[4] = 32'h8C030004;   // lw $3,4 ;memory(04)=AAAAAAAA
        instr_RAM[5] = 32'h00430820;   // add $1,$2,$3
        instr_RAM[6] = 32'hAC010008;   // sw $1,4 ;memory(08)=FFFFFFFF
        instr_RAM[7] = 32'h1022FFFF;   // beq $1,$2,-4  
        instr_RAM[8] = 32'h1021FFFA;   // beq $1,$1,-24 (Assume delay slot present, so it  
                                       // New PC = PC+4-24 = PC-20
        */

    end 
    
    //register the output from the inst_RAM
    //  based on the address pointer.
    logic [31:0] sig_instruction;
    always @ (posedge clock)
        sig_instruction <= instr_RAM[instr_mem_addr];
    

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Program Counter (PC)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //generate local variables
    logic [9:0] PC, Next_PC;
    
    //combinational logic to determine next PC value
    always @ (*) 
        if(reset | (ip_zero && ip_branch)) // has to be or to check both conditions
            Next_PC <= ip_add_result;  // next address is calculated
        else if (ip_stall)  //if the stall signal is high we want to stop the PC until the stall condition has passed.
            Next_PC <= PC;
        else
            Next_PC <= PC + 4;
        
    
    //Register the next PC value
    always @ (posedge clock) 
        if(reset)
            PC <= 0;
        else
            PC <= Next_PC;
        
               
    //assign the address for the instruction memory
    //  The instruction memory is 32 bits per memory location. 
    //  So it is 4 Bytes per memory location. 
    //  Due to this we drop the two least significant bits of the PC
    assign instr_mem_addr = Next_PC >> 2;
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Pipeline Register
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    //Register Signals
    logic [31:0] reg_instruction;
    logic [9:0] reg_PC;
    
    //Register block
    always @ (posedge clock) begin
        if (reset | (ip_zero && ip_branch)) begin
            reg_instruction <= 0;  //registers reset
            reg_PC          <= 0;
            end
        else if (ip_stall) begin //if stall then we hold the current values
            reg_instruction <= reg_instruction;  //registers retain same value to stall
            reg_PC          <= reg_PC;
            end
        else begin
            reg_instruction <= sig_instruction;   // signal_instruction (what is read out of the iram in the particular clock cycle)
            reg_PC          <= PC;              // output is assigned to registers 
            end
    end
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Assign the Outputs
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    assign op_instruction = reg_instruction;
    assign op_PC          = reg_PC;
    assign op_PC_plus_4   = reg_PC + 4;

endmodule