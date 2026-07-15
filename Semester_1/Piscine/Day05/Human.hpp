#ifndef HUMAN_HPP_
    #define HUMAN_HPP_
    #include <iostream>
    #include "Cyberman.hpp"

namespace Mondas {
    class Cyberman;
    class Human {
    private:
        std::string _name;
        unsigned int _iq;
        Mondas::Cyberman::Inhibitor* _inhibitor;
    public:
        Human(std::string _name, unsigned int iq = 192, Mondas::Cyberman::Inhibitor* inhibitor = nullptr);
        ~Human();
        std::string getName() const;
        unsigned int getIq() const;
        void setIq(unsigned int newIq);
        void think() const;
    };    
}

#endif /* HUMAN_HPP_ */
