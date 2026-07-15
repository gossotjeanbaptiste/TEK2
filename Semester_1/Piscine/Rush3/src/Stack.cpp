/*
** EPITECH PROJECT, 2026
** piscine
** File description:
** Stack
*/

#include "../include/Stack.hpp"
#include "../include/Exception.hpp"

Stack::Stack() = default;

Stack::~Stack()
{
    cleanup();
}

void Stack::push(IOperand* op)
{
    _container.push_back(op);
}

IOperand* Stack::pop()
{
    if (_container.empty())
        throw StackException("Pop on empty stack");
    IOperand* top = _container.back();
    _container.pop_back();
    return top;
}

IOperand* Stack::top() const
{
    if (_container.empty())
        throw StackException("Top on empty stack");
    return _container.back();
}

void Stack::clear()
{
    for (IOperand* op : _container)
        delete op;
    _container.clear();
}

void Stack::cleanup()
{
    clear();
}

std::size_t Stack::size() const
{
    return _container.size();
}

bool Stack::empty() const
{
    return _container.empty();
}
