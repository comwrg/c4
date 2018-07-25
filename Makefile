CC = gcc
CFLAGS = -std=c99 -g -m32

TESTS = func_main \
		printf

RED = '\033[0;31m'
NC  = '\033[0m' # No Color


all: c4 alithmetic

c4:
	$(CC) $(CFLAGS) -o c4 c4.c

alithmetic:
	$(CC) $(CFLAGS) -g -o arithmetic arithmetic.c

test:
	for t in $(TESTS); do \
		./c4 test/$$t.c > $$t.out; \
		diff test/$$t.out $$t.out > /dev/null || echo ${RED}Test $$t failed.${NC}; \
	done

clean:
	-rm c4 arithmetic *.out

.PHONY: c4 all test
# vim:ft=make
