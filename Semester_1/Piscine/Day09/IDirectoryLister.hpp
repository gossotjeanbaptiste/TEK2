#ifndef INCLUDED_IDIRECTORYLISTER_HPP
    #define INCLUDED_IDIRECTORYLISTER_HPP

#include <string>
#include <exception>

class IDirectoryLister
{
    public:
        class OpenFailureException : public std::exception {
        private:
            std::string _message;
        public:
            OpenFailureException(const std::string &message);
            virtual const char *what() const noexcept override;
        };

        class NoMoreFileException : public std::exception {
        public:
            virtual const char *what() const noexcept override;
        };

        virtual ~IDirectoryLister() = default;
        virtual bool open(const std::string &path, bool hidden) = 0;
        virtual std::string get() = 0;
};

#endif
