CC = g++

SRC   = src/ttp-scatter.cpp
FLAGS = -Wall -lm
OUT   = ttp-scatter

compile:
	$(CC) $(FLAGS) $(SRC) -o $(OUT)
	
test:
	time ./$(OUT) data/circ4.txt