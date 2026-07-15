#include "IDirectoryLister.hpp"
#include "DirectoryLister.hpp"
#include <iostream>
#include <cstring>

DirectoryLister::DirectoryLister() : _dir(nullptr), _hidden(false) {}

DirectoryLister::DirectoryLister(const std::string &path, bool hidden) : _dir(nullptr), _hidden(hidden)
{
    open(path, hidden);
} 

DirectoryLister::~DirectoryLister() 
{
    if (_dir != nullptr)
        closedir(_dir);
}

bool DirectoryLister::open(const std::string &path, bool hidden)
{
    if (_dir != nullptr) {
        closedir(_dir);
        _dir = nullptr;
    }
    _hidden = hidden;
    _dir = opendir(path.c_str());

    if (_dir == nullptr) {
        perror(path.c_str());
        return false;
    }
    return true;
}

std::string DirectoryLister::get()
{
    if (_dir == nullptr)
        return "";
    struct dirent *entry = readdir(_dir);
    while (entry != nullptr) {
        if (!_hidden && entry->d_name[0] == '.')
            entry = readdir(_dir);
        else
            return entry->d_name;
    }
    return "";
}