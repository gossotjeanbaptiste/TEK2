/*
** EPITECH PROJECT, 2026
** Day01
** File description:
** string_destroy
*/

#include "string.h"

void string_destroy(string_t *this)
{
    if (this->string != NULL) {
        free(this->string);
        this->string = NULL;
    }
}
