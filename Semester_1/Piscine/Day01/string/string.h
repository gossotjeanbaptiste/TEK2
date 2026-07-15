/*
** EPITECH PROJECT, 2026
** Day01
** File description:
** string
*/

#ifndef INCLUDED_STRING_H
    #define INCLUDED_STRING_H
    #include <stdbool.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
typedef struct string_s string_t;

typedef struct string_s {
    char *string;
    void (*string_destroy)(string_t *this);
    bool (*empty)(const string_t *this);
    size_t (*length)(const string_t *this);
    char (*at)(const string_t *this, size_t pos);
    void (*print)(const string_t *this);
    const char *(*data)(const string_t *this);
    void (*clear)(string_t *this);
    void (*assign)(string_t *this, const char *str);
    void (*append)(string_t *this, const char *str);
} string_t;

size_t strlen(const char *s);
char *strcpy(char *restrict dst, const char *restrict src);

void string_init(string_t *this, const char *s);
void string_destroy(string_t *this);
bool empty(const string_t *this);
size_t length(const string_t *this);
char at_position(const string_t *this, size_t pos);
void print(const string_t *this);
const char *data(const string_t *this);
void clear(string_t *this);
void assign(string_t *this, const char *str);
void append(string_t *this, const char *str);

#endif
