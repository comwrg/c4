/**
 * use AT&T syntax
 */
#include "instruction.h"
#include "register.h"
#include <stdio.h>

char *INSTRUCTIONS = "printf";
int S = 0;

/**
 * mov $1, -4(%eax)
 */
void w_mov_offset(char flag, int src, int offset, void **dst) {
    *++pc = MOV;
    *++pc = flag;
    *++pc = src;
    *++pc = offset;
    *++pc = dst;

    if (!S) 
        return;
    printf("mov \t%c%s, ", flag, get_reg_name(src));
    if (offset) {
        printf("%d(%%%s)", offset, get_reg_name(dst));
    } else {
        printf("%%%s", get_reg_name(dst));
    }
    printf("\n");
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
        src = **(int **)get_reg_addr(*pc++);
    } else if (flag == '$') {
        src = *pc++;
    }
    int offset = *pc++;
    int dst = *pc++;
    int r = get_reg_addr(dst);
    if (r) {
        *(int *)(*(int *)r + offset) = src;
    } else {
        *(int *)dst = src;
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

    if (!S)
        return;
    printf("push \t%d(%%%s)\n", offset, get_reg_name(src));
}

void w_push(char flag, int src) {
    *++pc = PUSH;
    *++pc = flag;
    if (flag == '%') {
        *++pc = 0; // offset
    }
    *++pc = src;

    if (!S)
        return;
    printf("push \t%c%s\n", flag, get_reg_name(src));
}

void push() {
    char flag = *pc++;
    if (flag == '%') {
        int offset = *pc++;
        int src = *pc++;
        int r = get_reg_addr(src);
        if (r) {
            *--esp = *(int *)((*(int *)r) + offset);
        } else {
            *--esp = *(int *)(src + offset);
        }
    } else if (flag == '$') {
       *--esp = *pc++;
    }
}

void w_pop(char flag, int src) {
    *++pc = POP;
    *++pc = flag;
    *++pc = src;

    if (!S)
        return;
    printf("pop \t%c%s\n", flag, get_reg_name(src));
}

void pop() {
    char flag = *pc++;
    if (flag == '%') {
        **((int **)(pc++)) = *esp++;
    } else if (flag == '$') {
        *pc++ = *esp++;
    }
}

void w_call(void* func) {
    *++pc = CALL;
    *++pc = func;

    if (!S)
        return;
    printf("call \t%p\n", func);
}

void w_add(char flag, int src, int **dst) {
    *++pc = ADD;
    *++pc = flag;
    *++pc = src;
    *++pc = dst;

    if (!S)
        return;
    if (flag == '$') {
        printf("add \t$%d, %%%s\n", src, get_reg_name(dst));
    } else if (flag == '%') {
        printf("add \t%%%s, %%%s\n", get_reg_name(src), get_reg_name(dst));
    }
}

void add() {
    char flag = *pc++;
    int src;
    if (flag == '%') {
        src = **(int **)get_reg_addr(*pc++);
    } else if (flag == '$') {
        src = *pc++;
    }
    int dst = *pc++;
    int r = get_reg_addr(dst);
    if (r) {
        **(int **)r += src;
    } else {
        *(int *)dst += src;
    }
}

/**
 * sub $16, %esp
 */
void w_sub(char flag, int src, int **dst) {
    *++pc = SUB;
    *++pc = flag;
    *++pc = src;
    *++pc = dst;

    if (!S)
        return;
    if (flag == '$') {
        printf("sub \t$%d, %%%s\n", src, get_reg_name(dst));
    } else if (flag == '%') {
        printf("sub \t%%%s, %%%s\n", get_reg_name(src), get_reg_name(dst));
    }
}

void sub() {
    char flag = *pc++;
    int src;
    if (flag == '%') {
        src =  **(int **)get_reg_addr(*pc++);
    } else if (flag == '$') {
        src = *pc++;
    }

    int dst = *pc++;
    int r = get_reg_addr(dst);
    if (r) {
        **(int **)r -= src;
    } else {
        *(int *)dst -= src;
    }
}
// vim: et:ts=4:sw=4:
