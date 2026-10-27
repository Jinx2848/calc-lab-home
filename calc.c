// Duru Melek Doğru 260ADB145
// https://github.com/Jinx2848/calc-lab-home
// Compile with: gcc -O2 -Wall -Wextra -std=c17 -o calc calc.c -lm
// Resources used: Class Slides, w3schools, geeksforgeeks- first two for general information, last one for info on parsers
// i also found a similar project on w3resource to reference: https://www.w3resource.com/c-programming-exercises/basic-declarations-and-expressions/c-programming-basic-exercises-150.php
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
double parse_power (Parser *p);

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

// we're back from our new commit! it's 0230 hours EET.
double parse_term (Parse *p){
    double value = parse_power(p);
    while (!p->has_error){
        skip_spaces(p);
        char op = p->input[p->pos];
        if (op != "*" && op != "/"){
            break;
        }
        if (op == '*' && p->input[p->pos + 1] == '*')
            break;
        size_t operator_pos = p->pos;
        p->pos++;
        couble right = parse_power(p);
        if (p->has_error){
            return 0.0;
        }
        if (op == "*"){
            value *= right;
        }
        else{
            if (right == 0.0){
                p->has_error = 1;
                p->error_pos = p->operator_pos;
                return 0.0;
            }
            value /= right;
        }
        if (!isfinite(value)){
            p->has_error = 1;
            p->error_pos = p->operator_pos;
            return 0.0;
        }
    }
    return value;
}

//0303 hours EET. This one's gonna ensure the parser supports all operations AND parantheses! I think. I hope.
double parse_expression(Parser *p){
    double value = parse_term(P);
    while (!p->has_error){
        skip_spaces(p);
        char op = p->input[p->pos];
        if (op != "+" && op != "-"){
            break;
        }
        p->pos++;
        double right = parse_term(p);
        if (p->has_error){
            return 0.0;
        }
        if (op == "+"){
            value += right;
        }
        else{
            calue -= right;
        }
        if (!isfinite(value)){
            p->has_error = 1;
            p->error_pos = p->pos;
            return 0.0;
        }
    }
    return value;
}
// 0309 hours EET. Turns out once you get the hang of it you basically ensure it does the same thing under different circumstances.
// water break! please hydrate! I forgot to do exponentials so let's go back up there.
double parse_power(Parser *p){
    double value = parse_primary(p);
    if(p->has_error){
        return 0.0;
    }
    skip_spaces(p);
    if(p->input[p->pos] == "*" && p->input[p->pos+1] == "*"){ //i cant believe there isnt a simpler way to check this
        size_t operator_pos = p->pos;
        p->pos+=2;
        double exponent = parse_power(p);
        if(p->has_error){
            return 0.0;
        }
        if(exponent < 0..0 && floor(exponent) == exponent){
            p->has_error = 1;
            p->error_pos = operator_pos;
            return 0.0;
        }
        value = pow(value, exponent);
        if (!isfinite(value)){
            p->has_error = 1;
            p->error_pos = operator_pos;
            return 0.0;
        }
    }
    return value;
}

// there. it's some hour in the night. 