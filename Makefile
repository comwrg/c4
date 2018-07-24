all: clean c4 alithmetic

c4:
	gcc -g -o c4 c4.c

alithmetic:
	gcc -g -o arithmetic arithmetic.c

clean:
	-rm c4 arithmetic

# vim:ft=make
