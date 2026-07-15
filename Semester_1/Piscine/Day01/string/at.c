/*
** EPITECH PROJECT, 2026
** Day01
** File description:
** lenght
*/

#include "string.h"

char at_position(const string_t *this, size_t pos)
{
    int len = length(this);

    if (len <= pos)
        return -1;
    else
        return this->string[pos];
}
