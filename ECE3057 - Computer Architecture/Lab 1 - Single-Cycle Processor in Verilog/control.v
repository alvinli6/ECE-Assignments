// vhdl: sudhakar yalamanchili
// translated to verilog by yehowshua with vhd2l and manual tuning
// https://github.com/ldoolitt/vhd2vl

// control unit. simply implements the truth table for a small set of
// instructions 

module control(
input wire [5:0] opcode,

output wire [1:0] regdst,
output wire memread,
output wire [1:0] memtoreg,
output wire memwrite,
output wire [1:0] alusrc,
output wire regwrite,
output wire branch,
output wire do_jump,
output wire [2:0] aluop
);

//internals
wire rformat; // if instruction is R-type
wire lw; // if instruction is lw
wire sw; // if instruction is sw
wire beq; // if instruction is beq
wire alui; // if instruction is immediate ALU instruction
wire s_extend; // if immediate needs to be sign extended (or lui)
wire z_extend; // if immediate needs to be zero extended (or lui)
wire lui; // if instruction is lui
wire j; // if instruction is j (jump)
wire jal; // if instruction is j (jump and link)

//immediate instructions - internals
wire andi;
wire ori; // if instruction is ori
wire addi;
wire slti;

  // recognize opcode for each instruction type
  assign rformat = opcode == 6'b000000 ? 1'b1 : 1'b0; //R-type
  assign lw = opcode == 6'b100011 ? 1'b1 : 1'b0; //I-type
  assign sw = opcode == 6'b101011 ? 1'b1 : 1'b0; //I-type
  assign beq = opcode == 6'b000100 ? 1'b1 : 1'b0; //I-type
  assign ori = opcode == 6'b001101 ? 1'b1 : 1'b0; //I-type
  assign lui = opcode == 6'b001111 ? 1'b1 : 1'b0; //I-type
  assign j = opcode == 6'b000010 ? 1'b1 : 1'b0; //J-type
  assign jal = opcode == 6'b000011 ? 1'b1 : 1'b0; //J-type

  assign alui = ori; // alui is 1 if instruction is immediate ALU (ori | andi | addi | slti)... etc.
  
  assign s_extend = lui | lw | sw;
  assign z_extend = lui | ori; // OR other logicial alui instructions

  // implement each output signal as the column of the truth
  // table which defines the control
  assign regdst = {rformat,jal}; // 10 for R-type instructions, 01 for jal (write to $31), 00 for not R-type instructions
  assign alusrc[1:0] = {s_extend,z_extend}; // 11 for lui, 10 for sign extended immediates, 01 for zero extended immediates, 00 for no immediate
								// ALUsrc here serves as the "ExtSel" signal that determines the TYPE of extension necessary for the immediate field.
  assign memtoreg[1:0] = {lw,jal}; // 10 for writing from memory to register, 01 for writing PC to register, 00 for writing ALU to register
  assign regwrite = rformat | lw | ori | lui | jal; // 1 for writing to any register at all
  assign memread = lw; // 1 for reading from memory
  assign memwrite = sw; // 1 for writing to memory
  assign branch = beq; // 1 if any type of branch
  assign do_jump = j | jal; // 1 if doing unconditional absolute jumps (j or jal)
  assign aluop[2:0] = {alui,rformat,beq}; // 000 for lw/sw/lui, 001 for branch, 010 for R-type, 100 for ALUi
										  // (000 is +, 001 is -, 010 is function, 100 is opcode)... these are the 4 options of the ALUop MUX

endmodule