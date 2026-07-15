#ifndef INCLUDED_CHIP4008_HPP
#define INCLUDED_CHIP4008_HPP

#include "../AComponent.hpp"
#include <map>
#include <utility>

namespace nts {
    class Chip4008 : public AComponent
    {
        private:
            struct Link {
                IComponent* component;
                std::size_t pin;
            };
            std::map<std::size_t, Link> _links;
            std::pair<Tristate, Tristate> fullAdder(Tristate a, Tristate b, Tristate cin);
            Tristate xorGate(Tristate a, Tristate b);
            Tristate computeInput(std::size_t pin);

        public:
            Chip4008(const std::string& name);
            virtual ~Chip4008() = default;
            void simulate(std::size_t tick) override;
            Tristate compute(std::size_t pin) override;
            void setLink(std::size_t pin, IComponent &other, std::size_t otherPin) override;
    };
}

#endif