#ifndef POWERPOTION_HPP_
    #define POWERPOTION_HPP_
    #include "IPotion.hpp"
    #include "ICharacter.hpp"
    #include <iostream>
    #include <algorithm>

    class PowerPotion : public IPotion {
        public:
            PowerPotion();
            ~PowerPotion();
            int getType() const override { return 2; }
            void applyEffect(ICharacter &character) const override {
                std::cout << character.getName() << "'s power is restored." << std::endl;
                const_cast<ICharacter&>(character).addPower(50);
            }
    };

#endif /* POWERPOTION_HPP_ */
