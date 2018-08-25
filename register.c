#include "register.h"

int *get_reg_addr(int r) {
    switch (r) {
        case EAX: return &eax;
        case EBP: return &ebp;
        case ESP: return &esp;
        default:  return 0;
    }
}




// vim: et:ts=4:sw=4:
