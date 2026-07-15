#ifndef INCLUDED_CYBERMAN_HPP
    #define INCLUDED_CYBERMAN_HPP

#include <iostream>
#include "Human.hpp"

namespace Mondas {
    class Cyberman {
        private:
            Mondas::Human &_human;
            std::string _leetedName;
        public:
            class Inhibitor {
                public:
                    Inhibitor() : _active(false) {}
                    void set(bool active) { _active = active; }
                    bool get() const {return _active; }
                private:
                bool _active;
            };
            Cyberman(Mondas::Human &human);
            ~Cyberman() = default;
            void think() const;
            unsigned int getIq() const;
            Mondas::Human &getHuman();
            const Mondas::Human &getHuman() const;
    };
}

#endif
