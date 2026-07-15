#ifndef HEALTHPOTION_HPP_
    #define HEALTHPOTION_HPP_
    #include "IPotion.hpp"
    #include "ICharacter.hpp"
    #include <iostream>
    #include <algorithm>

    class HealthPotion : public IPotion {
        public:
            HealthPotion();
            ~HealthPotion();
            int getType() const override { return 1; }
            void applyEffect(ICharacter &character) const override {
                std::cout << character.getName() << " feels rejuvenated." << std::endl;
                const_cast<ICharacter&>(character).addHp(50);
            }
    };

#endif /* HEALTHPOTION_HPP_ */
