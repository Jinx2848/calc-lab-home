// Duru Melek Doğru 260ADB145
// https://github.com/Jinx2848/calc-lab-home
// Compile with: gcc -O2 -Wall -Wextra -std=c17 -o calc calc.c -lm
// Resources used: Class Slides, w3schools, geeksforgeeks- first two for general information, last one for info on parsers
// i also found a similar project on w3resource to reference: https://www.w3resource.com/c-programming-exercises/basic-declarations-and-expressions/c-programming-basic-exercises-150.php
// chatGPT was used to provide a starting point/create a plan on how to complete the project. No code has been directly taken.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

// defining the parser here!!

typedef struct {
    const char *input;
    size_t pos;
    size_t error_pos;
    int has_error;
    size_t operator_pos;
} Parser;

// right i forgot to check if it's a FLOAT.
typedef struct{
    double value;
    int is_float;
} EvalValue;
// ok so let's separate functions. main, evaluating the file to spot errors, calculating, record errors
EvalValue parse_expression(Parser *p);
EvalValue parse_term(Parser *p);
EvalValue parse_primary (Parser *p);
EvalValue parse_power (Parser *p);

// this func ensures unnecessary whitespace is skipped
void skip_spaces(Parser *p){
    while (isspace((unsigned char)p->input[p->pos])) {
        p->pos++;
    }
}

// this func reads the expression
EvalValue parse_primary(Parser *p){
    skip_spaces(p);
    //oh boy ok here we go. we take care of where the token begins
    size_t start = p->pos;

    // and now we start handling the input. we start by parantheses bc order of operation
    // i think they call it pemdas in english?
    if (p->input[p->pos] == '(') {
        p->pos++;
        EvalValue value = parse_expression(p);
        if (p->has_error){
            return (EvalValue){0.0, 0};
        }
        skip_spaces(p);

        if(p->input[p->pos] != ')'){
            p->has_error = 1;
            p->error_pos = p->pos;
            return (EvalValue){0.0, 0};
        }

        p->pos++;
        return value;
    }
    // now we recognize the numebr
    // fix: have to require that the number requires digfits before decimal point
    int is_float = 0;
    if(isdigit((unsigned char)p->input[p->pos])){
        size_t i = start;

        while(isdigit((unsigned char)p->input[i])){
            i++;
        }
        // fix: require digits after dp too
        if(p->input[i] == '.'){
            is_float = 1;
            if(!isdigit((unsigned char)p->input[i+1])){
                p->has_error = 1;
                p->error_pos = i;
                return (EvalValue){0.0, 0};
            }
            i++;
            while(isdigit((unsigned char)p->input[i])){
                i++;
            }
        }
        char *end;
        // learned strtod from geeksforgeeks
        double value = strtod(&p->input[p->pos], &end);

        if (end != &p->input[i]){
        p->has_error = 1;
        p->error_pos = start;
        return (EvalValue){0.0, 0};
        }
        p->pos = i;
        if (!isfinite(value)){
            p->has_error = 1;
            p->error_pos = start;
            return (EvalValue){0.0, 0};   
        }
        return (EvalValue){value, is_float};
    }
    p->has_error = 1;
    p->error_pos = start;
    return (EvalValue){0.0, 0};
}
// used chatgpt here because it gave and error and my very tired mind could not spot it. turns out it was a typo defining the parser.

// we're back from our new commit! it's 0230 hours EET.
EvalValue parse_term (Parser *p){
    EvalValue value = parse_power(p);
    while (!p->has_error){
        skip_spaces(p);
        size_t op_pos = p->pos;
        char op = p->input[p->pos];
        if (op != '*' && op != '/'){
            break;
        }
        if (op == '*' && p->input[p->pos + 1] == '*'){
            break;
        }
        p->operator_pos = op_pos;
        p->pos++;
        skip_spaces(p);
        size_t right_start = p->pos;
        EvalValue right = parse_power(p);
        if (p->has_error){
            return (EvalValue){0.0, 0};
        }
        if (op == '*'){
            value.value *= right.value;
            value.is_float = value.is_float || right.is_float;
        }
        else{
            if (right.value == 0.0){
                p->has_error = 1;
                p->error_pos = right_start;
                return (EvalValue){0.0, 0};
            }
            if (!value.is_float && !right.is_float) {
                value.value = trunc(value.value / right.value);
                value.is_float = 0;
            } else {
                value.value /= right.value;
                value.is_float = 1;
            }
        }
        if (!isfinite(value.value)){
            p->has_error = 1;
            p->error_pos = op_pos;
            return (EvalValue){0.0, 0};
        }
    }
    return value;
}

//0303 hours EET. This one's gonna ensure the parser supports all operations AND parantheses! I think. I hope.
EvalValue parse_expression(Parser *p){
    EvalValue value = parse_term(p);
    while (!p->has_error){
        skip_spaces(p);
        size_t op_pos = p->pos;
        char op = p->input[p->pos];
        if (op != '+' && op != '-'){
            break;
        }
        p->operator_pos = op_pos;
        p->pos++;
        EvalValue right = parse_term(p);
        if (p->has_error){
            return (EvalValue){0.0, 0};
        }
        if (op == '+'){
            value.value += right.value;
        }
        else{
            value.value -= right.value;
        }
        value.is_float = value.is_float || right.is_float;
        if (!isfinite(value.value)){
            p->has_error = 1;
            p->error_pos = op_pos;
            return (EvalValue){0.0, 0};
        }
    }
    return value;
}
// 0309 hours EET. Turns out once you get the hang of it you basically ensure it does the same thing under different circumstances.
// water break! please hydrate! I forgot to do exponentials so let's go back up there.
EvalValue parse_power(Parser *p){
    EvalValue value = parse_primary(p);
    if(p->has_error){
        return (EvalValue){0.0, 0};
    }
    skip_spaces(p);
    if(p->input[p->pos] == '*' && p->input[p->pos+1] == '*'){ //i cant believe there isnt a simpler way to check this
        size_t op_pos = p->pos;
        p->operator_pos = op_pos;
        p->pos+=2;
        skip_spaces(p);
        size_t exponent_start = p->pos;
        EvalValue exponent = parse_power(p);
        if(p->has_error){
            return (EvalValue){0.0, 0};
        }
        if(exponent.value < 0.0){
            p->has_error = 1;
            p->error_pos = exponent_start;
            return (EvalValue){0.0, 0};
        }
        value.value = pow(value.value, exponent.value);
        value.is_float = value.is_float || exponent.is_float || value.value != trunc(value.value);
        if (!isfinite(value.value)){
            p->has_error = 1;
            p->error_pos = op_pos;
            return (EvalValue){0.0, 0};
        }
    }
    return value;
}

// there. it's some hour in the night. 

// time for file management and main() yay! i'm so ready to be done.

int main(int argc, char *argv[]) {
    char name[] = "durumelek";
    char lastname[] = "dogru";
    char id[] = "260ADB145";

    char *input_dir = NULL, *input_file = NULL, *output_dir = NULL;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--dir")) {
            if (++i >= argc || input_file || input_dir) {
                fprintf(stderr, "Invalid input directory\n");
                return 1;
            }
            input_dir = argv[i];
        } else if (!strcmp(argv[i], "-o") ||
                   !strcmp(argv[i], "--output-dir")) {
            if (++i >= argc || output_dir) {
                fprintf(stderr, "Invalid output directory\n");
                return 1;
            }
            output_dir = argv[i];
        } else if (argv[i][0] != '-') {
            if (input_file || input_dir) {
                fprintf(stderr, "Only one input is allowed\n");
                return 1;
            }
            input_file = argv[i];
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            return 1;
        }
    }

    if ((input_file == NULL) == (input_dir == NULL)) {
        fprintf(stderr, "Usage: calc [-d DIR | input.txt] [-o DIR]\n");
        return 1;
    }
    char default_dir[512];
    if (output_dir == NULL) {
        const char *base = input_file ? input_file : input_dir;
        const char *slash = strrchr(base, '/');
        base = slash ? slash + 1 : base;

        snprintf(default_dir, sizeof(default_dir),
                 "%s_%s_%s", base, name, id);
        output_dir = default_dir;
    }
    struct stat info;
    //this creates output folder if needed
    if (stat(output_dir, &info) != 0) {
        if (errno != ENOENT || mkdir(output_dir, 0777) != 0) {
            perror(output_dir);
            return 1;
        }
    } else if (!S_ISDIR(info.st_mode)) {
        fprintf(stderr, "Output path is not a folder\n");
        return 1;
    }
    DIR *dir = input_dir ? opendir(input_dir) : NULL;
    if (input_dir && dir == NULL) {
        perror(input_dir);
        return 1;
    }
    int status = 0;
    struct dirent *entry;
    while (1) {
        const char *filename = input_file;
        if (input_dir) {
            entry = readdir(dir);
            if (entry == NULL) {
                break;
            }
            filename = entry->d_name;
            size_t n = strlen(filename);

            if (n <= 4 || strcmp(filename + n - 4, ".txt") != 0){
                continue;
            }
        }

        char input_path[1024], output_path[1024], base[512];

        if (input_dir)
            snprintf(input_path, sizeof(input_path), "%s/%s",
                     input_dir, filename);
        else
            snprintf(input_path, sizeof(input_path), "%s", filename);

        const char *slash = strrchr(filename, '/');
        const char *short_name = slash ? slash + 1 : filename;

        snprintf(base, sizeof(base), "%s", short_name);
        size_t len = strlen(base);

        if (len >= 4 && strcmp(base + len - 4, ".txt") == 0)
            base[len - 4] = '\0';

        int path_len = snprintf(output_path, sizeof(output_path),
                        "%s/%s_%s_%s_%s.txt",
                        output_dir, base, name, lastname, id);

if (path_len < 0 || (size_t)path_len >= sizeof(output_path)) {
    fprintf(stderr, "Output path is too long.\n");
    status = 1;
    if (!input_dir) break;
    continue;
}

        FILE *in = fopen(input_path, "rb");

        if (in == NULL) {
            perror(input_path);
            status = 1;
            if (!input_dir) break;
            continue;
        }

        size_t capacity = 4096, length = 0;
        char *text = malloc(capacity + 1);
        int ch;

        if (text == NULL) {
            fclose(in);
            status = 1;
            if (!input_dir) break;
            continue;
        }

        while ((ch = fgetc(in)) != EOF) {
            if (length == capacity) {
                capacity *= 2;
                char *temp = realloc(text, capacity + 1);

                if (temp == NULL) {
                    free(text);
                    text = NULL;
                    status = 1;
                    break;
                }
                text = temp;
            }
            text[length++] = (char)ch;
        }

        if (ferror(in)) status = 1;
        fclose(in);

        if (text == NULL) {
            if (!input_dir) break;
            continue;
        }

        text[length] = '\0';

        size_t start = 0, used = 0, outcap = 4096, error = 0;
        char *result = malloc(outcap);

        if (result == NULL) {
            free(text);
            status = 1;
            if (!input_dir) break;
            continue;
        }
        result[0] = '\0';

        while (start < length && error == 0) {
            size_t end = start;
            while (end < length && text[end] != '\n') end++;

            size_t n = end - start;
            char *line = malloc(n + 1);

            if (line == NULL) {
                status = 1;
                break;
            }

            memcpy(line, text + start, n);
            line[n] = '\0';

            size_t first = 0;
            while (first < n && isspace((unsigned char)line[first]))
                first++;

            if (first < n && line[first] != '#') {
                Parser p = {line, 0, 0, 0, 0};
                EvalValue value = parse_expression(&p);
                skip_spaces(&p);

                if (!p.has_error && line[p.pos] != '\0') {
                    p.has_error = 1;
                    p.error_pos = p.pos;
                }

                if (p.has_error) {
                    error = start + p.error_pos + 1;
                } else {
                    char answer[128];
                    snprintf(answer, sizeof(answer), "%.15g\n", value.value);
                    size_t needed = strlen(answer);

                    if (used + needed + 1 > outcap) {
                        outcap = (used + needed + 1) * 2;
                        char *temp = realloc(result, outcap);

                        if (temp == NULL) {
                            status = 1;
                        } else {
                            result = temp;
                        }
                    }

                    if (!status) {
                        strcpy(result + used, answer);
                        used += needed;
                    }
                }
            }

            free(line);
            start = end + (end < length);
        }

        if (error != 0)
            used = (size_t)snprintf(result, outcap, "ERROR:%zu\n", error);

        FILE *out = status ? NULL : fopen(output_path, "wb");

        if (out == NULL) {
            if (!status) perror(output_path);
            status = 1;
        } else {
            if (used && fwrite(result, 1, used, out) != used)
                status = 1;
            if (fclose(out) != 0)
                status = 1;
        }

        free(text);
        free(result);

        if (!input_dir) break;
    }

    if (dir && closedir(dir) != 0)
        status = 1;

    return status;
}
