NAME=visor
GTKFLAGS=`pkg-config gtkmm-3.0 --cflags --libs`
SRC=base.cpp
CFLAGS=-g -Wall -o $(NAME)
CC=g++

all: main

main: $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(GTKFLAGS)
