/*
** EPITECH PROJECT, 2026
** piscine
** File description:
** VMException
*/

#pragma once

#include <exception>
#include <string>

class VMException : public std::exception
{
public:
    // Constructeur avec message
    explicit VMException(const std::string& message)
        : _message(message)
    {
    }

    // Retourne le message d’erreur
    virtual const char* what() const noexcept override
    {
        return _message.c_str();
    }

private:
    std::string _message;
};
