// Duru Melek Doğru 260ADB145
// https://github.com/Jinx2848/calc-lab-home
// Compile with: gcc -O2 -Wall -Wextra -std=c17 -o calc calc.c -lm
// Resources used: Class Slides, w3schools, geeksforgeeks- first two for general information, last one for info on parsers
// chatGPT was used once to provide a starting point/create a plan on how to complete the project. No code has been directly taken.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

// defining the parser here!!

typedef struct {
    const char *input;
    size_t pos;
    size_t error_pos;
    int has_error;
} Parser;
// ok so let's separate functions. main, evaluating the file to spot errors, calculating, record errors
double parse_expression(Parser *p);
double parse_term(Parser *p);
double parse_primary (Parser *p);

// this func ensures unnecessary whitespace is skipped
void skip_spaces(Parser *p){
    while (isspace((unsigned char)p->input[p->pos])) {
        p->pos++;
    }
}

// this func reads the expression
double parse_primary(Parser *p){
    skip_spaces(p);
    //oh boy ok here we go. we take care of where the token begins
    size_t start = p->pos;

    // and now we start handling the input. we start by parantheses bc order of operation
    // i think they call it pemdas in english?
    if (p->input[p->pos] == '(') {
        p->pos++;
        double value = parse_expression(p);
        if (p->has_error){
            return 0.0;
        }
        skip_spaces(p);

        if(p->input[p->pos] != ")"){
            p->has_error = 1;
            p->error_pos = p->pos;
            return 0.0;
        }

        p->pos++;
        return value;

    // now we recognize the numebr
    if(isdigit((unsigned char)p->input[p->pos]) || p->input[p->pos] == "."){
        char *end;
        // learned strtod from geeksforgeeks
        double value = strtod(&p->input[p->pos], &end);

        if (end == &p->input[p->pos]){
        p->has_error = 1;
        p->error_pos = start;
        return 0.0;
        }

        p->pos += (size_t)(end - &p->input[p->pos]); //it took me three tries to write size_t.
        if (!isinfinite(value)){
            p->has_error = 1;
            p->error_pos = start;
            return 0.0;   
        }
        return value;
    }
    p->has_error = 1;
    p->error_pos = start;
    return 0.0;
}

// used chatgpt here because it gave and error and my very tired mind could not spot it. turns out it was a typo defining the parser.

// now we actually start doing stuff but let me commit here hold on