#ifndef INCLUDED_SAFEDIRECTORYLISTER_HPP
    #define INCLUDED_SAFEDIRECTORYLISTER_HPP

#include <dirent.h>
#include "IDirectoryLister.hpp"

class SafeDirectoryLister : public IDirectoryLister
{
    private:
        DIR *_dir;
        bool _hidden;
    public:
        SafeDirectoryLister();
        SafeDirectoryLister(const std::string &path, bool hidden);
        ~SafeDirectoryLister();
        
        bool open(const std::string &path, bool hidden) override;
        std::string get() override;
        
        SafeDirectoryLister(const SafeDirectoryLister &) = delete;
        SafeDirectoryLister(SafeDirectoryLister &&) = delete;
        SafeDirectoryLister &operator=(const SafeDirectoryLister &) = delete;
        SafeDirectoryLister &operator=(SafeDirectoryLister &&) = delete;
};

#endif
