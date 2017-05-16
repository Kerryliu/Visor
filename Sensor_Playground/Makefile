NAME=test
SRC=test.cpp device.cpp
CFLAGS=-std=c++1z -g -Wall -o
CC=g++
all: main
main: $(SRC)
	$(CC) $(CFLAGS) $(NAME) -include $(SRC) -lstdc++fs 
