CC=gcc
GSLUBICATION = /home/$(USER)/locally_install/
JSONUBICATION = /home/$(USER)/locally_install/json-c-0.15/
CFLAGS=  -Wall -g -I. -I$(GSLUBICATION)include/gsl/ -I$(JSONUBICATION)include/json-c/
LFLAGS= -L$(GSLUBICATION)lib/ -L$(GSLUBICATION)lib/ -lgsl -lgslcblas -lm -ljson-c -lpthread
DEPS = allvars.h
OBJ = main.o two_body_analytical.o input_output.o montecarlo.o mappings.o #Nbody_newton_integrator.o initializations.o  
EXEC = main.out

Main: $(OBJ) $(DEPS)
	$(CC) -Wall -o $(EXEC) $(OBJ) $(CFLAGS) $(LFLAGS)

runmain: Main
	./$(EXEC) 

runmainARG: Main
	./$(EXEC) $(ARG)
#gnuplot -c plot_histograms_orbital_parameters.plt data/montecarlo/densities/distanceNaxis$(ARG) $(shell expr $(ARG) \* $(ARG) \* $(ARG) \* $(ARG) \* $(ARG) \* $(ARG) )

%.o: %.c
	gcc -c -o $@ $< $(CFLAGS) $(LFLAGS)

clean:
	rm *.o* *~ 

runplotARG:
	gnuplot -c plot_histograms_rinterval.plt data/montecarlo/densities/EarthSunSystemAround/DistanceNaxis$(ARG)first$(ARG2) $(ARG2)

wetzel:
	gcc wetzel.c montecarlo.c mappings.c two_body_analytical.c input_output.c -o wetzel.out $(CFLAGS) $(LFLAGS)
