# See the file /usr/include/make/default.mk for info on how to.

LDLIBS = -lm 
CC = gcc
CFLAGS =  -O3   -Wall 

BINPATH = ${HOME}/color/bin/pc
LCLPATH = .

prog: clr2sat
	echo "To install make install"

CODEC = clr2sat.o graph.o
clr2sat.o: clr2sat.c
clr2sat: $(CODEC) 
	$(CC) $(CFLAGS) $(CODEC) -o $(LCLPATH)/clr2sat $(LDLIBS);

$(CODEC) : mysys.h graph.h makefile

install: clr2sat
	mv $(LCLPATH)/clr2sat $(BINPATH)/clr2sat

doc: README
	less README

clean:
	\rm *.o 
