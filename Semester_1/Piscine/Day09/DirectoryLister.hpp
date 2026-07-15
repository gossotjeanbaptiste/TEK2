#ifndef INCLUDED_DIRECTORYLISTER_HPP
    #define INCLUDED_DIRECTORYLISTER_HPP

#include <iostream>
#include <dirent.h>
#include "IDirectoryLister.hpp"

class DirectoryLister : public IDirectoryLister
{
    private:
    DIR *_dir;
    bool _hidden;
    protected:
    public:
        DirectoryLister();
        DirectoryLister(const std::string &path, bool hidden);
        ~DirectoryLister();
        bool open(const std::string &path, bool hidden);
        std::string get();
        DirectoryLister(const DirectoryLister &) = delete;
        DirectoryLister(DirectoryLister &&) = delete;
        DirectoryLister &operator=(const DirectoryLister &) = delete;
        DirectoryLister &operator=(DirectoryLister &&) = delete;
};

#endif
