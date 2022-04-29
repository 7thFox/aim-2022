#include "print.h"
#include "read.h"
#include "structs.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

JsonValue   *parse_start();
JsonValue   *parse_value();
JsonObject  *parse_object();
JsonArray   *parse_array();
JsonNumber  *parse_number();
JsonString  *parse_string();
JsonBoolean *parse_true();
JsonBoolean *parse_false();
void         parse_null();

void *verify(void *ptr) {
    if (!ptr) {
        fprintf(stderr, "malloc/realloc failed\n");
        exit(1);
    }
    return ptr;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Expected file name parameter\n");
        return 1;
    }

    file = fopen(argv[1], "r");
    if (!file) {
        fprintf(stderr, "Failed to open file '%s'\n", argv[0]);
        return 1;
    }

    JsonValue *root = parse_start();
    print_value(root, 0);

    fclose(file);
}

JsonValue *parse_start() {
    JsonValue *root = parse_value();

    if (peek() != EOF) {
        fprintf(stderr, "ERROR(%i,%i): Expected EOF, got %c\n", disp_line, disp_pos, peek());
        exit(1);
    }
    return root;
}

JsonValue *parse_value() {
    JsonValue *value = verify(malloc(sizeof(JsonValue)));

    switch (peek()) {
        case '{':
            value->type  = T_Object;
            value->value = parse_object();
            break;
        case '[':
            value->type  = T_Array;
            value->value = parse_array();
            break;
        case '"':
            value->type  = T_String;
            value->value = parse_string();
            break;
        case 't':
            value->type  = T_Boolean;
            value->value = parse_true();
            break;
        case 'f':
            value->type  = T_Boolean;
            value->value = parse_false();
            break;
        case 'n':
            value->type  = T_Null;
            value->value = NULL;
            parse_null();
            break;
        default:
            if (peek() >= '0' && peek() <= '9') {
                value->type  = T_Number;
                value->value = parse_number();
            }
            else {
                fprintf(stderr, "ERROR(%i,%i): Unexpected char %c\n", disp_line, disp_pos, peek());
                exit(1);
            }
            break;
    }
    return value;
}

JsonObject *parse_object() {
    assert_char('{');

    size_t      cap = 16;
    JsonObject *v   = verify(malloc(sizeof(JsonObject)));
    v->count        = 0;

    skip_whitespace();
    if (peek() == '}') {
        next();
        v->values = NULL;
        skip_whitespace();
        return v;
    }

    v->values = verify(malloc(sizeof(JsonProperty *) * cap));

    JsonProperty *firstProp = verify(malloc(sizeof(JsonProperty)));
    firstProp->name         = parse_string();
    assert_char(':');
    skip_whitespace();
    firstProp->value = parse_value();

    v->values[v->count] = firstProp;
    v->count++;

    while (peek() == ',') {
        next();
        skip_whitespace();
        if (v->count >= cap) {
            cap *= 2;
            v->values = verify(realloc(v->values, sizeof(JsonProperty *) * cap));
        }

        JsonProperty *prop = verify(malloc(sizeof(JsonProperty)));
        prop->name         = parse_string();
        assert_char(':');
        skip_whitespace();
        prop->value = parse_value();

        v->values[v->count] = prop;
        v->count++;
    }

    assert_char('}');

    v->values = verify(realloc(v->values, sizeof(JsonProperty *) * v->count)); // free extra allocated space

    skip_whitespace();
    return v;
}

JsonArray *parse_array() {
    assert_char('[');

    size_t     cap = 16;
    JsonArray *v   = verify(malloc(sizeof(JsonArray)));
    v->count       = 0;

    skip_whitespace();
    if (peek() == ']') {
        next();
        v->values = NULL;
        skip_whitespace();
        return v;
    }

    v->values = verify(malloc(sizeof(JsonValue *) * cap));

    v->values[v->count] = parse_value();
    v->count++;

    while (peek() == ',') {
        next();
        skip_whitespace();

        if (v->count >= cap) {
            cap *= 2;
            v->values = verify(realloc(v->values, sizeof(JsonValue *) * cap));
        }
        v->values[v->count] = parse_value();
        v->count++;
    }

    assert_char(']');

    v->values = verify(realloc(v->values, sizeof(JsonValue *) * v->count)); // free extra allocated space

    skip_whitespace();
    return v;
}

JsonNumber *parse_number() {
    JsonNumber *v = verify(malloc(sizeof(JsonNumber)));
    v->value      = 0;

    bool negative = false;
    if (peek() == '-') {
        next();
        negative = true;
    }

    // no leading zeros per spec
    if (peek() == '0') {
        next();
        skip_whitespace();
        return v;
    }

    while (peek() >= '0' && peek() <= '9') {
        v->value *= 10;
        v->value += (next() - '0');
    }

    if (negative) {
        v->value *= -1;
    }

    if (v->value == 0) {
        fprintf(stderr, "ERROR(%i,%i): Expected digit, got %c\n", disp_line, disp_pos, peek()); // zero value is returned early
        exit(1);
    }

    skip_whitespace();

    return v;
}

JsonString *parse_string() {
    size_t      str_cap  = 128;
    size_t      str_size = 0;
    JsonString *v        = verify(malloc(sizeof(JsonString)));
    v->value             = verify(malloc(sizeof(char) * str_cap));

    assert_char('"');

    while (next() != '"') {
        v->value[str_size] = prev();
        str_size++;
        if (str_size >= str_cap) {
            str_cap *= 2;
            v->value = verify(realloc(v->value, sizeof(char) * str_cap));
        }
    }

    skip_whitespace();

    v->value[str_size] = '\0';
    v->value           = verify(realloc(v->value, sizeof(char) * (str_size + 1))); // free extra allocation
    return v;
}

JsonBoolean *parse_true() {
    assert_char('t');
    assert_char('r');
    assert_char('u');
    assert_char('e');

    skip_whitespace();

    JsonBoolean *v = verify(malloc(sizeof(JsonBoolean)));
    v->value       = true;
    return v;
}

JsonBoolean *parse_false() {
    assert_char('f');
    assert_char('a');
    assert_char('l');
    assert_char('s');
    assert_char('e');

    skip_whitespace();

    JsonBoolean *v = verify(malloc(sizeof(JsonBoolean)));
    v->value       = false;
    return v;
}

void parse_null() {
    assert_char('n');
    assert_char('u');
    assert_char('l');
    assert_char('l');

    skip_whitespace();
}