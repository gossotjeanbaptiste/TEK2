/*
** EPITECH PROJECT, 2026
** Rush1
** File description:
** vertex
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "G-PDG-300_rush1/vertex.h"

typedef struct {
    Class base;
    int x;
    int y;
    int z;
} vertex_class_t;

static const vertex_class_t _description;

static void vertex_constructor(vertex_class_t *this, va_list *args)
{
    this->x = va_arg(*args, int);
    this->y = va_arg(*args, int);
    this->z = va_arg(*args, int);
}

static void vertex_destructor(vertex_class_t *this)
{
    (void)this;
}

static char *vertex_string(vertex_class_t *this)
{
    int size = snprintf(NULL, 0, "<Vertex (%d, %d, %d)>",
        this->x, this->y, this->z);
    char *str = malloc(size + 1);

    sprintf(str, "<Vertex (%d, %d, %d)>", this->x, this->y, this->z);
    return str;
}

static Object *vertex_add(vertex_class_t *this, const vertex_class_t *other)
{
    vertex_class_t *result = malloc(sizeof(vertex_class_t));

    memcpy(result, &_description, sizeof(_description));
    result->x = this->x + other->x;
    result->y = this->y + other->y;
    result->z = this->z + other->z;
    return ((Object *)result);
}

static Object *vertex_sub(vertex_class_t *this, const vertex_class_t *other)
{
    vertex_class_t *result = malloc(sizeof(vertex_class_t));

    memcpy(result, &_description, sizeof(_description));
    result->x = this->x - other->x;
    result->y = this->y - other->y;
    result->z = this->z - other->z;
    return ((Object *)result);
}

static const vertex_class_t _description = {
    {   /* Class struct */
        .__size__ = sizeof(_description),
        .__name__ = "Vertex",
        .__ctor__ = (ctor_t)&vertex_constructor,
        .__dtor__ = (dtor_t)&vertex_destructor,
        .__str__ = (to_string_t)&vertex_string,
        .__add__ = (binary_operator_t)&vertex_add,
        .__sub__ = (binary_operator_t)&vertex_sub,
        .__mul__ = NULL,
        .__div__ = NULL,
        .__eq__ = NULL,
        .__gt__ = NULL,
        .__lt__ = NULL
    },
    .x = 0,
    .y = 0,
    .z = 0
};

const Class *Vertex = (const Class *)&_description;
