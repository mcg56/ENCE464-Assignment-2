all: poisson

# -g outputs debugging information
# -Wall enables all warnings
CFLAGS = -g -Wall

# -lpthread links the pthread library
LDLIBS = -lpthread

poisson: poisson.c

.PHONY: disassembly
disassembly: poisson.s

poisson.s: poisson
	objdump -S --disassemble $< > $@

.PHONY: test
test: poisson
	./test.sh

.PHONY: clean
clean:
	rm -f poisson *.o *.s
