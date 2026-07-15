#include "../../../include/Components/Adv_Chipset/4801.hpp"

/**
 * @brief Constructeur de Chip4801
 * @param name Nom du composant RAM
 * 
 * Initialise la puce 4801 avec un nom et remplit la mémoire de 256 bytes à 0.
 */
Chip4801::Chip4801(const std::string& name) : _name(name)
{
    _memory.fill(0);
}

/**
 * @brief Constructeur de copie de Chip4801
 * @param other Référence constante à l'objet à copier
 * 
 * Effectue une copie profonde de tous les attributs du composant.
 */
Chip4801::Chip4801(const Chip4801& other) : AComponent(other), _name(other._name), _memory(other._memory)
{
}

/**
 * @brief Opérateur d'assignation
 * @param other Référence constante à l'objet à assigner
 * @return Référence à l'objet courant
 * 
 * Copie les attributs du composant source vers le composant courant.
 */
Chip4801& Chip4801::operator=(const Chip4801& other)
{
    if (this != &other) {
        AComponent::operator=(other);
        _name = other._name;
        _memory = other._memory;
    }
    return *this;
}

/**
 * @brief Simule le comportement de la RAM à chaque cycle
 * @param tick Numéro du cycle de simulation (inutilisé)
 * 
 * Effectue les opérations de lecture/écriture contrôlées par les signaux :
 * - enable (pin 18) : Active la RAM
 * - write (pin 21) : Sélectionne l'opération d'écriture
 * - read (pin 20) : Sélectionne l'opération de lecture
 */
void Chip4801::simulate(std::size_t tick)
{
    (void)tick;
    
    nts::Tristate enable = _getPinValue(18);
    nts::Tristate write = _getPinValue(21);
    nts::Tristate read = _getPinValue(20);
    
    if (enable == nts::False)
        return;
    
    unsigned char address = _getAddress();
    
    if (write == nts::True) {
        unsigned char data = _getDataByte();
        _memory[address] = data;
    }
    
    if (read == nts::True) {
        unsigned char data = _memory[address];
        _setDataByte(data);
    }
}

/**
 * @brief Retourne la valeur logique d'une pin de sortie
 * @param pin Numéro de la pin à lire
 * @return Valeur tristate de la pin (True, False ou Undefined)
 * 
 * Retourne les valeurs des pins de données de sortie (9-17, 13-16)
 * mises à jour lors d'une opération de lecture.
 */
nts::Tristate Chip4801::compute(std::size_t pin)
{
    if ((pin >= 9 && pin <= 17) || pin == 13 || pin == 14 || pin == 15 || pin == 16)
        if (_pinValues.find(pin) != _pinValues.end())
            return _pinValues[pin];
    return nts::Undefined;
}

/**
 * @brief Établit une connexion entre une pin et un autre composant
 * @param pin Numéro de la pin locale
 * @param other Référence au composant à connecter
 * @param otherPin Numéro de la pin du composant autre
 * 
 * Enregistre les connexions pour permettre la lecture des signaux
 * d'autres composants via les pins de la RAM.
 */
void Chip4801::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    _links[pin] = {&other, otherPin};
}

/**
 * @brief Récupère la valeur logique d'une pin d'entrée
 * @param pin Numéro de la pin à lire
 * @return Valeur tristate de la pin connectée (True, False ou Undefined)
 * 
 * Interroge le composant connecté à la pin spécifiée via le réseau de liens.
 */
nts::Tristate Chip4801::_getPinValue(std::size_t pin)
{
    if (_links.find(pin) == _links.end())
        return nts::Undefined;
    
    auto [component, otherPin] = _links[pin];
    return component->compute(otherPin);
}

/**
 * @brief Construit l'adresse 8 bits à partir des pins d'adresse
 * @return Adresse (0-255) composée des 8 bits d'adresse
 * 
 * Assemble les bits depuis les pins 8, 7, 6, 5, 4, 3, 2, 1
 * (addr_0 à addr_7 respectivement) pour former l'adresse mémoire.
 */
unsigned char Chip4801::_getAddress()
{
    unsigned char address = 0;
    
    std::array<std::size_t, 8> addressPins = {8, 7, 6, 5, 4, 3, 2, 1};
    
    for (int i = 0; i < 8; ++i) {
        nts::Tristate val = _getPinValue(addressPins[i]);
        if (val == nts::True)
            address |= (1 << i);
    }
    
    return address;
}

/**
 * @brief Construit l'octet de données 8 bits à partir des pins d'entrée
 * @return Octet de données (0-255) composé des 8 bits d'entrée
 * 
 * Assemble les bits depuis les pins 9, 10, 11, 13, 14, 15, 16, 17
 * (in_0 à in_7 respectivement) pour former l'octet à écrire en mémoire.
 */
unsigned char Chip4801::_getDataByte()
{
    unsigned char data = 0;
    
    std::array<std::size_t, 8> dataPins = {9, 10, 11, 13, 14, 15, 16, 17};
    
    for (int i = 0; i < 8; ++i) {
        nts::Tristate val = _getPinValue(dataPins[i]);
        if (val == nts::True)
            data |= (1 << i);
    }
    
    return data;
}

/**
 * @brief Met à jour les pins de sortie avec les bits d'un octet
 * @param data Octet (0-255) à décomposer en 8 bits
 * 
 * Décompose l'octet en ses 8 bits et met à jour les pins de sortie 
 * 9, 10, 11, 13, 14, 15, 16, 17 avec les différents bits de données.
 */
void Chip4801::_setDataByte(unsigned char data)
{
    std::array<std::size_t, 8> dataPins = {9, 10, 11, 13, 14, 15, 16, 17};
    
    for (int i = 0; i < 8; ++i)
        _pinValues[dataPins[i]] = (data & (1 << i)) ? nts::True : nts::False;
}

/**
 * @brief Affiche les informations du composant
 * @param os Flux de sortie (par défaut: std::cout)
 * 
 * Affiche le type de puce (RAM 256 bytes) et son nom.
 */
void Chip4801::display(std::ostream& os)
{
    os << "Chip4801 (RAM 256 bytes): " << _name << std::endl;
}
