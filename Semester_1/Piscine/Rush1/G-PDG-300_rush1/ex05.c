/*
** EPITECH PROJECT, 2026
** Paradigms Seminar
** File description:
** Exercice 05
*/

#include <stdio.h>
#include <stdlib.h>
#include "array.h"
#include "int.h"
#include "new.h"

int         main(void)
{
    Object  *array = new(Array, 10, Int, 0);
    Object  *it = begin(array);
    Object  *it_end = end(array);
    char    *str_val;

    printf("array size: %zu\n", len(array));
    setitem(array, 5, 12);
    setitem(array, 6, 13);
    while (lt(it, it_end))
    {
        str_val = str(getval(it));
        printf("%s\n", str_val);
        free(str_val);
        incr(it);
    }
    delete(it);
    delete(it_end);
    delete(array);
    return (0);
}

