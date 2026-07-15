#include "../../../include/Components/Adv_Chipset/4512.hpp"

/**
 * @brief Constructeur du multiplexeur 4512
 * @param name Nom du composant
 */
Chip4512::Chip4512(const std::string& name) : _name(name)
{
}

/**
 * @brief Constructeur de copie
 * @param other Référence constante à l'objet à copier
 */
Chip4512::Chip4512(const Chip4512& other) : AComponent(other), _name(other._name), _outputValue(other._outputValue)
{
}

/**
 * @brief Opérateur d'assignation
 * @param other Référence constante à l'objet à assigner
 * @return Référence à l'objet courant
 */
Chip4512& Chip4512::operator=(const Chip4512& other)
{
    if (this != &other) {
        AComponent::operator=(other);
        _name = other._name;
        _outputValue = other._outputValue;
    }
    return *this;
}

/**
 * @brief Simule le comportement du multiplexeur à chaque cycle
 * @param tick Numéro du cycle de simulation
 * 
 * Sélectionne une entrée parmi 8 selon le code 3 bits (in_a, in_b, in_c).
 * Si inhibit = 1 ou enable = 0, la sortie est False.
 */
void Chip4512::simulate(std::size_t tick)
{
    (void)tick;
    
    nts::Tristate inhibit = _getPinValue(10);
    nts::Tristate enable = _getPinValue(15);
    
    if (inhibit == nts::True || enable == nts::False) {
        _outputValue = nts::False;
        return;
    }
    
    unsigned char selectCode = _getSelectCode();
    _outputValue = _getDataInput(selectCode);
}

/**
 * @brief Retourne la valeur logique d'une pin de sortie
 * @param pin Numéro de la pin à lire
 * @return Valeur tristate (True, False ou Undefined)
 */
nts::Tristate Chip4512::compute(std::size_t pin)
{
    if (pin == 14) {
        return _outputValue;
    }
    return nts::Undefined;
}

/**
 * @brief Établit une connexion entre une pin et un autre composant
 * @param pin Numéro de la pin locale
 * @param other Référence au composant à connecter
 * @param otherPin Numéro de la pin du composant autre
 */
void Chip4512::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    _links[pin] = {&other, otherPin};
}

/**
 * @brief Récupère la valeur logique d'une pin d'entrée
 * @param pin Numéro de la pin à lire
 * @return Valeur tristate de la pin connectée
 */
nts::Tristate Chip4512::_getPinValue(std::size_t pin)
{
    if (_links.find(pin) == _links.end())
        return nts::Undefined;
    
    auto [component, otherPin] = _links[pin];
    return component->compute(otherPin);
}

/**
 * @brief Construit le code de sélection 3 bits
 * @return Adresse (0-7) formée par in_a, in_b, in_c
 * 
 * - Pin 11 (in_a) = bit 0
 * - Pin 12 (in_b) = bit 1
 * - Pin 13 (in_c) = bit 2
 */
unsigned char Chip4512::_getSelectCode()
{
    unsigned char code = 0;
    
    std::array<std::size_t, 3> selectPins = {11, 12, 13};
    
    for (int i = 0; i < 3; ++i) {
        nts::Tristate val = _getPinValue(selectPins[i]);
        if (val == nts::True) code |= (1 << i);
    }
    
    return code;
}

/**
 * @brief Récupère l'entrée de données sélectionnée
 * @param index Index de l'entrée (0-7)
 * @return Valeur de l'entrée de données sélectionnée
 */
nts::Tristate Chip4512::_getDataInput(unsigned char index)
{
    static const std::array<std::size_t, 8> dataPins = {1, 2, 3, 4, 5, 6, 7, 9};
    
    if (index >= 8) { return nts::Undefined; }
    return _getPinValue(dataPins[index]);
}

/**
 * @brief Affiche les informations du composant
 * @param os Flux de sortie (par défaut: std::cout)
 */
void Chip4512::display(std::ostream& os)
{
    os << "Chip4512 (Multiplexer 8-to-1): " << _name << std::endl;
}
