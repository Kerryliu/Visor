NAME=Visor
GTKFLAGS=`pkg-config gtkmm-3.0 cairo --cflags --libs`
MAGIC=-lstdc++fs
SRC=main.cpp window.cpp device.cpp tree.cpp graph.cpp
CFLAGS=-std=c++1z -g -Wall -o $(NAME)
CC=g++

all: main

main: $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(GTKFLAGS) $(MAGIC)
