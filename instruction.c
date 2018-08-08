/**
 * use AT&T syntax
 */
#include "instruction.h"

char *INSTRUCTIONS = "printf";

/**
 * mov $1, %eax
 * mov %ebx, %eax
 */
void w_mov(char flag, int src, void **dst) {
    *++pc = MOV;
    *++pc = flag;
    *++pc = src;
    *++pc = dst;
}

void mov() {
    char flag = *pc++;
    if (flag == '%') {
        **((int **)(pc++)) = **((int **)(pc++));
    } else if (flag == '$') {
        **((int **)(pc++)) = *pc++;
    }
}

void w_push(char flag, int src) {
    *++pc = PUSH;
    *++pc = flag;
    *++pc = src;
}

void push() {
    char flag = *pc++;
    if (flag == '%') {
        *--sp = **((int **)(pc++));
    } else if (flag == '$') {
       *--sp = *pc++;
    }
}

void w_pop(char flag, int src) {
    *++pc = POP;
    *++pc = flag;
    *++pc = src;
}

void pop() {
    char flag = *pc++;
    if (flag == '%') {
        **((int **)(pc++)) = *sp++;
    } else if (flag == '$') {
        *pc++ = *sp++;
    }
}

void w_call(void* func) {
    *++pc = CALL;
    *++pc = func;
}

// vim: et:ts=4:sw=4:
