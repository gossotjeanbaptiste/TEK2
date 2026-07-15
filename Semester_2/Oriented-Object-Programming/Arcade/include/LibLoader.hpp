#pragma once

#include <string>
#include <dlfcn.h>
#include <exception>

namespace arc {

    enum libType {
        Graphic,
        Game,
        Error,
    };

    class LibLoader
    {
        private:
            void *_handle;
            libType _libType;
            std::string _currentPath;

            void clearError();
        public:
            explicit LibLoader(std::string path);
            LibLoader();
            ~LibLoader();




            class exception : public std::exception {
                private:
                    std::string _message;
                public:
                    explicit exception(const std::string& message);
                    const char* what() const noexcept override;
            };

            class openException : public exception {
                public:
                    explicit openException(const std::string& libPath);
            };

            class closeException : public exception {
                public:
                    explicit closeException(const std::string& libPath);
            };

            class notfoundException : public exception {
                public:
                    explicit notfoundException(const std::string& functionName);
            };




            template<typename T>
                T loadFunction(std::string function_name) {
                    if (!_handle) {
                        throw exception("No library loaded");
                    }

                    dlerror();
                    T func = (T)dlsym(_handle, function_name.c_str());
                    const char* error = dlerror();

                    if (error != nullptr) {
                        std::string errorMsg = std::string("Symbol not found: ") + function_name + " - " + error;
                        throw exception(errorMsg);
                    }

                    return func;
                };

            void changeLib(std::string path);
            bool isLoaded() const;
            const std::string& getCurrentPath() const;
            libType getLibType() const;
            void setLibType(libType type);
            
            libType detectLibType();
    };

}
