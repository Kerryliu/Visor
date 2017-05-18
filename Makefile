NAME=visor
GTKFLAGS=`pkg-config gtkmm-3.0 cairo --cflags --libs`
SRC=base.cpp window.cpp
CFLAGS=-std=c++1z -g -Wall -o $(NAME)
CC=g++

all: main

main: $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(GTKFLAGS)
