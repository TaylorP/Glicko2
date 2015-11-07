IDIR=./
CC=g++
CFLAGS=-I$(IDIR)
PLAIN_OBJ = main.o glicko/rating.o 

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

glicko_basic: $(PLAIN_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -rf *.o
	rm -f glicko_basic