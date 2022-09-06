all: poisson

# -g outputs debugging information
# -Wall enables all warnings
CFLAGS = -g -Wall

# -lpthread links the pthread library
LDLIBS = -lpthread

poisson: poisson.c

test: poisson
	./test.sh

clean:
	rm -f poisson *.o
