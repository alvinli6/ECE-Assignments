// vhdl: sudhakar yalamanchili
// translated to verilog by yehowshua with vhd2l and manual tuning
// https://github.com/ldoolitt/vhd2vl

// instruction decode unit.
// note that this module differs from the text in the following ways
// 1. the memtoreg mux is implemented in this module instead of a (syntactically) 
// different pipeline stage. 

module decode(
input wire clock,
input wire reset,
input wire [31:0] instruction,
input wire [31:0] memory_data,
input wire [31:0] alu_result,
input wire regwrite,
input wire [1:0] memtoreg,
input wire [4:0] wreg_address,
input wire [31:0] pc4,

output wire [31:0] register_rs,
output wire [31:0] register_rt,
output wire [31:0] sign_extend,
output wire [31:0] zero_extend, // for instructions like ori, andi, etc.
output wire [31:0] lower_extend, // for lui
output wire [4:0] wreg_rd,
output wire [4:0] wreg_rt,
output wire [27:0] jump_shifted_address // lower 28 bits of jump address (target address * 4)
);

//internals
reg [31:0] write_data;
wire [4:0] read_register_1_address;
wire [4:0] read_register_2_address;
wire [15:0] instruction_immediate_value;
wire [25:0] jump_address; 

reg [31:0] register_array [0:31];

  // initializing all registers to their reg # (eg. $7 holds 7)
  integer i;
  initial begin
    for (i=0;i<32;i=i+1)
      register_array[i] = i;
  end

  assign read_register_1_address = instruction[25:21];
  assign read_register_2_address = instruction[20:16];
  assign instruction_immediate_value = instruction[15:0];
  assign jump_address = instruction[25:0];

  //read both registers from register file
  assign register_rs = register_array[read_register_1_address];
  assign register_rt = register_array[read_register_2_address];

  // write to the regfile
  always @(*) begin
    if (memtoreg == 2'b00) // writing ALU to register
      write_data = alu_result[31:0];
    else if (memtoreg == 2'b01) // writing PC (PC+4) to register
      write_data = pc4; // NOTE: This should actually be PC+8 if accounting for one delay slot after jump instruction
						// ... but for simplicity's sake and because we're not pipelining here, I will store PC+4 (instead of PC+8) at $31
    else if (memtoreg == 2'b10) // writing memory to register
      write_data = memory_data;
  end
  
  always @ (posedge clock) begin
  if(regwrite == 1'b1)
	register_array[wreg_address] <= write_data;
  end

  // sign extend 16-bits to 32-bits
  assign sign_extend = { {16{instruction_immediate_value[15]}}, instruction_immediate_value};
  
  // zero extend 16-bits to 32-bits
  assign zero_extend = { {16'h0000}, instruction_immediate_value};
  
  // shift immediate left 16 bits and concatenate with 16 lower bit zeroes
  assign lower_extend = {instruction_immediate_value, {16'h0000}};

  // move possible write destinations to execute stage                   
  assign wreg_rd = instruction[15:11];
  assign wreg_rt = instruction[20:16];
  
  // create lower 28 bits of jump target address
  assign jump_shifted_address = {jump_address,2'b00};

endmodule