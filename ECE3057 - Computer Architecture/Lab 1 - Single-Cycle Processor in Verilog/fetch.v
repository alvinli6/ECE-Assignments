// vhdl: sudhakar yalamanchili
// translated to verilog by yehowshua with vhd2l and manual tuning
// https://github.com/ldoolitt/vhd2vl

// instruction fetch behavioral model. instruction memory is
// provided within this model. if increments the pc,  
// and writes the appropriate output signals. 


module fetch(
input wire clock,
input wire reset,
input wire [31:0] branch_addr, // starting address of branch
input wire do_branch, // whether or not to do a branch instruction
input wire do_jump, // whether or not to do a jump instruction
input wire [27:0] jump_shifted_address, // lower 28 bits of jump address (target address * 4)

output wire [31:0] instruction,
output wire [31:0] pc4
);

// internals
reg [31:0] pc;
reg [31:0] next_pc;
wire [31:0] full_jump_address;

  // load the program into the memory
  // a better way to do this is with gnu-as
  // and verilog primitive ``readmemh``.
  // note we use [0:31]. This allows us to 
  // paste instructions from big-endian
  // assemblers lik gnu-as in big-endian
  // mode and most online mips assemblers
  reg [0:31] iram [0:18];
    initial begin 
      iram[0] = 32'hac030000; // sw	$3, 0x0 (M[0x0] = 3)
      iram[1] = 32'h8c040000; // lw	$4, 0x0 ($4 = 3)
      iram[2] = 32'h00832820; // add $5, $4, $3 ($5 = 3 + 3 = 6)
      iram[3] = 32'h00e52822; // sub $5, $7, $5 ($5 = 7 - 6 = 1)
      iram[4] = 32'h00a12824; // and $5, $5, $1 ($5 = 0001 AND 0001 = 1)
	  iram[5] = 32'h3C06abcd; // lui $6, 0xABCD ($6 = 0xabcd0000)
      iram[6] = 32'h34c6e039; // ori $6, $6, 0xe039 ($6 = 0xabcd0000 OR 0x0000e039 = 0xabcde039)
	  iram[7] = 32'h0023402a; // slt $8, $1, $3 ($8 = 1... $1 < $3)
	  iram[8] = 32'h0061202a; // slt $4, $3, $1 ($4 = 0... $3 > $1)
	  iram[9] = 32'h0025402a; // slt $8, $1, $5 ($8 = 0... $1 = $5)
      iram[10] = 32'h10a10003; // beq $5, $1, location ($5 = $1 = 1 so jump to location)
								// current jump is +3 words
	  iram[11] = 32'h00000000; // nop - branch delay slot
	  iram[12] = 32'h00204820; // add $9, $1, $0 ($9 = 1 + 0 = 1)
	  iram[13] = 32'h01214822; // sub $9, $9, $1 ($9 = 1 - 1 = 0)
	  iram[14] = 32'h0c000011; // jal 0x0011 (jump to iram[17] and link iram[15]... store $0x003c in $31)
	  iram[15] = 32'h00000000; // nop - jump delay slot... technically ONLY need if jal saves PC+8 in $31
	  iram[16] = 32'h00205020; // add $10, $1, $0 ($10 = 1 + 0 = 1) ... never reaches this instruction
	  iram[17] = 32'h0800000c; // j 0x000c (jump to iram[12])
	  iram[18] = 32'h00000000; // nop - jump delay slot
    end 


  // access instruction pointed to by current pc
  assign instruction = iram[pc[6:2]]; // increase to 5 bits [6:2] to use MORE THAN 16 instructions in iram

  // compute value of next pc
  always @(*) begin
    if(do_branch == 1'b1)
      next_pc = branch_addr;
    else if (do_jump == 1'b1)
      next_pc = full_jump_address;
	else
	  next_pc = pc + 4;
  end
  
  // compute full jump address
  assign full_jump_address = {pc4[31:28],jump_shifted_address};

  // update the pc on the next clock			   
  always @(posedge clock) begin
    if(reset == 1'b1)
      pc <= 32'h00000000;
    else
      pc <= next_pc; // flip flop (PC register)
  end

  assign pc4 = pc + 4;

endmodule