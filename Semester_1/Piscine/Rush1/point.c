/*
** EPITECH PROJECT, 2026
** Paradigms Seminar
** File description:
** Exercice 02
*/

#include <stdio.h>
#include "G-PDG-300_rush1/vertex.h"
#include "G-PDG-300_rush1/new.h"


typedef struct {
    Class base;
    int x;
    int y;
} PointClass;

static const PointClass _description;

static void point_constructor(PointClass *this, va_list *args)
{
    this->x = va_arg(*args, int);
    this->y = va_arg(*args, int);
}

static void point_destructor(PointClass *this)
{
    (void)this;
}

static char *point_string(PointClass *this)
{
    const int len = snprintf(NULL, 0, "<%s (%d, %d)>",
        this->base.__name__, this->x, this->y);
    char *str = malloc(len + 1);

    if (str != NULL) {
        snprintf(str, len + 1, "<%s (%d, %d)>",
            this->base.__name__, this->x, this->y);
        return str;
    }
    raise("Out of memory");
}

static Object *point_addition(PointClass *this, const PointClass *to_add)
{
    PointClass *result = malloc(sizeof(PointClass));

    memcpy(result, &_description, sizeof(_description));
    result->x = this->x + to_add->x;
    result->y = this->y + to_add->y;
    return ((Object *)result);
}

static Object *point_substraction(PointClass *this, const PointClass *to_sub)
{
    PointClass *result = malloc(sizeof(PointClass));

    memcpy(result, &_description, sizeof(_description));
    result->x = this->x - to_sub->x;
    result->y = this->y - to_sub->y;
    return ((Object *)result);
}


static const PointClass _description = {
    {   /* Class struct */
        .__size__ = sizeof(_description),
        .__name__ = "Point",
        .__ctor__ = (ctor_t)&point_constructor,
        .__dtor__ = (dtor_t)&point_destructor,
        .__str__ = (to_string_t)&point_string,
        .__add__ = (binary_operator_t)&point_addition,
        .__sub__ = (binary_operator_t)&point_substraction,
        .__mul__ = NULL,
        .__div__ = NULL,
        .__eq__ = NULL,
        .__gt__ = NULL,
        .__lt__ = NULL
    },
    .x = 0,
    .y = 0
};

const Class *Point = (const Class *)&_description;
