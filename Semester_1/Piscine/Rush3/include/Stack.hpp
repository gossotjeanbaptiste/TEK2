/*
** EPITECH PROJECT, 2026
** piscine
** File description:
** Stack
*/

#pragma once

#include <vector>
#include "../include/IOperand.hpp"
#include "VMException.hpp"

class Stack
{
public:
    Stack();
    ~Stack();

    void push(IOperand* op);
    IOperand* pop();
    IOperand* top() const;

    void clear();
    void cleanup(); // Supprime vraiment les operands
    std::size_t size() const;
    bool empty() const;

private:
    std::vector<IOperand*> _container;
};
