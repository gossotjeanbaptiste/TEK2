#ifndef INCLUDED_COMPONENTFACTORY_HPP
#define INCLUDED_COMPONENTFACTORY_HPP

#include <map>
#include <functional>
#include <string>
#include "../IComponent.hpp"

namespace nts {
    enum class ComponentType {
        // Gates
        AND,
        OR,
        NOT,
        XOR,
        // Chipsets simples
        CHIP_4001,
        CHIP_4011,
        CHIP_4030,
        CHIP_4069,
        CHIP_4071,
        CHIP_4081,
        // Chipsets avancés
        CHIP_2716,
        CHIP_4008,
        CHIP_4013,
        CHIP_4017,
        CHIP_4040,
        CHIP_4094,
        CHIP_4512,
        CHIP_4514,
        CHIP_4801,
        // Composants spéciaux
        CLOCK,
        INPUT,
        OUTPUT,
        TRUE_COMPONENT,
        FALSE_COMPONENT,
        LOGGER
    };

    class ComponentFactory {
    private:
        // * `using CreatorFunc est un alias afin d'eviter de recopier les signatures des lambdas
        using CreatorFunc = std::function<nts::IComponent*(const std::string&)>;
        static std::map<ComponentType, CreatorFunc> registry;
    public:
        // * static signifie que c'est une methode de classe et pas une methode d'instance on peut l'appeler sans creer d'obj ComponentFactory
        static nts::IComponent* create(ComponentType type, const std::string& name);
        // * équivalent void registerComponent(ComponentType type, std::function<nts::IComponent*(const std::string&)> creator)
        static void registerComponent(ComponentType type, CreatorFunc creator);
        static void initRegistry();
    };

}

#endif
