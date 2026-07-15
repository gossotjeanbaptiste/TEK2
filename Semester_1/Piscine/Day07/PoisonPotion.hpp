#ifndef POISONPOTION_HPP_
    #define POISONPOTION_HPP_
    #include "IPotion.hpp"
    #include "ICharacter.hpp"
    #include <iostream>

    class PoisonPotion : public IPotion {
        public:
            PoisonPotion();
            ~PoisonPotion();
            int getType() const override { return 3; }
            void applyEffect(ICharacter &character) const override {
                std::cout << character.getName() << " has been poisoned." << std::endl;
                const_cast<ICharacter&>(character).damage(50);
            }
    };

#endif /* POISONPOTION_HPP_ */
