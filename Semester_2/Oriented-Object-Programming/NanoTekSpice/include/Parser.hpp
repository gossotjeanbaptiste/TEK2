#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <iostream>
#include <sstream>
#include "IComponent.hpp"
#include "Circuit.hpp"

namespace nts {
    class Parser {
        public:
            Parser() = default;
            ~Parser() = default;
            void parseFile(const std::string &filename, nts::Circuit &circuit);

        private:
            void parseChipsets(const std::string &line, nts::Circuit &circuit);
            void parseLinks(const std::string &line, nts::Circuit &circuit);
            std::string cleanLine(std::string line);
            enum Section {
                NONE,
                CHIPSETS,
                LINKS
            };
            Section _currentSection = NONE;
    };

    class ParserException : public std::exception {
        public:
            ParserException(const std::string &message) : _message(message) {}
            const char *what() const noexcept override { return _message.c_str(); }
        private:
            std::string _message;
    };
}

#endif /* !PARSER_HPP_ */