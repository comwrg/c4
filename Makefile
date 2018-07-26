CC = gcc
CFLAGS = -std=c99 -g -m32

TESTS = func_main \
		printf

RED   = '\033[0;31m'
GREEN = '\033[0;32m'
NC    = '\033[0m' # No Color


all: c4 alithmetic

c4:
	$(CC) $(CFLAGS) -o c4 c4.c

alithmetic:
	$(CC) $(CFLAGS) -g -o arithmetic arithmetic.c

test:
	@for t in $(TESTS); do \
		./c4 test/$$t.c > $$t.out; \
		diff test/$$t.out $$t.out; \
		if [ $$? -eq 0 ]; then \
			echo ${GREEN}Test $$t success.${NC}; \
		else \
			echo ${RED}Test $$t failed.${NC}; \
			exit 1; \
		fi \
	done

clean:
	-rm c4 arithmetic *.out

.PHONY: c4 all test
# vim:ft=make
