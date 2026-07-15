#ifndef KNIGHT_HPP_
    #define KNIGHT_HPP_
    #include "Peasant.hpp"
    class Knight : virtual public Peasant {
        public:
            Knight(const std::string &name, int power);
            virtual int attack() override;
            int special() override;
            void rest() override;
            ~Knight();
    };
#endif /* KNIGHT_HPP_ */