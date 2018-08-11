/**
 * use AT&T syntax
 */
#include "instruction.h"
#include <stdio.h>

char *INSTRUCTIONS = "printf";

/**
 * mov $1, -4(%eax)
 */
void w_mov_offset(char flag, int src, int offset, void **dst) {
    *++pc = MOV;
    *++pc = flag;
    *++pc = src;
    *++pc = offset;
    *++pc = dst;
}

/**
 * mov $1, %eax
 * mov %ebx, %eax
 */
void w_mov(char flag, int src, void **dst) {
    w_mov_offset(flag, src, 0, dst);
}

void mov() {
    char flag = *pc++;
    int src;
    if (flag == '%') {
        src = **((int **)(pc++));
    } else if (flag == '$') {
        src = *pc++;
    }
    int offset = *pc++;
    if (offset) {
        int *dst_address = **(int **)pc++ + offset;
        *dst_address = src;
    } else {
        **(int **)pc++ = src;
    }
}

/**
 * push -4(%esp)
 */
void w_push_offset(int offset, int src) {
    *++pc = PUSH;
    *++pc = '%';
    *++pc = offset;
    *++pc = src;
}

void w_push(char flag, int src) {
    *++pc = PUSH;
    *++pc = flag;
    if (flag == '%') {
        *++pc = 0; // offset
    }
    *++pc = src;
}

void push() {
    char flag = *pc++;
    if (flag == '%') {
        int offset = *pc++;
        if (offset) {
            *--sp = *((int *)((**(int **)pc++) + offset));
        } else {
            *--sp = **(int **)pc++;
        }
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

/**
 * sub $16, %esp
 */
void w_sub(char flag, int src, int **dst) {
    *++pc = SUB;
    *++pc = flag;
    *++pc = src;
    *++pc = dst;
}

void sub() {
    char flag = *pc++;
    if (flag == '%') {
        **((int **) pc++) -= **((int **) pc++);
    } else if (flag == '$') {
        **((int **) pc++) -= *pc++;
    }
}
// vim: et:ts=4:sw=4:
