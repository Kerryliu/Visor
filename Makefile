NAME=visor
GTKFLAGS=`pkg-config gtkmm-3.0 cairo --cflags --libs`
MAGIC=-lstdc++fs
SRC=base.cpp window.cpp device.cpp
CFLAGS=-std=c++1z -g -Wall -o $(NAME)
CC=g++

all: main

main: $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(GTKFLAGS) $(MAGIC)
