all: poisson

# -g outputs debugging information
# -lpthread links the pthread library
poisson: poisson.c
	gcc -g -o poisson poisson.c -lpthread

test: poisson
	./test.sh

clean:
	rm -f poisson *.o
