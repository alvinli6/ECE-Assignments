SIM_SRC  = cache.cpp core.cpp dram.cpp memsys.cpp sim.cpp
SIM_OBJS = $(SIM_SRC:.cpp=.o)

all: $(SIM_SRC) sim

%.o: %.cpp
	g++ -std=c++11 -Wall -c -o $@ $<

sim: $(SIM_OBJS)
	g++ -std=c++11 -Wall -o $@ $^

clean:
	rm sim *.o
