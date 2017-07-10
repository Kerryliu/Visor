NAME=Visor
GTKFLAGS=`pkg-config gtkmm-3.0 cairo --cflags --libs gconfmm-2.6`
MAGIC=-lstdc++fs
SRC=main.cpp window.cpp device.cpp tree.cpp graph.cpp legend.cpp
CFLAGS=-std=c++14 -g -Wall -o $(NAME)
CC=g++

all: main

main: $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(GTKFLAGS) $(MAGIC)
