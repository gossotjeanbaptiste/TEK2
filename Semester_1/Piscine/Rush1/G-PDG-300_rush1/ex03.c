/*
** EPITECH PROJECT, 2026
** Paradigms Seminar
** File description:
** Exercice 03
*/

#include <stdio.h>
#include <stdlib.h>
#include "new.h"
#include "point.h"
#include "vertex.h"

int         main(void)
{
    Object  *p1 = new(Point, 12, 13);
    Object  *p2 = new(Point, 2, 2);
    Object  *v1 = new(Vertex, 1, 2, 3);
    Object  *v2 = new(Vertex, 4, 5, 6);
    char    *str_p1, *str_p2, *str_add, *str_sub;
    Object  *add_result, *sub_result;
    

    str_p1 = str(p1);
    str_p2 = str(p2);
    add_result = addition(p1, p2);
    str_add = str(add_result);
    printf("%s + %s = %s\n", str_p1, str_p2, str_add);
    free(str_p1);
    free(str_p2);
    free(str_add);
    delete(add_result);

    str_p1 = str(p1);
    str_p2 = str(p2);
    sub_result = subtraction(p1, p2);
    str_sub = str(sub_result);
    printf("%s - %s = %s\n", str_p1, str_p2, str_sub);
    free(str_p1);
    free(str_p2);
    free(str_sub);
    delete(sub_result);

    str_p1 = str(v1);
    str_p2 = str(v2);
    add_result = addition(v1, v2);
    str_add = str(add_result);
    printf("%s + %s = %s\n", str_p1, str_p2, str_add);
    free(str_p1);
    free(str_p2);
    free(str_add);
    delete(add_result);

    str_p1 = str(v1);
    str_p2 = str(v2);
    sub_result = subtraction(v1, v2);
    str_sub = str(sub_result);
    printf("%s - %s = %s\n", str_p1, str_p2, str_sub);
    free(str_p1);
    free(str_p2);
    free(str_sub);
    delete(sub_result);

    delete(p1);
    delete(p2);
    delete(v1);
    delete(v2);
    return (0);
}
