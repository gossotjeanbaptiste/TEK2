#ifndef PALADIN_HPP_
    #define PALADIN_HPP_
    #include <string>
    #include <cstddef>
    #include <ostream>
    #include "Priest.hpp"
    #include "Enchanter.hpp"
    #include "Peasant.hpp"
    #include "Knight.hpp"
    class Paladin :
    virtual public Peasant, virtual public Knight, 
    virtual public Enchanter, virtual public Priest
    { 
        public :
        Paladin(const std::string& name, int power);
        int attack() override;
        int special() override;
        void rest() override;
        ~Paladin();
    };
#endif