// http://lotabout.me/2016/write-a-C-interpreter-4/

#include <stdio.h>
#include <stdlib.h>

enum { Num };

int token;
int token_val;
char *src;

void next() {
    while (*src == ' ') {
        src++;
    }
    token = *src++;
    if (token >= '0' && token <= '9') {
        token_val = token - '0';
        token = Num;
        while (*src >= '0' && *src <= '9') {
            token_val = token_val*10 + *src-'0';
            src++;
        }
    }
}

void match(int tk) {
    if (token != tk) {
        printf("error, token != tk, %c, %c\n", token, tk);
        exit(-1);
    }
    next();
}

int expr();

int factor() {
    int v;
    if (token == '(') {
        match('(');
        v = expr();
        match(')');
    } else {
        v = token_val;
        match(Num);
    }
    return v;
}

int term_tail(int v) {
    if (token == '*') {
        match('*');
        return term_tail(v * factor());
    } else if (token == '/') {
        match('/');
        return term_tail(v / factor());
    } else {
        return v;
    }
}

int term() {
    return term_tail(factor());
}

int expr_tail(int v) {
    if (token == '+') {
        match('+');
        return expr_tail(v + term());
    } else if (token == '-') {
        match('-');
        return expr_tail(v - term());
    } else {
        return v;
    }
}

int expr() {
    return expr_tail(term());
}


int main() {
    src = "(1+2)*(1+3)";
    next();
    printf("%d\n", expr());


    return 0;
}



// vim: et:ts=4:sw=4:
