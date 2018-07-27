#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define POOL_SIZE 256*1024 

char *src, *psrc;
int line; // record line number in src

int token, token_val;

int *text, *pc, 
    *stack, *sp, *bp
    ;
char  *data, *pdata;

void *eax, *ax;

struct identifier {
    int token;
    int hash;
    char *name;
    int class;
    int type;
    size_t value;
};

struct identifier   *symbols, *psymbols,
                    *pmain // point to func main
                    ;

// instructions
enum {
    EXIT = 256, CALL, 

    PUSH_A, POP_A, // push [address]
    PUSH_C, POP_C, // push [const value]
    
    MOV_A, // mov [address], [address] 
    MOV_C, // mov [address], [const value]

    IB, /* IB is instructions begin mark */
        PRTF, 
    IE /* IE is instructions end mark */
};
char *INSTRUCTIONS = "printf";

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
    bp = sp = (int)stack + POOL_SIZE;

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
    pmain = psymbols;

    psrc = KEYWORDS;
    for (int i = KB+1; i < KE; ++i) { // loop keywords
        next();
        psymbols->token = i;
    }

    psrc = INSTRUCTIONS;
    for (int i = IB+1; i < IE; ++i) {
        next();
        psymbols->token = Sys;
        psymbols->value = i;
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
                if (psymbols->hash == hash && !memcmp(psymbols->name, pos, psrc - pos)) {
                    break;
                }
            }

            if (!psymbols->token) { // not found
                psymbols->name = pos;
                psymbols->hash = hash;
                psymbols->token = Id;
            }

            token = psymbols->token;
            token_val = psymbols->value;
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

        char *chs = "(){};";
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
            *++pc = PUSH_A; *++pc = &bp; // push bp
            *++pc = MOV_A; *++pc = &sp; *++pc = &bp; // mov bp, sp, let bp = sp
            int *stk = malloc(1024); int *pstk = stk; // init a stack for reverse params, direction is low -> high
            while (token != ')') {
                *++pstk = token_val;
                match(token);
            }
            while (stk != pstk) { // reverse params
                *++pc = PUSH_C;
                *++pc = *pstk--;
            }
            free(stk); stk = NULL;
            *++pc = CALL; *++pc = func; // call func
            *++pc = POP_C; *++pc = &bp; // pop bp

            match(')');
            match(';');
        }
    }
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
    int op, func, offset, *chs, *args;
    pc = pmain->value;
    while(op = *pc++) {
        switch (op) {
            case EXIT:
                printf("exit(%d)\n", *pc++); 
                return; 
            case MOV_A:
                **((int **)(pc++)) = **((int **)(pc++));
                break;
            case MOV_C:
                **((int **)(pc++)) = *pc++;
                break;
            case PUSH_A:
                *--sp = **((int **)(pc++));
               break;
            case PUSH_C:
               *--sp = *pc++;
               break;
            case POP_A:
                **((int **)(pc++)) = *sp++;
                break;
            case POP_C:
                *pc++ = *sp++;
                break;
            case CALL:
                func = *pc++;
                switch(func) {
                    case PRTF:
                        chs = *sp++;
                        args = malloc(1024);
                        offset = 0;
                        while (bp != sp) {
                            *(args + offset++) = *sp++;
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
