#include <stdio.h>
#include "register.h"

int *get_reg_addr(int r) {
    switch (r) {
        case EAX: return &eax;
        case ECX: return &ecx;
        case EDX: return &edx;
        case EBP: return &ebp;
        case ESP: return &esp;
        default:  return 0;
    }
}

const char *get_reg_name(int r) {
    static char name[128];
    switch (r) {
        case EAX: sprintf(name, "%s", "eax"); break;
        case ECX: sprintf(name, "%s", "ecx"); break;
        case EDX: sprintf(name, "%s", "edx"); break;
        case EBP: sprintf(name, "%s", "ebp"); break;
        case ESP: sprintf(name, "%s", "esp"); break;
        default:  sprintf(name, "%d", r);     break;
    }
    return name;
}



// vim: et:ts=4:sw=4:
