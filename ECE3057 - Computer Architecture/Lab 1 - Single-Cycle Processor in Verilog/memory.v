// vhdl: sudhakar yalamanchili
// translated to verilog by yehowshua with vhd2l and manual tuning
// https://github.com/ldoolitt/vhd2vl

// data memory component.   

module memory(
input wire clock,
input wire [31:0] address,
input wire [31:0] write_data,
input wire memwrite,

output wire [31:0] read_data
);

// internals
// reverse to [0:31] in case we want
// to use big endian output from 
// gnu objcopy
reg [0:31] dram [0:31];

  // initialize register array to zero
  // could also use ``readmemh`` with
  // gnu opbjcopy
  integer i;
  initial begin
    for (i=0;i<32;i=i+1)
      dram[i] = 0;
  end


  // memory read operation
  // we reverse bits since we declare read_data
  // as [31:0] and dram as [0:31]
  genvar index;
  for(index = 0; index < 32; index = index+1)
    assign read_data[index] = dram[address[6:2]][index];

  // write to memory
  // again, we reverse bits since we 
  // declare write_data as [31:0] and
  // dram as [0:31]
  for(index = 0; index < 32; index = index+1)
    always @(posedge clock)
      if(memwrite == 1'b1)
        dram[address[6:2]][index] <= write_data[index];

endmodule
