all: threads

# -g outputs debugging information
# -lpthread links the pthread library
threads: threads.c
	gcc -g -o threads threads.c -lpthread

clean:
	rm -f threads *.o
