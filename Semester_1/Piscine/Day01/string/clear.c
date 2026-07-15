/*
** EPITECH PROJECT, 2026
** Day01
** File description:
** lenght
*/

#include "string.h"

void clear(string_t *this)
{
    if (this->string != NULL) {
        free(this->string);
    }
    this->string = strdup("");
    if (this->string == NULL)
        abort();
}
