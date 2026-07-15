/*
** EPITECH PROJECT, 2026
** Day01
** File description:
** empty
*/

#include "string.h"

bool empty(const string_t *this)
{
    if (strlen(this->string) != 0)
        return false;
    return true;
}
