#ifndef Priest_HPP_
    #define Priest_HPP_
    #include "Peasant.hpp"
    #include "Enchanter.hpp"
    class Priest : virtual public Enchanter {
        public:
            Priest(const std::string &name, int power);
            virtual void rest() override;
            ~Priest();
    };
#endif /* Priest_HPP_ */