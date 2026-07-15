#ifndef INCLUDED_ACOMPONENT_HPP
    #define INCLUDED_ACOMPONENT_HPP

#include <iostream>
#include "../IComponent.hpp"

class AComponent : public nts::IComponent
{
    private:
    protected:
    public:
        AComponent();
        AComponent(const AComponent& other);
        AComponent& operator=(const AComponent& other);
        ~AComponent() = default;

        void display(std::ostream& os = std::cout);
};

#endif
