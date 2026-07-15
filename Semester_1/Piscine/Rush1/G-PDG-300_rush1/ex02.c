/*
** EPITECH PROJECT, 2026
** Paradigms Seminar
** File description:
** Exercise 02
*/

#include <stdio.h>
#include <stdlib.h>
#include "new.h"
#include "point.h"
#include "vertex.h"

int         main(void)
{
    Object  *point = new(Point, 42, -42);
    Object  *vertex = new(Vertex, 0, 1, 2);
    char    *point_str = str(point);
    char    *vertex_str = str(vertex);

    printf("point = %s\n", point_str);
    printf("vertex = %s\n", vertex_str);

    free(point_str);
    free(vertex_str);
    delete(point);
    delete(vertex);
    return (0);
}
