#include "../../../include/Components/Adv_Chipset/4013.hpp"

/**
 * @brief Constructeur du double flip-flop D (4013)
 * @param name Nom du composant dans le circuit
 * 
 * Initialise les deux bascules D à l'état indéfini (Undefined).
 * Les clocks précédentes sont également initialisées à Undefined.
 */
Chip4013::Chip4013(const std::string& name) : _name(name), _Q1(nts::Undefined), _Q2(nts::Undefined),
    _prevClk1(nts::Undefined), _prevClk2(nts::Undefined)
{
}

/**
 * @brief Constructeur de copie
 * @param other Référence constante à l'objet à copier
 */
Chip4013::Chip4013(const Chip4013& other) : _name(other._name), _Q1(other._Q1), _Q2(other._Q2),
    _prevClk1(other._prevClk1), _prevClk2(other._prevClk2), _links(other._links),
    _pinValues(other._pinValues)
{
}

/**
 * @brief Opérateur d'assignation
 * @param other Référence constante à l'objet à assigner
 * @return Référence à l'objet courant
 */
Chip4013& Chip4013::operator=(const Chip4013& other)
{
    if (this != &other) {
        _name = other._name;
        _Q1 = other._Q1;
        _Q2 = other._Q2;
        _prevClk1 = other._prevClk1;
        _prevClk2 = other._prevClk2;
        _links = other._links;
        _pinValues = other._pinValues;
    }
    return *this;
}

/**
 * @brief Simule le comportement des deux bascules D à chaque cycle
 * @param tick Numéro du cycle de simulation
 * 
 * Traite indépendamment chaque bascule D :
 * 1. Vérifiie les signaux Set (preset) et Reset (clear) - prioritaires
 * 2. Si front montant de clock et aucun Set/Reset : capture la donnée D dans Q
 * 3. Met à jour les clocks précédentes pour la détection du front suivant
 */
void Chip4013::simulate(std::size_t tick)
{
    (void)tick;
    {
        nts::Tristate set1 = _getPinValue(3);
        nts::Tristate reset1 = _getPinValue(4);
        nts::Tristate clk1 = _getPinValue(1);
        
        if (set1 == nts::True) { _Q1 = nts::True; } 
        else if (reset1 == nts::True) { _Q1 = nts::False; }
        else if (_isRisingEdge(clk1, _prevClk1)) {
            nts::Tristate data1 = _getPinValue(2);
            _Q1 = data1;
        }
        _prevClk1 = clk1;
    }
    {
        nts::Tristate set2 = _getPinValue(13);
        nts::Tristate reset2 = _getPinValue(14);
        nts::Tristate clk2 = _getPinValue(11);
        
        if (set2 == nts::True) { _Q2 = nts::True; } 
        else if (reset2 == nts::True) { _Q2 = nts::False; }
        else if (_isRisingEdge(clk2, _prevClk2)) {
            nts::Tristate data2 = _getPinValue(12);
            _Q2 = data2;
        }
        _prevClk2 = clk2;
    }
    _pinValues[5] = _Q1;
    _pinValues[6] = (_Q1 == nts::Undefined) ? nts::Undefined : (_Q1 == nts::True ? nts::False : nts::True);
    _pinValues[9] = _Q2;
    _pinValues[8] = (_Q2 == nts::Undefined) ? nts::Undefined : (_Q2 == nts::True ? nts::False : nts::True);
}

/**
 * @brief Détecte un front montant du signal d'horloge
 * @param currentClk Valeur courante de l'horloge
 * @param prevClk Valeur précédente de l'horloge
 * @return true si transition de False/Undefined à True, false sinon
 */
bool Chip4013::_isRisingEdge(nts::Tristate currentClk, nts::Tristate prevClk)
{
    return (currentClk == nts::True) && (prevClk != nts::True);
}

/**
 * @brief Retourne la valeur logique d'une pin de sortie
 * @param pin Numéro de la pin à lire
 * @return Valeur tristate (True, False ou Undefined)
 */
nts::Tristate Chip4013::compute(std::size_t pin)
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
void Chip4013::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    _links[pin] = {&other, otherPin};
}

/**
 * @brief Récupère la valeur logique d'une pin d'entrée
 * @param pin Numéro de la pin à lire
 * @return Valeur tristate de la pin connectée
 */
nts::Tristate Chip4013::_getPinValue(std::size_t pin)
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
void Chip4013::display(std::ostream& os) const
{
    os << "Chip4013 (Dual D-type Flip-Flop): " << _name 
       << " (Q1=" << (_Q1 == nts::True ? "1" : _Q1 == nts::False ? "0" : "?")
       << ", Q2=" << (_Q2 == nts::True ? "1" : _Q2 == nts::False ? "0" : "?") << ")" << std::endl;
}

std::ostream& operator<<(std::ostream& os, const Chip4013& chip)
{
    chip.display(os);
    return os;
}
