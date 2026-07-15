#include "SafeDirectoryLister.hpp"
#include <cstring>
#include <cerrno>

SafeDirectoryLister::SafeDirectoryLister() : _dir(nullptr), _hidden(false) {}

SafeDirectoryLister::SafeDirectoryLister(const std::string &path, bool hidden)
    : _dir(nullptr), _hidden(hidden) {
    open(path, hidden);
}

SafeDirectoryLister::~SafeDirectoryLister() {
    if (_dir != nullptr) {
        closedir(_dir);
    }
}

bool SafeDirectoryLister::open(const std::string &path, bool hidden) {
    if (_dir != nullptr) {
        closedir(_dir);
        _dir = nullptr;
    }
    
    _hidden = hidden;
    _dir = opendir(path.c_str());
    
    if (_dir == nullptr) {
        throw OpenFailureException(strerror(errno));
    }
    return true;
}

std::string SafeDirectoryLister::get() {
    if (_dir == nullptr)
        throw OpenFailureException("Directory stream is invalid");
    
    struct dirent *entry = readdir(_dir);
    
    while (entry != nullptr) {
        if (!_hidden && entry->d_name[0] == '.')
            entry = readdir(_dir);
        else
            return entry->d_name;
    }
    throw NoMoreFileException();
}
