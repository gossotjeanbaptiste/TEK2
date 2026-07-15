/*
** EPITECH PROJECT, 2026
** Day01
** File description:
** lenght
*/
#include "string.h"

void append(string_t *this, const char *str)
{
    int len_this = strlen(this->string);
    int len_str = strlen(str);
    int size = len_this + len_str + 1;
    char *new_str = malloc(size * sizeof(char));

    for (int i = 0; i < len_this; i++)
        new_str[i] = this->string[i];
    for (int i = 0; i < len_str; i++)
        new_str[len_this + i] = str[i];
    new_str[size - 1] = '\0';
    free(this->string);
    this->string = new_str;
}
