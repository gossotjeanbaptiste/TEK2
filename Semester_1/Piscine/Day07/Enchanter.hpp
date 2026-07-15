#ifndef ENCHANTER_HPP_
    #define ENCHANTER_HPP_
    #include "Peasant.hpp"
    class Enchanter : virtual public Peasant {
        public:
            Enchanter(const std::string &name, int power);
            virtual int attack() override;
            virtual int special() override;
            virtual void rest() override;
            ~Enchanter();
    };
#endif /* ENCHANTER_HPP_ */