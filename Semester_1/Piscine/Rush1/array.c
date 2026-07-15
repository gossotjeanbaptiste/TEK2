/*
** EPITECH PROJECT, 2026
** Paradigms Seminar
** File description:
** Exercice 05
*/

#include <stdlib.h>
#include <stdarg.h>
#include "G-PDG-300_rush1/raise.h"
#include "G-PDG-300_rush1/array.h"
#include "G-PDG-300_rush1/new.h"

typedef struct {
    Container base;
    Class *_type;
    size_t _size;
    Object **_tab;
} array_class_t;

typedef struct {
    Iterator base;
    array_class_t *_array;
    size_t _idx;
} array_iterator_class_t;

static void array_iterator_constructor
(array_iterator_class_t *this, va_list *args)
{
    this->_array = va_arg(*args, array_class_t *);
    this->_idx = va_arg(*args, int);
}

static bool array_iterator_equal
(array_iterator_class_t *this, array_iterator_class_t *to_compare)
{
    return (this->_idx == to_compare->_idx);
}

static bool array_iterator_is_greater
(array_iterator_class_t *this, array_iterator_class_t *to_compare)
{
    return (this->_idx > to_compare->_idx);
}

static bool array_iterator_is_lower
(array_iterator_class_t *this, array_iterator_class_t *to_compare)
{
    return (this->_idx < to_compare->_idx);
}

static void array_iterator_increment(array_iterator_class_t *this)
{
    this->_idx += 1;
}

static Object *array_iterator_getvalue(array_iterator_class_t *this)
{
    if (this->_idx >= this->_array->_size)
        raise("Out of range");
    return (this->_array->_tab[this->_idx]);
}

static void array_iterator_setvalue(array_iterator_class_t *this, ...)
{
    va_list args;

    va_start(args, this);
    if (this->_idx >= this->_array->_size) {
        va_end(args);
        raise("Out of range");
    }
    delete(this->_array->_tab[this->_idx]);
    this->_array->_tab[this->_idx] = va_new(this->_array->_type, &args);
    va_end(args);
}

static const array_iterator_class_t _iteratorDescription = {
    {   /* Iterator struct */
        {   /* Class struct */
            .__size__ = sizeof(_iteratorDescription),
            .__name__ = "ArrayIterator",
            .__ctor__ = (ctor_t)&array_iterator_constructor,
            .__dtor__ = NULL,
            .__str__ = NULL,
            .__add__ = NULL,
            .__sub__ = NULL,
            .__mul__ = NULL,
            .__div__ = NULL,
            .__eq__ = (binary_comparator_t)&array_iterator_equal,
            .__gt__ = (binary_comparator_t)&array_iterator_is_greater,
            .__lt__ = (binary_comparator_t)&array_iterator_is_lower,
        },
        .__incr__ = (incr_t)&array_iterator_increment,
        .__getval__ = (getval_t)&array_iterator_getvalue,
        .__setval__ = (setval_t)&array_iterator_setvalue,
    },
    ._array = NULL,
    ._idx = 0
};
static const Class *ArrayIterator = (const Class *)&_iteratorDescription;

static void array_constructor(array_class_t *this, va_list *args)
{
    size_t i;
    va_list args_copy;

    this->_size = va_arg(*args, size_t);
    this->_type = va_arg(*args, Class *);
    this->_tab = malloc(sizeof(Object *) * this->_size);
    for (i = 0; i < this->_size; i++) {
        va_copy(args_copy, *args);
        this->_tab[i] = va_new(this->_type, &args_copy);
        va_end(args_copy);
    }
}

static void array_destructor(array_class_t *this)
{
    for (unsigned int i = 0; i < this->_size; i++)
        delete(this->_tab[i]);
    free(this->_tab);
}

static size_t array_len(array_class_t *this)
{
    return (this->_size);
}

static Iterator *array_begin(array_class_t *this)
{
    return (new(ArrayIterator, this, 0));
}

static Iterator *array_end(array_class_t *this)
{
    return (new(ArrayIterator, this, this->_size));
}

static Object *array_getitem(array_class_t *this, ...)
{
    va_list args;
    size_t idx;

    va_start(args, this);
    idx = va_arg(args, size_t);
    va_end(args);
    if (idx >= this->_size)
        raise("Index out of range");
    return (this->_tab[idx]);
}

static void array_setitem(array_class_t *this, ...)
{
    va_list args;
    size_t idx;

    va_start(args, this);
    idx = va_arg(args, size_t);
    if (idx >= this->_size) {
        va_end(args);
        raise("Index out of range");
    }
    delete(this->_tab[idx]);
    this->_tab[idx] = va_new(this->_type, &args);
    va_end(args);
}

static const array_class_t _description = {
    {   /* Container struct */
        {   /* Class struct */
            .__size__ = sizeof(_description),
            .__name__ = "Array",
            .__ctor__ = (ctor_t)&array_constructor,
            .__dtor__ = (dtor_t)&array_destructor,
            .__str__ = NULL,
            .__add__ = NULL,
            .__sub__ = NULL,
            .__mul__ = NULL,
            .__div__ = NULL,
            .__eq__ = NULL,
            .__gt__ = NULL,
            .__lt__ = NULL,
        },
        .__len__ = (len_t)&array_len,
        .__begin__ = (iter_t)&array_begin,
        .__end__ = (iter_t)&array_end,
        .__getitem__ = (getitem_t)&array_getitem,
        .__setitem__ = (setitem_t)&array_setitem,
    },
    ._type = NULL,
    ._size = 0,
    ._tab = NULL
};

const Class *Array = (const Class *)&_description;
