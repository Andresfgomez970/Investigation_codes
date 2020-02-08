CC=gcc
GSLUBICATION = /home/$(USER)/local/
CFLAGS= -I. -I$(GSLUBICATION)include/gsl/ -I$(GSLUBICATION)include/ -g
LFLAGS= -L$(GSLUBICATION)lib/ -lgsl -lgslcblas -lm
DEPS = ../allvars.h
OBJ = $(ARG).c
EXEC = $(ARG).out

Main: #$(OBJ) $(DEPS)
	$(CC) -Wall -o $(EXEC) $(OBJ) $(CFLAGS) $(LFLAGS)

runmain: Main
	./$(EXEC) 

%.o: %.c
	gcc -c -o $@ $< $(CFLAGS) $(LFLAGS)

clean:
	rm *.o* *~ 



