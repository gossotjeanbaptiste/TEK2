#ifndef ICHARACTER_HPP_
    #define ICHARACTER_HPP_
    #include <string>

    class IPotion;

    class ICharacter {
        public:
            virtual ~ICharacter() = default;
            virtual const std::string &getName() const = 0;
            virtual int getHp() const = 0;
            virtual int getPower() const = 0;
            virtual int attack() = 0;
            virtual int special() = 0;
            virtual void rest() = 0;
            virtual void damage(int dmg) = 0;
            virtual void drink(const IPotion &potion) = 0;
            virtual void addHp(int amount) = 0;
            virtual void addPower(int amount) = 0;
    };
#endif /* ICHARACTER_HPP_ */
