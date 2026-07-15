#include "../include/Circuit.hpp"
#include "../include/Components/ComponentFactory.hpp"
#include "../include/Components/Special/Input.hpp"
#include "../include/Components/Special/Output.hpp"
#include "../include/Components/Special/Clock.hpp"
#include "../include/Components/Special/True.hpp"
#include "../include/Components/Special/False.hpp"
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>


/**
 * @brief Convertit une représentation en chaîne de caractères en sa valeur enum ComponentType correspondante.
 * 
 * Cette fonction associe les identifiants de chaîne à leurs énumérations nts::ComponentType correspondantes.
 * Elle supporte les composants de base (input, output, clock, true, false), les portes logiques (and, or, xor, not),
 * et diverses puces de circuits intégrés (4001, 4011, 4030, 4069, 4071, 4081, 4008, 4013, 4017, 
 * 4040, 4094, 4512, 4514, 4801, 2716) ainsi que les composants logger.
 * 
 * @param type Une référence const à une chaîne contenant l'identifiant du type de composant.
 * 
 * @return nts::ComponentType La valeur enum correspondant à la chaîne d'entrée.
 * 
 * @throws std::runtime_error Si la chaîne fournie ne correspond à aucun type de composant connu.
 * 
 * @note Utilise une carte statique pour une recherche efficace. La carte n'est initialisée qu'une seule fois.
 * 
 * @see nts::ComponentType
 */
static nts::ComponentType stringToType(const std::string &type) {
    // !                        clé             valeur
    static const std::map<std::string, nts::ComponentType> typeMap = {
        {"input", nts::ComponentType::INPUT},
        {"output", nts::ComponentType::OUTPUT},
        {"clock", nts::ComponentType::CLOCK},
        {"true", nts::ComponentType::TRUE_COMPONENT},
        {"false", nts::ComponentType::FALSE_COMPONENT},
        {"and", nts::ComponentType::AND},
        {"or", nts::ComponentType::OR},
        {"xor", nts::ComponentType::XOR},
        {"not", nts::ComponentType::NOT},
        {"4001", nts::ComponentType::CHIP_4001},
        {"4011", nts::ComponentType::CHIP_4011},
        {"4030", nts::ComponentType::CHIP_4030},
        {"4069", nts::ComponentType::CHIP_4069},
        {"4071", nts::ComponentType::CHIP_4071},
        {"4081", nts::ComponentType::CHIP_4081},
        {"4008", nts::ComponentType::CHIP_4008},
        {"4013", nts::ComponentType::CHIP_4013},
        {"4017", nts::ComponentType::CHIP_4017},
        {"4040", nts::ComponentType::CHIP_4040},
        {"4094", nts::ComponentType::CHIP_4094},
        {"4512", nts::ComponentType::CHIP_4512},
        {"4514", nts::ComponentType::CHIP_4514},
        {"4801", nts::ComponentType::CHIP_4801},
        {"2716", nts::ComponentType::CHIP_2716},
        {"logger", nts::ComponentType::LOGGER}
    };
    auto it = typeMap.find(type);
    if (it == typeMap.end())
        throw std::runtime_error("Unknown component type: " + type);
    return it->second; // retourne la valeur
}

/**
 * @brief Ajoute un nouveau composant au circuit
 * 
 * Crée un composant du type spécifié avec le nom donné et l'ajoute à la collection
 * de composants du circuit. Le nom du composant doit être unique dans le circuit.
 * 
 * @param type Le type de composant à créer (par ex. "AND", "OR", "INPUT", etc.)
 * @param name L'identifiant unique du composant au sein de ce circuit
 * 
 * @throws std::runtime_error Si un composant avec le même nom existe déjà dans le circuit
 * 
 * @note La fonction prend possession du composant créé via unique_ptr
 * @note La chaîne du type de composant est convertie en énumération ComponentType via stringToType()
 * @note Le composant est instancié en utilisant ComponentFactory::create()
 * 
 * @see ComponentFactory::create()
 * @see stringToType()
 */
void nts::Circuit::addComponent(const std::string &type, const std::string &name) {
    if (_components.find(name) != _components.end())
        throw std::runtime_error("Component name already exists: " + name);
    nts::ComponentType cType = stringToType(type);
    nts::IComponent *newComp = nts::ComponentFactory::create(cType, name);
    _components[name] = std::unique_ptr<nts::IComponent>(newComp);
}

/**
 * @brief Établit un lien bidirectionnel entre deux broches de composants.
 * 
 * Crée une connexion entre deux broches de différents composants du circuit.
 * Le lien est bidirectionnel, ce qui signifie que les deux composants sont conscients de la connexion.
 * 
 * @param n1 Le nom du premier composant.
 * @param p1 Le numéro de broche du premier composant.
 * @param n2 Le nom du deuxième composant.
 * @param p2 Le numéro de broche du deuxième composant.
 * 
 * @throws std::runtime_error Si l'un des noms de composants (n1 ou n2) n'existe pas
 *                             dans la carte des composants du circuit.
 * 
 * @note Les deux composants doivent exister dans le circuit avant de les lier.
 *       L'ordre des paramètres détermine la direction de la connexion,
 *       mais le lien lui-même est bidirectionnel.
 */
void nts::Circuit::addLink(const std::string &n1, size_t p1, const std::string &n2, size_t p2) {
    if (_components.find(n1) == _components.end() || _components.find(n2) == _components.end())
        throw std::runtime_error("Unknown component name in link");
    _components[n1]->setLink(p1, *_components[n2], p2);
    _components[n2]->setLink(p2, *_components[n1], p1);
}

/**
 * @brief Simule un cycle du circuit.
 * 
 * Incrémente le compteur de tick de simulation et appelle la méthode simulate
 * sur chaque composant du circuit avec la valeur de tick mise à jour.
 * 
 * @return void
 */
void nts::Circuit::simulate() {
    _tick++;
    for (auto &it : _components)
        it.second->simulate(_tick);
}

/**
 * @brief Affiche l'état actuel du circuit.
 * 
 * Affiche le compteur de tick actuel et énumère tous les composants du circuit
 * avec leurs valeurs calculées à la broche 1. Pour chaque composant, tente de
 * récupérer la valeur tristate ; si une exception se produit, affiche 'U'
 * (indéfini) à la place.
 * 
 * @note Ceci est une fonction membre const qui ne modifie pas l'état du circuit.
 * 
 * Format de sortie :
 * - Première ligne : "tick: <valeur_tick>"
 * - Lignes suivantes : en-tête "input(s):" suivi des noms des composants et des valeurs
 * - Section finale : en-tête "output(s):" (actuellement vide)
 * 
 * @return void
 */
void nts::Circuit::display() const {
    std::cout << "tick: " << _tick << std::endl;
    
    std::cout << "input(s):" << std::endl;
    for (auto const& [name, comp] : _components) {
        if (dynamic_cast<Input*>(comp.get()) || 
            dynamic_cast<Clock*>(comp.get())) {
            try {
                nts::Tristate value = comp->compute(1); // ! compute sur le composant pas le circuit
                std::cout << "  " << name << ": " << nts::tristateToString(value) << std::endl;
            } catch (...) {
                std::cout << "  " << name << ": U" << std::endl;
            }
        }
    }
    std::cout << "output(s):" << std::endl;
    for (auto const& [name, comp] : _components) {
        if (dynamic_cast<Output*>(comp.get()) ||
            dynamic_cast<TrueComponent*>(comp.get()) ||
            dynamic_cast<FalseComponent*>(comp.get())) {
            try {
                nts::Tristate value = comp->compute(1); // * voir Circuit.cpp:157
                std::cout << "  " << name << ": " << nts::tristateToString(value) << std::endl;
            } catch (...) {
                std::cout << "  " << name << ": U" << std::endl;
            }
        }
    }
}

/**
 * @brief Définit la valeur d'entrée d'un composant nommé dans le circuit.
 * 
 * Cette fonction vous permet d'assigner une valeur Tristate à un composant d'entrée
 * par son nom. La valeur peut être spécifiée sous forme de chaîne ("0", "1", "U").
 * 
 * @param name Le nom du composant d'entrée à définir. Doit correspondre à un composant existant
 *             dans la carte des composants du circuit.
 * @param value La valeur à assigner en tant que chaîne. Les valeurs acceptées sont :
 *              - "0" pour nts::False
 *              - "1" pour nts::True
 * 
 * @throws std::runtime_error si le composant avec le nom donné n'existe pas
 *         dans la carte des composants du circuit.
 * @throws std::runtime_error si la valeur fournie n'est pas l'une des représentations
 *         de chaîne acceptées ("0", "1", ou "U").
 * 
 * @note La fonction utilise dynamic_cast pour convertir le composant en pointeur Input.
 *       Si le cast échoue (le composant n'est pas de type Input), la fonction retourne
 *       silencieusement sans définir aucune valeur.
 * 
 * @see nts::Tristate, nts::Input
 */
void nts::Circuit::setInputValue(const std::string &name, const std::string &value) {
    if (_components.find(name) == _components.end())
        throw std::runtime_error("Unknown input name: " + name);
    nts::Tristate tristate;
    if (value == "1")
        tristate = nts::True;
    else if (value == "0")
        tristate = nts::False;
    else if (value == "U")
        tristate = nts::Undefined;
    else
        throw std::runtime_error("Invalid value: " + value + " (use 0, 1 or U)");
    Input *input = dynamic_cast<Input*>(_components[name].get());
    if (input) { input->setValue(tristate); } 
    // * assigne la nouvelle valeur de nts::Tristate _value défini dans Input.hpp
}

