#include "IDirectoryLister.hpp"
#include <cstring>

IDirectoryLister::OpenFailureException::OpenFailureException(const std::string &message)
    : _message(message) {}

const char *IDirectoryLister::OpenFailureException::what() const noexcept {
    return _message.c_str();
}

const char *IDirectoryLister::NoMoreFileException::what() const noexcept {
    return "End of stream";
}
