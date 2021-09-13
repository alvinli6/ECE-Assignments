SIM_SRC  = sim.cpp pipeline.cpp bpred.cpp
SIM_OBJS = $(SIM_SRC:.cpp=.o)

all: $(SIM_SRC) sim

%.o: %.c
	g++ -std=c++11 -O3 -c -o $@ $<

sim: $(SIM_OBJS)
	g++ -std=c++11 -O3 -o $@ $^

clean:
	rm sim *.o
