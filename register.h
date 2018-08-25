/*
 * register.h
 */

#ifndef REGISTER_H
#define REGISTER_H

void *eax, *ecx, *edx;
int *ebp, *esp;

enum {
    EAX, ECX, EDX,
    EBP, ESP
};

int *get_reg_addr(int);

#endif /* !REGISTER_H */
// vim: et:ts=4:sw=4:
