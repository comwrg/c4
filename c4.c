#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "instruction.h"
#include "register.h"

#define POOL_SIZE 256*1024 

char *src, *psrc;
int line; // record line number in src

int token, token_val, token_len;

int *text, *pc, 
    *stack, *esp, *ebp
    ;
char  *data, *pdata;

void *eax, *ax;

struct identifier {
    int token;
    char *name;
    int class;
    int type;
    size_t value;
};

struct identifier   *symbols, *psymbols,
                    *pmain // point to func main
                    ;

struct ParamInfo {
    int flag;
    int value;
    int len;
};

enum { 
    Id = 128, Fun, Num, Sys,
    KB, /* KB is Keywords begin mark */
        Int, Return, 
    KE /* KE is Keywords end mark */
};
char *KEYWORDS = "int return";
char *KEYWORD_MAIN = "main";

void next();

void dispose() {
    if (eax)
        free(eax),     eax     = NULL;
    if (src)
        free(src),     src     = NULL;
    if (text)
        free(text),    text    = NULL;
    if (stack)
        free(stack),   stack   = NULL;
    if (symbols)
        free(symbols), symbols = NULL;
    if (data)
        free(data),    data    = NULL;
}

void fail(char *tip) {
    dispose();
    printf("error: %s\n", tip);
    exit(-1);
}


void init_malloc() {
    eax = malloc(4); // 4 bytes
    ax = (((int) eax) + 2); // last 2 bytes in eax

    src = (char *) malloc(POOL_SIZE);
    if (!src) {
        fail("malloc src failed");
    }
    psrc = src;
    line = 1;

    text = malloc(POOL_SIZE);
    if (!text) {
        fail("malloc text failed");
    }
    pc = text;

    stack = malloc(POOL_SIZE);
    if (!stack) {
        fail("malloc stack failed");
    }
    ebp = esp = (int)stack + POOL_SIZE;

    symbols = (struct identifier *) malloc(POOL_SIZE);
    if (!symbols) {
        fail("malloc symbols failed");
    }
    psymbols = symbols;
    memset(symbols, 0, POOL_SIZE);

    data = (char *) malloc(POOL_SIZE);
    if (!data) {
        fail("malloc data failed");
    }
    pdata = data;
}

void init_symbols() {
    psrc = KEYWORD_MAIN;
    next(); 
    psymbols->token = token;
    psymbols->name = token_val;
    pmain = psymbols;

    psrc = KEYWORDS;
    for (int i = KB+1; i < KE; ++i) { // loop keywords
        next();
        psymbols->token = i;
        psymbols->name = token_val;
    }

    psrc = INSTRUCTIONS;
    for (int i = IB+1; i < IE; ++i) {
        next();
        psymbols->token = Sys;
        psymbols->value = i;
        psymbols->name = token_val;
    }
        
        
    psrc = src;
}

void read_src(char *filename, char *p) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fail("open file error");
    }

    int c;
    while ( (c = fgetc(fp)) != EOF) {
        *p++ = c;
    }

    *p = '\0';

    fclose(fp);
}

void next() {
    while (token = *psrc++) {
        if (token == '\n') {
            ++line;
        } else if (token == '#') {
            // skip, not support include
            while (*psrc != '\0' && *psrc != '\n') {
                psrc++;
            }
        } else if ((token >= 'a' && token <= 'z') 
                || (token >= 'A' && token <= 'Z') 
                || (token == '_')) {
            char *pos = psrc - 1; // record the beginning of a word
            int hash = token;
            while ((*psrc >= 'a' && *psrc <= 'z') 
                || (*psrc >= 'A' && *psrc <= 'Z') 
                || (*psrc >= '0' && *psrc <= '9') 
                || (*psrc == '_')) {             
                hash = hash * 147 + *psrc++;
            }

            for (psymbols = symbols; psymbols->token; ++psymbols) {
                if (!memcmp(psymbols->name, pos, psrc - pos)) {
                    token = psymbols-> token;
                    token_val = psymbols->value;
                    return;
                }
            }

            // not found
            token = Id;
            token_val = pos;
            token_len = psrc - pos;

            return;
        } else if (token >= '1' && token <= '9') { // number
            // only support dec for the time being
            token_val = token - '0';
            while (*psrc >= '0' && *psrc <= '9') { // 0-9
                token_val = token_val*10  +  *psrc++ - '0';
            }
            token = Num; // mark Num
            return;
        } else if (token == '"') { // string
            token_val = pdata;
            for (; *psrc != '\0' && *psrc != '"'; ++pdata) {
                *pdata = *psrc++;
                if (*pdata == '\\') {
                    *pdata = *psrc++;
                    if (*pdata == 'n') {
                        *pdata = '\n';
                    }
                }
            }
            ++psrc;
            *pdata++ = '\0';
            return;
        }

        char *chs = "(){};=";
        int ch;
        while (ch = *chs++) {
            if (token == ch) {
                return ;
            }
        }

    }
}

void match(int tk) {
    if (token != tk) {
        fail("token != tk");
    }
    next();
}

void function_parameter() {
}

void function_body() {
    struct identifier *local_symbols = malloc(POOL_SIZE);
    memset(local_symbols, 0, POOL_SIZE);
    struct identifier *p;
    int offset = 0;
    while (token != '}') {
        if (token == Return) {
            match(Return);
            *++pc = EXIT;
            if (token != ';') {
                *++pc = token_val;
            } 
            match(token);
            match(';');
        } else if (token == Sys) { // system func
            int func = token_val;
            match(Sys);
            match('(');

            // init a stack for reverse params, direction is low -> high
            struct ParamInfo *stk = malloc(POOL_SIZE); 
            struct ParamInfo *pstk = stk;             
            for (++pstk; token != ')'; ++pstk) {
                pstk->flag = token;
                pstk->value = token_val;
                pstk->len = token_len;
                match(token);
            }
            for (--pstk; stk != pstk; --pstk) { // reverse params
                if (pstk->flag == Id) {
                    for (p = local_symbols; p->token; ++p) { // loop for find id
                        if (!memcmp(p->name, pstk->value, pstk->len)) {
                            w_push_offset(p->value, EBP);
                            break;
                        }
                    }
                    if (!p->token) {
                        fail("variable can be used before define");
                    }
                } else if (pstk->flag == Num || pstk->flag == '"') {
                    w_push('$', pstk->value);
                }
            }
            free(stk); stk = NULL;
            w_call(func); // call func

            match(')');
            match(';');
        } else if (token == Int) {
            match(Int);
            if (token != Id) {
                fail("should be variable name after variable type");
            }
            if (!psymbols->token) {
                for (p = local_symbols; p->token; ++p) {
                    if (!memcmp(p->name, token_val, token_len)) {
                        // found in local symbols, err
                        fail("local variable redefine");
                    }
                }
                p->token = Id;
                p->name = token_val;
                p->type = Int;
                
                w_sub('$', 4, ESP); // sub $4, $esp 
                offset -= 4;
                p->value = offset;
            } else {
                // TODO
                // found in global symbols
            }
            match(Id);
            if (token == '=') {
                match('=');
                if (token != Num) {
                    fail("an integer variable can only be assigned to an integer");
                }
                w_mov_offset('$', token_val, p->value, EBP); // e.g. mov $0, -4(%ebp)
                match(Num);
            } else if (token == ',') {
                // TODO like int a,b, define serveral variables one time
            }
            match(';');
        }
    }

    free(local_symbols); local_symbols = NULL;
}

void function_declaration() {
    match('(');
    function_parameter();
    match(')');
    match('{');
    function_body();
    match('}');
}

void global_declaration() {
    int type;
    if (token == Int) {
        type = Int;
        match(Int);
    }

    match(Id);

    psymbols->type = type;
    if (!psymbols->token) {
        psymbols->token = token;
        psymbols->name = token_val;
    }

    if (token == '(') {
        psymbols->class = Fun;
        *++pc = 0;  // text and text delimiter
        psymbols->value = (size_t) (pc+1);
        function_declaration();
    }

}

void program() {
    next();
    global_declaration();
    /* while (token > 0) { */
    /* } */
}

void eval() {
    int op, func, offset, *chs, *args, flag;
    pc = pmain->value;
    while(op = *pc++) {
        switch (op) {
            case EXIT:
                printf("exit(%d)\n", *pc++); 
                return; 
            case MOV:  mov();  break;
            case PUSH: push(); break;
            case POP:  pop();  break;
            case SUB:  sub();  break;
            case CALL:
                func = *pc++;
                switch(func) {
                    case PRTF:
                        chs = *esp++;
                        args = malloc(1024);
                        offset = 0;
                        for (char *tk = chs; *tk != '\0'; ++tk) {
                            if (*tk != '%') {
                                continue;
                            }
                            if (*(tk+1) == '%') {
                                ++tk;
                                continue;
                            }
                            *(args + offset++) = *esp++;
                        }
                        vprintf((const char *) chs, (va_list) args);
                        free(args); args = NULL;
                        break;
                    default:
                        fail("unknow func");
                }
                break;
            default:
                fail("unknow instruction");
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("no input filename.\n");
        return -1;
    }
    init_malloc();
    init_symbols();
    read_src(*(argv+1), src);

    program();

    eval();

    dispose();
    return 0;
}

// vim: et:ts=4:sw=4:fdm=indent:
