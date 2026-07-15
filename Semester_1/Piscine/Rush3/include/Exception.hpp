/*
** EPITECH PROJECT, 2026
** piscine
** File description:
** Exception
*/

#pragma once

#include "VMException.hpp"

class StackException : public VMException
{
public:
    explicit StackException(const std::string& message)
        : VMException("Stack error: " + message)
    {
    }
};

class RegisterException : public VMException
{
public:
    explicit RegisterException(const std::string& message)
        : VMException("Register error: " + message)
    {
    }
};