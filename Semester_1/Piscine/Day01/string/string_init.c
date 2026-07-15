/*
** EPITECH PROJECT, 2026
** Day01
** File description:
** string_init
*/

#include "string.h"

void string_init(string_t *this, const char *s)
{
    this->string = strdup(s);
    if (this->string == NULL)
        abort();
    this->append = append;
    this->assign = assign;
    this->at = at_position;
    this->clear = clear;
    this->data = data;
    this->empty = empty;
    this->length = length;
    this->print = print;
    this->string_destroy = string_destroy;
}
