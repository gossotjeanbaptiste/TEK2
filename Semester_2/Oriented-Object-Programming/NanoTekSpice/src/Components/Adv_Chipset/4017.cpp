#include "../../../include/Components/Adv_Chipset/4017.hpp"

/**
 * @brief Constructeur du compteur décimal 4017
 * @param name Nom du composant
 */
Chip4017::Chip4017(const std::string& name) : _name(name)
{
}

/**
 * @brief Constructeur de copie
 * @param other Référence constante à l'objet à copier
 */
Chip4017::Chip4017(const Chip4017& other) : AComponent(other), _name(other._name), _counter(other._counter)
{
}

/**
 * @brief Opérateur d'assignation
 * @param other Référence constante à l'objet à assigner
 * @return Référence à l'objet courant
 */
Chip4017& Chip4017::operator=(const Chip4017& other)
{
    if (this != &other) {
        AComponent::operator=(other);
        _name = other._name;
        _counter = other._counter;
    }
    return *this;
}

/**
 * @brief Simule le comportement du compteur à chaque cycle
 * @param tick Numéro du cycle de simulation
 * 
 * Incrémente le compteur (0-9) ou le réinitialise selon les signaux de contrôle.
 * Une seule sortie parmi les 10 est active (True) selon la valeur du compteur.
 */
void Chip4017::simulate(std::size_t tick)
{
    (void)tick;
    nts::Tristate reset = _getPinValue(15);

    if (reset == nts::True) { _counter = 0; }
    else { _counter = (_counter + 1) % 10; }
    
    static const std::array<std::size_t, 10> outputPins = {3, 2, 4, 7, 10, 1, 5, 6, 9, 11};    
    for (int i = 0; i < 10; ++i)
        _pinValues[outputPins[i]] = (i == _counter) ? nts::True : nts::False;
    _pinValues[12] = (_counter >= 5) ? nts::True : nts::False;
}

/**
 * @brief Retourne la valeur logique d'une pin de sortie
 * @param pin Numéro de la pin à lire
 * @return Valeur tristate (True, False ou Undefined)
 */
nts::Tristate Chip4017::compute(std::size_t pin)
{
    if (_pinValues.find(pin) != _pinValues.end())
        return _pinValues[pin];
    return nts::Undefined;
}

/**
 * @brief Établit une connexion entre une pin et un autre composant
 * @param pin Numéro de la pin locale
 * @param other Référence au composant à connecter
 * @param otherPin Numéro de la pin du composant autre
 */
void Chip4017::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    _links[pin] = {&other, otherPin};
}

/**
 * @brief Récupère la valeur logique d'une pin d'entrée
 * @param pin Numéro de la pin à lire
 * @return Valeur tristate de la pin connectée
 */
nts::Tristate Chip4017::_getPinValue(std::size_t pin)
{
    if (_links.find(pin) == _links.end())
        return nts::Undefined;
    auto [component, otherPin] = _links[pin];
    return component->compute(otherPin);
}

/**
 * @brief Affiche les informations du composant
 * @param os Flux de sortie (par défaut: std::cout)
 */
void Chip4017::display(std::ostream& os)
{
    os << "Chip4017 (Decimal Counter): " << _name << " (count: " << static_cast<int>(_counter) << ")" << std::endl;
}
