#ifndef CHIP4094_HPP
#define CHIP4094_HPP

#include "../AComponent.hpp"
#include <vector>
#include <map>

namespace nts {
    class Chip4094 : public AComponent {
        public:
            Chip4094(const std::string &name);
            virtual ~Chip4094() = default;

            void simulate(std::size_t tick) override;
            Tristate compute(std::size_t pin) override;
            void setLink(std::size_t pin, IComponent &other, std::size_t otherPin) override;

        private:
            struct Link {
                IComponent* component;
                std::size_t pin;
            };
            std::map<std::size_t, Link> _links;
            Tristate computeInput(std::size_t pin);            
            std::vector<Tristate> _shiftRegister;
            std::vector<Tristate> _latches;
            Tristate _lastClock;
    };
}
#endif