#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>

typedef enum {
    T_Object,
    T_Array,
    T_String,
    T_Number,
    T_Null,
    T_Boolean,
} ValueType;

typedef struct {
    ValueType type;
    void     *value;
} JsonValue;

typedef struct {
    char *value;
} JsonString;

typedef struct {
    long value;
} JsonNumber;

typedef struct {
    bool value;
} JsonBoolean;

typedef struct {
    JsonString *name;
    JsonValue  *value;
} JsonProperty;

typedef struct {
    int            count;
    JsonProperty **values;
} JsonObject;

typedef struct {
    int         count;
    JsonValue **values;
} JsonArray;
#endif