all: matrixOperations.o matrixOperations.h matrix.c
	gcc -o matrixCalculator.out matrix.c matrixOperations.o

matrixOperations.o: matrixOperations.h matrixOperations.c
	gcc -c matrixOperations.c -o matrixOperations.o