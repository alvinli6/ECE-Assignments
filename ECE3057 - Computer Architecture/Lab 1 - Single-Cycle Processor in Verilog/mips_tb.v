module mips_tb;
reg clock;
reg reset;

mips mips(
.clock(clock),
.reset(reset)
);

  integer i;

  initial 
  begin
    $dumpfile("mips.vcd");

    // dump iram into vcd
    for(i = 0; i < 19; i = i + 1) // change based on # of instructions
      $dumpvars(0, mips_tb.mips.fetch.iram[i]);

    // dump regfile into vcd
    for(i = 0; i < 32; i = i + 1)
      $dumpvars(0, mips_tb.mips.decode.register_array[i]);
    
    $dumpvars(0, mips_tb);
    

    clock = 0;
    reset = 1;
    #2 reset = 0;
  end

  always
    #1 clock = ~clock;

  initial
    #50 $finish;

endmodule