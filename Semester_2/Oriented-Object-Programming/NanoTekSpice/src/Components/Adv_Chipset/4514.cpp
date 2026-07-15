#include "../../../include/Components/Adv_Chipset/4514.hpp"

/**
 * @brief Constructeur du décodeur 4514
 * @param name Nom du composant
 */
Chip4514::Chip4514(const std::string& name) : _name(name)
{
}

/**
 * @brief Constructeur de copie
 * @param other Référence constante à l'objet à copier
 */
Chip4514::Chip4514(const Chip4514& other) : AComponent(other), _name(other._name)
{
}

/**
 * @brief Opérateur d'assignation
 * @param other Référence constante à l'objet à assigner
 * @return Référence à l'objet courant
 */
Chip4514& Chip4514::operator=(const Chip4514& other)
{
    if (this != &other) {
        AComponent::operator=(other);
        _name = other._name;
    }
    return *this;
}

/**
 * @brief Simule le comportement du décodeur à chaque cycle
 * @param tick Numéro du cycle de simulation
 * 
 * Effectue la décision:
 * - Lit les 4 bits d'adresse
 * - Sélectionne la sortie correspondante
 * - Active le signal inhibit si nécessaire
 */
void Chip4514::simulate(std::size_t tick)
{
    (void)tick;
    nts::Tristate inhibit = _getPinValue(23);
    if (inhibit == nts::True) {
        for (int i = 0; i < 16; ++i)
            _pinValues[i] = nts::False;
        return;
    }

    unsigned char address = _getAddress();
    for (int i = 0; i < 16; ++i)
        _pinValues[i] = (i == address) ? nts::True : nts::False;
}

/**
 * @brief Retourne la valeur logique d'une pin de sortie
 * @param pin Numéro de la pin à lire
 * @return Valeur tristate (True, False ou Undefined)
 */
nts::Tristate Chip4514::compute(std::size_t pin)
{
    static const std::map<std::size_t, int> pinToOutput = {
        {11, 0},  {9, 1},   {10, 2},  {8, 3},
        {7, 4},   {6, 5},   {5, 6},   {4, 7},
        {18, 8},  {17, 9},  {20, 10}, {19, 11},
        {14, 12}, {13, 13}, {16, 14}, {15, 15}
    };
    auto it = pinToOutput.find(pin);

    if (it != pinToOutput.end()) {
        int outputIndex = it->second;
        if (_pinValues.find(outputIndex) != _pinValues.end())
            return _pinValues[outputIndex];
    }
    return nts::Undefined;
}

/**
 * @brief Établit une connexion entre une pin et un autre composant
 * @param pin Numéro de la pin locale
 * @param other Référence au composant à connecter
 * @param otherPin Numéro de la pin du composant autre
 */
void Chip4514::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    _links[pin] = {&other, otherPin};
}

/**
 * @brief Récupère la valeur logique d'une pin d'entrée
 * @param pin Numéro de la pin à lire
 * @return Valeur tristate de la pin connectée
 */
nts::Tristate Chip4514::_getPinValue(std::size_t pin)
{
    if (_links.find(pin) == _links.end()) { return nts::Undefined; }
    auto [component, otherPin] = _links[pin];
    return component->compute(otherPin);
}

/**
 * @brief Construit l'adresse 4 bits à partir des pins d'adresse
 * @return Adresse (0-15) composée des 4 bits d'adresse
 * 
 * Assemble les bits depuis les pins 2, 3, 21, 22
 * (in_0, in_1, in_2, in_3 respectivement) pour former l'adresse.
 */
unsigned char Chip4514::_getAddress()
{
    unsigned char address = 0;
    std::array<std::size_t, 4> addressPins = {2, 3, 21, 22};
    
    for (int i = 0; i < 4; ++i) {
        nts::Tristate val = _getPinValue(addressPins[i]);
        if (val == nts::True)
            address |= (1 << i);
    }
    
    return address;
}

/**
 * @brief Affiche les informations du composant
 * @param os Flux de sortie (par défaut: std::cout)
 */
void Chip4514::display(std::ostream& os)
{
    os << "Chip4514 (Decoder 4-to-16): " << _name << std::endl;
}
