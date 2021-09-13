test_matrix : main.o matrix.o complex.o
	g++ -o test_matrix main.o matrix.o complex.o 

main.o : main.cc matrix.h complex.h
	g++ -c main.cc

matrix.o : matrix.cc matrix.h complex.h
	g++ -c matrix.cc 

complex.o : complex.cc complex.h
	g++ -c complex.cc

