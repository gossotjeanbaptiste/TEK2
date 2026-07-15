

#ifndef INCLUDED_IPOTION_HPP
#define INCLUDED_IPOTION_HPP

#include <iostream>

class ICharacter;

class IPotion {
    public:
        virtual ~IPotion() = default;
        virtual int getType() const = 0;
        virtual void applyEffect(ICharacter &character) const = 0;
};

class UnknownPotion : public IPotion {
    public:
        UnknownPotion();
        ~UnknownPotion();
        int getType() const override { return 0; }
        void applyEffect(ICharacter &character) const override;
};

#endif