#ifndef INCLUDED_ARRAY_HPP
#define INCLUDED_ARRAY_HPP

#include <iostream>
#include <functional>
#include <stdexcept>

template<typename Type, std::size_t Size>
class Array
{
private:
    Type data[Size] = {};

public:
    Array() = default;

    ~Array() = default;

    Type& operator[](std::size_t index)
    {
        if (index >= Size)
            throw std::out_of_range("Out of range");
        return data[index];
    }

    const Type& operator[](std::size_t index) const
    {
        if (index >= Size)
            throw std::out_of_range("Out of range");
        return data[index];
    }

    std::size_t size() const
    {
        return Size;
    }

    void forEach(const std::function<void(const Type&)>& task) const
    {
        for (std::size_t i = 0; i < Size; ++i) {
            task(data[i]);
        }
    }

    template<typename U>
    Array<U, Size> convert(const std::function<U(const Type&)>& converter) const
    {
        Array<U, Size> result;
        for (std::size_t i = 0; i < Size; ++i) {
            result[i] = converter(data[i]);
        }
        return result;
    }

    template<typename U, std::size_t S>
    friend std::ostream& operator<<(std::ostream& os, const Array<U, S>& array);
};

template<typename Type, std::size_t Size>
std::ostream& operator<<(std::ostream& os, const Array<Type, Size>& array)
{
    os << "[";
    for (std::size_t i = 0; i < Size; ++i) {
        os << array.data[i];
        if (i < Size - 1)
            os << ", ";
    }
    os << "]";
    return os;
}

#endif
