/*
** EPITECH PROJECT, 2026
** Day01
** File description:
** lenght
*/

#include "string.h"

void assign(string_t *this, const char *str)
{
    free(this->string);
    this->string = strdup(str);
    if (this->string == NULL)
        abort();
}
