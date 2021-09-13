// vhdl: sudhakar yalamanchili
// translated to verilog by yehowshua with vhd2l and manual tuning
// https://github.com/ldoolitt/vhd2vl

//execution unit. only a subset of instructions are supported in this
//model, specifically add, sub, lw, sw, beq, and, or, slt

module execute(
input wire [31:0] pc4,
input wire [31:0] register_rs,
input wire [31:0] register_rt,
input wire [5:0] function_opcode,
input wire [31:0] sign_extend,
input wire [31:0] zero_extend,
input wire [31:0] lower_extend,
input wire [4:0] wreg_rd,
input wire [4:0] wreg_rt,
input wire [2:0] aluop,
input wire branch,
input wire [1:0] alusrc,
input wire [1:0] regdst,

output reg [31:0] alu_result,
output wire [31:0] branch_addr,
output reg [4:0] wreg_address,
output wire do_branch
);

//internals
wire zero;
wire [31:0] ainput;
reg [31:0] binput;
reg [2:0] alu_ctl;

  // compute the two alu inputs
  assign ainput = register_rs;
  always @(*) begin
    if (alusrc == 2'b00) // no immediate or no extended immediate
      binput = register_rt;
    else if (alusrc == 2'b01) // zero extended immediate
      binput = zero_extend;
    else if (alusrc == 2'b10) // sign extended immediate
      binput = sign_extend;
	else if (alusrc == 2'b11) // lower extended (last 16 bits are 0's) immediate
      binput = lower_extend;
  end

  // compute alu_ctl from function_opcode
  // and alu_op 
  always @(*) begin
    // r type instructions (ALUop = function)
    if ((aluop == 3'b010) & (function_opcode == 6'b100000))
      alu_ctl = 3'b010; // add
    else if ((aluop == 3'b010) & (function_opcode == 6'b100010))
      alu_ctl = 3'b110; // subtract
    else if ((aluop == 3'b010) & (function_opcode == 6'b100100))
      alu_ctl = 3'b000; // and
    else if ((aluop == 3'b010) & (function_opcode == 6'b100101))
      alu_ctl = 3'b001; // or
	else if ((aluop == 3'b010) & (function_opcode == 6'b101010))
      alu_ctl = 3'b111; // slt

    // for lw, sw, and lui (ALUop = add)
    else if (aluop == 3'b000)
      alu_ctl = 3'b010; // add (lw, sw, or lui)
	  
	// for beq, other branches (ALUop = subtract)  
    else if (aluop == 3'b001)
      alu_ctl = 3'b110; // subtract (beq)
	  
	// for ALUi instructions... or other instructions where (ALUop = opcode)
	else if (aluop == 3'b100)
      alu_ctl = 3'b001; // ori
  end

  // use alu_ctl to set alu_result
  always @(*) begin
    if (alu_ctl == 3'b010)
      alu_result = ainput + binput; // add
    else if (alu_ctl == 3'b110)
      alu_result = ainput - binput; // subtract
    else if (alu_ctl == 3'b000)
      alu_result = ainput & binput; // and
    else if (alu_ctl == 3'b001)
      alu_result = ainput | binput; // or
    else if (alu_ctl == 3'b111)
      alu_result = (ainput < binput) ? 32'b1 : 32'b0; // slt
  end


  assign zero = (alu_result == 32'h00000000) ? 1'b1 : 1'b0; // zero is 1 if alu_result is equal to 0
  assign do_branch = (branch & zero); // only do_branch (1) if branch is 1 and alu_result is 0 (applies only for beq)

  always @(*) begin // assigning register storage destination for data
    if (regdst == 2'b00) // write to rt for non R-type instructions
      wreg_address = wreg_rt;
    else if (regdst == 2'b01) // write to $31 for jal
      wreg_address = 5'b11111;
    else if (regdst == 2'b10) // write to rd for R-type instructions
      wreg_address = wreg_rd;
  end
  
  assign branch_addr = pc4 + {sign_extend[29:0],2'b00}; // address to branch to = pc + 4 + (sign_extended bits [29:0] of immediate TIMES 4)

endmodule
