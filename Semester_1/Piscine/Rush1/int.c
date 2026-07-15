/*
** EPITECH PROJECT, 2026
** Paradigms Seminar
** File description:
** Int class implementation
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "G-PDG-300_rush1/int.h"

typedef struct {
    Class base;
    int value;
} int_class_t;

static const int_class_t _description;

static void int_constructor(int_class_t *this, va_list *args)
{
    this->value = va_arg(*args, int);
}

static void int_destructor(int_class_t *this)
{
    (void)this;
}

static char *int_string(int_class_t *this)
{
    int size = snprintf(NULL, 0, "<Int (%d)>", this->value);
    char *str = malloc(size + 1);

    sprintf(str, "<Int (%d)>", this->value);
    return str;
}

static Object *int_addition(int_class_t *this, const int_class_t *other)
{
    int_class_t *result = malloc(sizeof(int_class_t));

    memcpy(result, &_description, sizeof(_description));
    result->value = this->value + other->value;
    return ((Object *)result);
}

static Object *int_substraction(int_class_t *this, const int_class_t *other)
{
    int_class_t *result = malloc(sizeof(int_class_t));

    memcpy(result, &_description, sizeof(_description));
    result->value = this->value - other->value;
    return ((Object *)result);
}

static Object *int_mul(int_class_t *this, const int_class_t *other)
{
    int_class_t *result = malloc(sizeof(int_class_t));

    memcpy(result, &_description, sizeof(_description));
    result->value = this->value * other->value;
    return ((Object *)result);
}

static Object *int_div(int_class_t *this, const int_class_t *other)
{
    int_class_t *result = malloc(sizeof(int_class_t));

    memcpy(result, &_description, sizeof(_description));
    if (other->value != 0)
        result->value = this->value / other->value;
    else
        result->value = 0;
    return ((Object *)result);
}

static bool int_equal_to(const int_class_t *this, const int_class_t *other)
{
    return (this->value == other->value);
}

static bool int_greater_than(const int_class_t *this, const int_class_t *other)
{
    return (this->value > other->value);
}

static bool int_lower_than(const int_class_t *this, const int_class_t *other)
{
    return (this->value < other->value);
}

static const int_class_t _description = {
    {   /* Class struct */
        .__size__ = sizeof(_description),
        .__name__ = "Int",
        .__ctor__ = (ctor_t)&int_constructor,
        .__dtor__ = (dtor_t)&int_destructor,
        .__str__ = (to_string_t)&int_string,
        .__add__ = (binary_operator_t)&int_addition,
        .__sub__ = (binary_operator_t)&int_substraction,
        .__mul__ = (binary_operator_t)&int_mul,
        .__div__ = (binary_operator_t)&int_div,
        .__eq__ = (binary_comparator_t)&int_equal_to,
        .__gt__ = (binary_comparator_t)&int_greater_than,
        .__lt__ = (binary_comparator_t)&int_lower_than
    },
    .value = 0
};

const Class *Int = (const Class *)&_description;
