/*
** EPITECH PROJECT, 2026
** Paradigms Seminar
** File description:
** Char class implementation
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "G-PDG-300_rush1/char.h"

typedef struct {
    Class base;
    char value;
} char_class_t;

static const char_class_t _description;

static void char_constructor(char_class_t *this, va_list *args)
{
    this->value = (char)va_arg(*args, int);
}

static void char_destructor(char_class_t *this)
{
    (void)this;
}

static char *char_str(char_class_t *this)
{
    int size = snprintf(NULL, 0, "<Char (%c)>", this->value);
    char *str = malloc(size + 1);

    sprintf(str, "<Char (%c)>", this->value);
    return str;
}

static Object *char_add(char_class_t *this, const char_class_t *other)
{
    char_class_t *result = malloc(sizeof(char_class_t));

    memcpy(result, &_description, sizeof(_description));
    result->value = this->value + other->value;
    return ((Object *)result);
}

static Object *char_sub(char_class_t *this, const char_class_t *other)
{
    char_class_t *result = malloc(sizeof(char_class_t));

    memcpy(result, &_description, sizeof(_description));
    result->value = this->value - other->value;
    return ((Object *)result);
}

static Object *char_mul(char_class_t *this, const char_class_t *other)
{
    char_class_t *result = malloc(sizeof(char_class_t));

    memcpy(result, &_description, sizeof(_description));
    result->value = this->value * other->value;
    return ((Object *)result);
}

static Object *char_div(char_class_t *this, const char_class_t *other)
{
    char_class_t *result = malloc(sizeof(char_class_t));

    memcpy(result, &_description, sizeof(_description));
    if (other->value != 0)
        result->value = this->value / other->value;
    else
        result->value = 0;
    return ((Object *)result);
}

static bool char_equal_to(const char_class_t *this, const char_class_t *other)
{
    return (this->value == other->value);
}

static bool char_greater_than
(const char_class_t *this, const char_class_t *other)
{
    return (this->value > other->value);
}

static bool char_lower_than
(const char_class_t *this, const char_class_t *other)
{
    return (this->value < other->value);
}

static const char_class_t _description = {
    {   /* Class struct */
        .__size__ = sizeof(_description),
        .__name__ = "Char",
        .__ctor__ = (ctor_t)&char_constructor,
        .__dtor__ = (dtor_t)&char_destructor,
        .__str__ = (to_string_t)&char_str,
        .__add__ = (binary_operator_t)&char_add,
        .__sub__ = (binary_operator_t)&char_sub,
        .__mul__ = (binary_operator_t)&char_mul,
        .__div__ = (binary_operator_t)&char_div,
        .__eq__ = (binary_comparator_t)&char_equal_to,
        .__gt__ = (binary_comparator_t)&char_greater_than,
        .__lt__ = (binary_comparator_t)&char_lower_than
    },
    .value = 0
};

const Class *Char = (const Class *)&_description;
