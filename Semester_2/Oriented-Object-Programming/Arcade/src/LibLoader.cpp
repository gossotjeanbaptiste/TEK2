#include "../include/LibLoader.hpp"
#include <iostream>


arc::LibLoader::exception::exception(const std::string& message) : _message(message) {}

const char* arc::LibLoader::exception::what() const noexcept {
    return _message.c_str();
}

arc::LibLoader::openException::openException(const std::string& libPath) 
    : exception("Failed to open library: " + libPath) 
{}

arc::LibLoader::closeException::closeException(const std::string& libPath) 
    : exception("Failed to close library: " + libPath)
{}

arc::LibLoader::notfoundException::notfoundException(const std::string& functionName) 
    : exception("Symbol not found: " + functionName)
{}

void arc::LibLoader::clearError()
{
    dlerror();
}

arc::LibLoader::LibLoader(std::string path) : _handle(nullptr), _libType(Error)
{
    changeLib(path);
}

arc::LibLoader::LibLoader() : _handle(nullptr), _libType(Error)
{
    clearError();
}

arc::LibLoader::~LibLoader()
{
    if (_handle) {
        if (dlclose(_handle) != 0)
            std::cerr << closeException(_currentPath).what() << std::endl;
        _handle = nullptr;
    }
}

void arc::LibLoader::changeLib(std::string path)
{
    clearError();
    if (_handle) {
        if (dlclose(_handle) != 0)
            throw closeException(_currentPath);
        _handle = nullptr;
    }
    _handle = dlopen(path.c_str(), RTLD_LAZY);
    _currentPath = path;
    if (!_handle) {
        const char* error = dlerror();
        std::string errorMsg = 
        std::string("Failed to open library: ") + path + " - " + (error ? error : "Unknown error");
        throw LibLoader::exception(errorMsg);
    }
    detectLibType();
}

bool arc::LibLoader::isLoaded() const
{
    return _handle != nullptr;
}

const std::string& arc::LibLoader::getCurrentPath() const
{
    return _currentPath;
}

arc::libType arc::LibLoader::getLibType() const
{
    return _libType;
}

void arc::LibLoader::setLibType(libType type)
{
    _libType = type;
}

arc::libType arc::LibLoader::detectLibType()
{
    clearError();
    if (!_handle) {
        _libType = Error;
        return Error;
    }
    dlerror();
    void* graphicsSymbol = dlsym(_handle, "createGraphics");
    if (dlerror() == nullptr && graphicsSymbol != nullptr) {
        _libType = Graphic;
        return Graphic;
    }
    dlerror();
    void* gameSymbol = dlsym(_handle, "createGame");
    if (dlerror() == nullptr && gameSymbol != nullptr) {
        _libType = Game;
        return Game;
    }
    _libType = Error;
    return Error;
}

