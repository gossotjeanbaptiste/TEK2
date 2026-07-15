#include "../../include/General.hpp"
#include <stdexcept>

// Définition du registre statique
std::map<nts::ComponentType, nts::ComponentFactory::CreatorFunc> nts::ComponentFactory::registry;

/**
 * @brief Initialise le registre des composants avec toutes les fonctions créatrices
 * 
 * Cette fonction remplit le registre statique avec les fonctions de création pour
 * tous les types de composants supportés :
 * - Portes logiques : AND, OR, NOT, XOR
 * - Circuits intégrés simples : 4001, 4011, 4030, 4069, 4071, 4081
 * - Circuits intégrés avancés : 2716, 4008, 4013, 4017, 4040, 4094, 4512, 4514, 4801
 * - Composants spéciaux : Clock, Input, Output, True, False, Logger
 * 
 * @note Cette fonction doit être appelée une seule fois avant la première création de composant
 * @see ComponentFactory::create()
 */
void nts::ComponentFactory::initRegistry()
{
    // * toutes les fonctions sont des lambdas, d'ou l'utilsation 
    // * d'un using dans le hpp afin de permettre l'utilisation d'un type CreatorFunc plutot que la définition du lambda
    registerComponent(ComponentType::AND, [](const std::string &name) { return new And(name); });
    registerComponent(ComponentType::OR, [](const std::string &name) { return new Or(name); });
    registerComponent(ComponentType::NOT, [](const std::string &name) { return new Not(name); });
    registerComponent(ComponentType::XOR, [](const std::string &name) { return new Xor(name); });
    
    registerComponent(ComponentType::CHIP_4001, [](const std::string &name) { return new Chip4001(name); });
    registerComponent(ComponentType::CHIP_4011, [](const std::string &name) { return new Chip4011(name); });
    registerComponent(ComponentType::CHIP_4030, [](const std::string &name) { return new Chip4030(name); });
    registerComponent(ComponentType::CHIP_4069, [](const std::string &name) { return new Chip4069(name); });
    registerComponent(ComponentType::CHIP_4071, [](const std::string &name) { return new Chip4071(name); });
    registerComponent(ComponentType::CHIP_4081, [](const std::string &name) { return new Chip4081(name); });
    
    registerComponent(ComponentType::CHIP_2716, [](const std::string &name) { return new Chip2716(name); });
    registerComponent(ComponentType::CHIP_4008, [](const std::string &name) { return new Chip4008(name); });
    registerComponent(ComponentType::CHIP_4013, [](const std::string &name) { return new Chip4013(name); });
    registerComponent(ComponentType::CHIP_4017, [](const std::string &name) { return new Chip4017(name); });
    registerComponent(ComponentType::CHIP_4040, [](const std::string &name) { return new Chip4040(name); });
    registerComponent(ComponentType::CHIP_4094, [](const std::string &name) { return new Chip4094(name); });
    registerComponent(ComponentType::CHIP_4512, [](const std::string &name) { return new Chip4512(name); });
    registerComponent(ComponentType::CHIP_4514, [](const std::string &name) { return new Chip4514(name); });
    registerComponent(ComponentType::CHIP_4801, [](const std::string &name) { return new Chip4801(name); });
    
    registerComponent(ComponentType::CLOCK, [](const std::string &name) { return new Clock(name); });
    registerComponent(ComponentType::INPUT, [](const std::string &name) { return new Input(name); });
    registerComponent(ComponentType::OUTPUT, [](const std::string &name) { return new Output(name); });
    registerComponent(ComponentType::TRUE_COMPONENT, [](const std::string &name) { return new TrueComponent(name); });
    registerComponent(ComponentType::FALSE_COMPONENT, [](const std::string &name) { return new FalseComponent(name); });
    registerComponent(ComponentType::LOGGER, [](const std::string &name) { return new Logger(name); });
}

/**
 * @brief Crée une instance d'un composant du type spécifié
 * @param type Le type de composant à créer (enum ComponentType)
 * @param name Le nom unique du composant dans le circuit
 * @return Un pointeur vers l'interface IComponent du composant créé
 * 
 * @throws std::runtime_error si le type de composant n'existe pas dans le registre
 * 
 * @note Le registre doit être initialisé via initRegistry() avant tout appel à create()
 * @note L'appelant est responsable de la libération de la mémoire allouée
 * 
 * @see initRegistry()
 * @see registerComponent()
 */
nts::IComponent* nts::ComponentFactory::create(ComponentType type, const std::string& name)
{
    auto it = registry.find(type);
    if (it == registry.end())
        throw std::runtime_error("Unknown component type");
    return it->second(name);
}

/**
 * @brief Enregistre une fonction créatrice pour un type de composant
 * @param type Le type de composant auquel associer la fonction (enum ComponentType)
 * @param creator Une fonction lambda ou callable qui crée une instance du composant
 * 
 * Cette fonction remplit le registre avec les fonctions de création.
 * Elle est appelée par initRegistry() et peut également être utilisée pour
 * enregistrer des types de composants personnalisés.
 * 
 * @note Chaque type ne peut être enregistré qu'une seule fois
 * 
 * @see initRegistry()
 * @see create()
 */
void nts::ComponentFactory::registerComponent(ComponentType type, CreatorFunc creator)
{
    registry[type] = creator;
}
