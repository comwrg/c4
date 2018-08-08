/*
 * instruction.h
 * include instruction set
 */

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

int *pc;
int *sp;

// instructions
enum {
    EXIT = 256, CALL, 
    
    PUSH,
    POP,
    MOV,

    IB, /* IB is instructions begin mark */
        PRTF, 
    IE /* IE is instructions end mark */
};
extern char *INSTRUCTIONS;

void w_mov(char flag, int src, void **dst);
void mov();
void w_push(char flag, int src);
void push();
void w_pop(char flag, int src);
void pop();
void w_call(void *func);



#endif /* !INSTRUCTION_H */
// vim: et:ts=4:sw=4:
