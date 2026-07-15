/*
** EPITECH PROJECT, 2026
** Day01
** File description:
** main
*/

#include "string.h"

int main(void)
{
    string_t s;

    string_init(&s, "Foo");
    s.print(&s);
    printf("\n");
    s.append(&s, "Bar\n");
    s.print(&s);
    string_destroy(&s);
    return 0;
}
