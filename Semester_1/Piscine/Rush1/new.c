/*
** EPITECH PROJECT, 2026
** Paradigms Seminar
** File description:
** new and delete implementations
*/

#include <stdlib.h>
#include <string.h>
#include "G-PDG-300_rush1/new.h"

Object *va_new(const Class *class, va_list *ap)
{
    Object *ptr;

    if (class == NULL)
        return (NULL);
    ptr = malloc(class->__size__);
    if (ptr == NULL)
        return (NULL);
    memcpy(ptr, (const void *)class, class->__size__);
    if (class->__ctor__ != NULL)
        class->__ctor__(ptr, ap);
    return (ptr);
}

Object *new(const Class *class, ...)
{
    va_list args;
    Object *ptr;

    va_start(args, class);
    ptr = va_new(class, &args);
    va_end(args);
    return (ptr);
}

void delete(Object *ptr)
{
    Class *class;
    if (ptr == NULL)
        return;
    class = (Class *)ptr;
    if (class->__dtor__ != NULL)
        class->__dtor__(ptr);
    free(ptr);
}
