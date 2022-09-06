all: poisson

# -g outputs debugging information
# -Wall enables all warnings
CFLAGS = -g -Wall

# -lpthread links the pthread library
LDLIBS = -lpthread

poisson: poisson.c

.PHONY: test
test: poisson
	./test.sh

.PHONY: clean
clean:
	rm -f poisson *.o
