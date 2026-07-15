#ifndef INCLUDED_CIRCUIT_HPP
    #define INCLUDED_CIRCUIT_HPP

#include <map>
#include <memory>
#include <string>
#include "IComponent.hpp"

namespace nts {
    // Fonction utilitaire pour convertir Tristate en string
    inline std::string tristateToString(nts::Tristate value) {
        switch (value) {
            case nts::Tristate::True: return "1";
            case nts::Tristate::False: return "0";
            case nts::Tristate::Undefined: return "U";
            default: return "?";
        }
    }
    class Circuit {
        public:
            Circuit() : _tick(0) {}
            ~Circuit() = default;

            void addComponent(const std::string &type, const std::string &name);
            void addLink(const std::string &n1, size_t p1, const std::string &n2, size_t p2);
            
            void simulate();
            void display() const;
            void setInputValue(const std::string &name, const std::string &value);
            
            nts::IComponent* getComponent(const std::string &name) {
                auto it = _components.find(name);
                if (it != _components.end()) { return it->second.get(); }
                return nullptr;
            }
            bool isEmpty() const { return _components.empty(); }

        private:
            std::map<std::string, std::unique_ptr<nts::IComponent>> _components; 
            /**< Carte de tous les composants du circuit, indexée par le nom du composant */
            size_t _tick; /**< Compteur de tick de simulation actuel */
    };
}
#endif