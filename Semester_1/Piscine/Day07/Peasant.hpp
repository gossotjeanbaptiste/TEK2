#ifndef PEASANT_HPP_
    #define PEASANT_HPP_
    #include <iostream>
    #include <string>
    #include "ICharacter.hpp"
    #include "IPotion.hpp"

    class Peasant : public ICharacter {
        protected:
            std::string name;
            int power;
            int hp;
        public:
            Peasant(const std::string &name, int power);
            const std::string &getName() const override;
            int getPower() const override;
            int getHp() const override;
            virtual int attack() override;
            virtual int special() override;
            virtual void rest() override;
            void damage(int damage) override;
            void drink(const IPotion &potion) override;
            void addHp(int amount) override;
            void addPower(int amount) override;
            virtual ~Peasant();
    };
#endif /* PEASANT_HPP_ */