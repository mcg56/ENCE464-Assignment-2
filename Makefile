all: poisson

# -g outputs debugging information
# -O3 optimises to highest level
# -pthread configures threading
CFLAGS = -O3 -pg -g -pthread

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
