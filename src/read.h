#ifndef READ_H
#define READ_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_CAP 4096
FILE  *file;
char   buffer[BUFFER_CAP];
size_t buff_pos;
size_t buff_size;
char   prev_char;

long disp_line = 1;
long disp_pos  = 0;

void _nextBuff() {
    if (feof(file)) {
        fprintf(stderr, "ERROR(%i,%i): Unexpected EOF\n", disp_line, disp_pos);
        exit(1);
    }

    buff_size = fread(buffer, sizeof(char), BUFFER_CAP, file);
    if (ferror(file) != 0) {
        fprintf(stderr, "Error reading file\n");
        exit(1);
    }
    buff_pos = 0;
}

char next() {
    if (buff_pos >= buff_size) {
        _nextBuff();
    }
    prev_char = buffer[buff_pos];
    buff_pos++;

    if (prev_char == '\n') {
        disp_line++;
        disp_pos = 0;
    }
    else {
        disp_pos++;
    }

    return prev_char;
}

char peek() {
    if (buff_pos >= buff_size) {
        if (feof(file)) {
            return EOF;
        }
        _nextBuff();
    }
    return buffer[buff_pos];
}

char prev() {
    return prev_char;
}

void skip_whitespace() {
    for (char c = peek(); c == ' ' || c == '\t' || c == '\r' || c == '\n'; c = peek()) {
        next();
    }
}

void assert_char(char expected) {
    if (next() != expected) {
        fprintf(stderr, "ERROR(%i,%i): Expected %c, got %c\n", disp_line, disp_pos, expected, prev());
        exit(1);
    }
}
#endif