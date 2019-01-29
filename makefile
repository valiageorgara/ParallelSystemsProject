CC = mpicc
CC_FLAGS = -fopenmp
LIBS = -lm

EXEC = exec
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
OUTPUT = filtered_waterfall_grey_1920_2520.raw filtered_waterfall_1920_2520.raw

mpi:
	$(CC) -o $(EXEC) $(SOURCES) $(LIBS)

openmp:
	$(CC) $(CC_FLAGS) -o exec $(SOURCES) $(LIBS)

clean:
	rm -f $(EXEC) $(OBJECTS) $(OUTPUT)
