/*
** EPITECH PROJECT, 2026
** Paradigms Seminar
** File description:
** Exercice 04
*/

#include <stdio.h>
#include <stdlib.h>
#include "new.h"
#include "int.h"
#include "float.h"
#include "char.h"

void compareAndDivide(Object *a, Object *b)
{
    char    *str_a, *str_b, *str_div;
    Object  *div_result;

    str_a = str(a);
    str_b = str(b);
    
    if (gt(a, b))
        printf("a > b\n");
    else if (lt(a, b))
        printf("a < b\n");
    else
        printf("a == b\n");
    
    div_result = division(b, a);
    str_div = str(div_result);
    printf("b / a = %s\n", str_div);
    
    free(str_a);
    free(str_b);
    free(str_div);
    delete(div_result);
}

int         main(void)
{
    Object  *i1, *i2;
    Object  *f1, *f2;
    Object  *c1, *c2;

    printf("=== Int Tests ===\n");
    i1 = new(Int, 10);
    i2 = new(Int, 5);
    compareAndDivide(i2, i1);
    delete(i1);
    delete(i2);

    printf("\n=== Float Tests ===\n");
    f1 = new(Float, 10.5f);
    f2 = new(Float, 2.5f);
    compareAndDivide(f2, f1);
    delete(f1);
    delete(f2);

    printf("\n=== Char Tests ===\n");
    c1 = new(Char, 'Z');
    c2 = new(Char, 'A');
    compareAndDivide(c2, c1);
    delete(c1);
    delete(c2);

    return (0);
}
