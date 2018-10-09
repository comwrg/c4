CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -g -m32 -coverage 

OBJS = c4.o instruction.o register.o

TESTS = func_main \
		printf_str \
		printf_int \
		local \
		arithmetic_add \
		arithmetic_sub

RED   = '\033[0;31m'
GREEN = '\033[0;32m'
NC    = '\033[0m' # No Color


all: c4 alithmetic

c4: $(OBJS)
	$(CC) $(CFLAGS) -o c4 $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

alithmetic:
	$(CC) $(CFLAGS) -g -o arithmetic arithmetic.c

test:
	@passed=0; \
	failed=0; \
	n=0; \
	for t in $(TESTS); do \
		./c4 test/$$t.c > $$t.out; \
		diff test/$$t.out $$t.out > $$t.diff; \
		if [ $$? -eq 0 ]; then \
			echo $$n. ${GREEN}PASSED: $$t${NC}; \
			rm $$t.out; \
			passed=$$(echo $$(($$passed+1))); \
		else \
			echo $$n. ${RED}FAILED: $$t${NC}; \
			echo "$$(cat $$t.diff)"; \
			failed=$$(echo $$(($$failed+1))); \
		fi; \
		rm $$t.diff; \
		n=$$(echo $$(($$n+1))); \
	done; \
	echo ; \
	echo ---------------------; \
	if [ $$failed -eq 0 ]; then \
		echo ${GREEN}All PASSED${NC}; \
	else \
		echo ${GREEN}SUM: $$n \ \ ${RED}FAILED: $$failed${NC}; \
	fi; \
	echo ---------------------; \
	if [ $$failed -ne 0 ]; then \
		exit 1; \
	fi; \


clean:
	-rm c4 arithmetic *.o *.gcno

.PHONY: c4 all test
# vim:ft=make
