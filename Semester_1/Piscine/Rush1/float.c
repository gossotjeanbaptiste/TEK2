/*
** EPITECH PROJECT, 2026
** Paradigms Seminar
** File description:
** Float class implementation
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "G-PDG-300_rush1/float.h"

typedef struct {
    Class base;
    float value;
} float_class_t;

static const float_class_t _description;

static void float_constructor(float_class_t *this, va_list *args)
{
    this->value = (float)va_arg(*args, double);
}

static void float_destructor(float_class_t *this)
{
    (void)this;
}

static char *float_string(float_class_t *this)
{
    int size = snprintf(NULL, 0, "<Float (%f)>", this->value);
    char *str = malloc(size + 1);

    sprintf(str, "<Float (%f)>", this->value);
    return str;
}

static Object *float_addition(float_class_t *this, const float_class_t *other)
{
    float_class_t *result = malloc(sizeof(float_class_t));

    memcpy(result, &_description, sizeof(_description));
    result->value = this->value + other->value;
    return ((Object *)result);
}

static Object *float_substraction
(float_class_t *this, const float_class_t *other)
{
    float_class_t *result = malloc(sizeof(float_class_t));

    memcpy(result, &_description, sizeof(_description));
    result->value = this->value - other->value;
    return ((Object *)result);
}

static Object *float_mul(float_class_t *this, const float_class_t *other)
{
    float_class_t *result = malloc(sizeof(float_class_t));

    memcpy(result, &_description, sizeof(_description));
    result->value = this->value * other->value;
    return ((Object *)result);
}

static Object *float_div(float_class_t *this, const float_class_t *other)
{
    float_class_t *result = malloc(sizeof(float_class_t));

    memcpy(result, &_description, sizeof(_description));
    if (other->value != 0.0f)
        result->value = this->value / other->value;
    else
        result->value = 0.0f;
    return ((Object *)result);
}

static bool float_equal_to
(const float_class_t *this, const float_class_t *other)
{
    return (this->value == other->value);
}

static bool float_greater_than
(const float_class_t *this, const float_class_t *other)
{
    return (this->value > other->value);
}

static bool float_lower_than
(const float_class_t *this, const float_class_t *other)
{
    return (this->value < other->value);
}

static const float_class_t _description = {
    {   /* Class struct */
        .__size__ = sizeof(_description),
        .__name__ = "Float",
        .__ctor__ = (ctor_t)&float_constructor,
        .__dtor__ = (dtor_t)&float_destructor,
        .__str__ = (to_string_t)&float_string,
        .__add__ = (binary_operator_t)&float_addition,
        .__sub__ = (binary_operator_t)&float_substraction,
        .__mul__ = (binary_operator_t)&float_mul,
        .__div__ = (binary_operator_t)&float_div,
        .__eq__ = (binary_comparator_t)&float_equal_to,
        .__gt__ = (binary_comparator_t)&float_greater_than,
        .__lt__ = (binary_comparator_t)&float_lower_than
    },
    .value = 0.0f
};

const Class *Float = (const Class *)&_description;
