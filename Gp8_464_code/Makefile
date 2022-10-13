all: poisson

# -O3 optimises to highest level
# -pg adds code to enable gprof analysis (NOTE: Please don't remove, it was found that this makes our code faster!)
# -g outputs debugging information
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
