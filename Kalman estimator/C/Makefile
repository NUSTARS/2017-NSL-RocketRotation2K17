CC=gcc
CFLAGS=-I.
DEPS = kalmanEstimator.h
OBJ = test.o kalmanEstimator.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm *.o
	rm test
