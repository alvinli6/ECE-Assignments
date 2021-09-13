all: sim

mips_tb: mips_tb.v mips.v fetch.v control.v decode.v execute.v memory.v
	iverilog -s mips_tb -o $@ $^

sim: mips_tb
	vvp $<

clean:
	rm -f mips_tb mips.vcd