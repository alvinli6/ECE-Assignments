all: sim

mips_tb: CONTROL.sv DMEMORY.sv EXECUTE.sv IDECODE.sv IFETCH.sv MIPS.sv MIPS_tb.sv STALL_CONT.sv WRITE_BACK.sv FWD_CONT.sv
	iverilog -g2005-sv -s MIPS_pipelined_tb -o $@ $^

sim: mips_tb
	vvp $<

clean:
	rm -f mips_tb sim.vcd
