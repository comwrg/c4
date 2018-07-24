#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define POOL_SIZE 256*1024 

char *src, *psrc;
int line; // record line number in src

int token, token_val;

int *text, *pc, 
    *stack, *sp
    ;
int ax;

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

enum {
    EXIT = 256
};

enum { 
    Id = 128, Fun, Num,
    KB, Int, Return, KE // Kb is Keywords begin mark, Ke is Keywords end mark
};
char *KEYWORDS = "int return";
char *KEYWORD_MAIN = "main";

void next();

void dispose() {
    if (src)
        free(src), src = NULL;
    if (text)
        free(text), text = NULL;
}

void fail(char *tip) {
    dispose();
    printf("error: %s\n", tip);
    exit(-1);
}


void init_malloc() {
    src = (char *) malloc(POOL_SIZE);
    if (!src) {
        fail("malloc src failed");
    }
    psrc = src;
    line = 1;

    text = (int *) malloc(POOL_SIZE);
    if (!text) {
        fail("malloc text failed");
    }
    pc = text;

    stack = (int *) malloc(POOL_SIZE);
    if (!stack) {
        fail("malloc stack failed");
    }
    sp = stack;

    symbols = (struct identifier *) malloc(POOL_SIZE);
    if (!symbols) {
        fail("malloc symbols failed");
    }
    psymbols = symbols;
    memset(symbols, 0, POOL_SIZE);


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
            return;
        } else if (token >= '1' && token <= '9') { // number
            // only support dec for the time being
            token_val = token - '0';
            while (*psrc >= '0' && *psrc <= '9') { // 0-9
                token_val = token_val*10  +  *psrc++ - '0';
            }
            token = Num; // mark Num
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
    if (token == Return) {
        match(Return);
        *++pc = EXIT;
        if (token != ';') {
            *++sp = token_val;
        } 
        match(token);
        match(';');
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
        psymbols->value = (size_t) pc;
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
    while (1) {
        int op = *pc++;
        if (op == EXIT) { printf("exit(%d)\n", ax = *sp--); return; }
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
