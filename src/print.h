#ifndef PRINT_H
#define PRINT_H

#include "structs.h"
#include <stdio.h>
#include <stdlib.h>

void print_string(JsonString *, const char*);
void print_value(JsonValue *, int);
void print_object(JsonObject *, int);
void print_array(JsonArray *, int);

void _indent(int indent) {
    printf("%.*s", indent * 2, "                                                  ");
}
void print_string(JsonString *v, const char *prefix) {
    printf("%s \"%s\"", prefix, v->value);
}
void print_value(JsonValue *v, int indent) {
    switch (v->type) {
        case T_Object:
            print_object(v->value, indent);
            break;
        case T_Array:
            print_array(v->value, indent);
            break;
        case T_String:
            print_string(v->value, "STRING");
            break;
        case T_Number:
            printf("NUMBER \"%i\"", ((JsonNumber *)(v->value))->value);
            break;
        case T_Null:
            printf("NULL");
            break;
        case T_Boolean:
            if (((JsonBoolean *)(v->value))->value) {
                printf("TRUE");
            }
            else {
                printf("FALSE");
            }
            break;
        default:
            fprintf(stderr, "Bad Type\n");
            exit(1);
            break;
    }
}
void print_object(JsonObject *v, int indent) {
    printf("OBJECT {\n");
    for (int i = 0; i < v->count; i++) {
        JsonProperty *p = v->values[i];
        _indent(indent + 1);
        print_string(p->name, "PROP");
        printf(" : ");
        print_value(p->value, indent + 1);
        printf("\n");
    }
    _indent(indent);
    printf("}");
}
void print_array(JsonArray *v, int indent) {
    printf("ARRAY [\n");
    for (int i = 0; i < v->count; i++) {
        _indent(indent + 1);
        printf("[%i] : ", i);
        print_value(v->values[i], indent + 1);
        printf("\n");
    }
    _indent(indent);
    printf("]");
}
#endif