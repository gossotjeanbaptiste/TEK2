#include "../../../include/Components/Gates/Xor.hpp"

Xor::Xor(const std::string& name) : _name(name)
{
    // Constructor
}

Xor::Xor(const Xor& other) : _name(other._name),
    _input1(other._input1), _input2(other._input2),
    _input1Pin(other._input1Pin), _input2Pin(other._input2Pin),
    _output(other._output)
{
    // Copy constructor
}

/**
 * @brief Operateur d'affectation pour le composant porte XOR.
 * 
 * Effectue une copie profonde de toutes les variables membres d'une autre instance Xor
 * vers cette instance.
 * 
 * @param other L'instance Xor a copier.
 * @return Une reference a cette instance Xor (*this) pour permettre le chainace des affectations.
 * 
 * @note Cet operateur verifie l'auto-affectation avant de copier.
 */
Xor& Xor::operator=(const Xor& other)
{
    if (this != &other) {
        _name = other._name;
        _input1 = other._input1;
        _input2 = other._input2;
        _input1Pin = other._input1Pin;
        _input2Pin = other._input2Pin;
        _output = other._output;
    }
    return *this;
}


/**
 * @brief Simule la logique de la porte XOR pour un tick d'horloge donne.
 * 
 * Calcule la sortie d'une porte XOR a deux entrees en fonction de l'etat actuel de ses entrees.
 * La simulation suit la table de verite standard de la porte XOR avec support de la logique
 * a trois etats (True, False, Undefined).
 * 
 * Table de verite :
 * - True XOR True = False (entrees identiques)
 * - True XOR False = True (entrees differentes)
 * - True XOR Undefined = Undefined
 * - False XOR False = False (entrees identiques)
 * - False XOR True = True (entrees differentes)
 * - False XOR Undefined = Undefined
 * - Undefined XOR True = Undefined
 * - Undefined XOR False = Undefined
 * - Undefined XOR Undefined = Undefined
 * 
 * @param tick Le numero de tick de simulation (inutilise dans cette implementation).
 * 
 * @return void
 * 
 * @note Met a jour la variable membre interne _output avec le resultat calcule.
 * @note La methode gere surement les references d'entree nulles avant le calcul.
 */
void Xor::simulate(std::size_t tick)
{
    (void)tick;
    if (_input1 && _input2) {
        nts::Tristate in1 = _input1->compute(_input1Pin);
        nts::Tristate in2 = _input2->compute(_input2Pin);
        if (in1 == nts::Undefined || in2 == nts::Undefined) { _output = nts::Undefined; }
        else if (in1 == in2) { _output = nts::False; } 
        else { _output = nts::True; }
    }
}

/**
 * @brief Calcule l'etat de sortie de la porte XOR.
 * 
 * @param pin Le numero de broche pour lequel calculer la sortie.
 *            Les broches 1 et 2 sont des broches d'entree.
 *            La broche 3 est la broche de sortie.
 * 
 * @return nts::Tristate L'etat de la broche demandee.
 *                       - Retourne la sortie calculee de la porte XOR si la broche est 3.
 *                       - Retourne nts::Undefined pour les numeros de broche invalides.
 */
nts::Tristate Xor::compute(std::size_t pin)
{
    if (pin == 3) { return _output; }
    return nts::Undefined;
}

/**
 * @brief Etablit le lien entre ce composant porte XOR et un autre composant.
 * 
 * Cette methode etablit les connexions pour les broches d'entree de la porte XOR.
 * Les broches 1 et 2 sont des broches d'entree, tandis que la broche 3 est la broche de sortie.
 * 
 * @param pin Le numero de broche de cette porte XOR (1 ou 2 pour les entrees, 3 pour la sortie).
 * @param other Une reference au composant a lier.
 * @param otherPin Le numero de broche de l'autre composant a lier.
 * 
 * @note Les connexions de la broche 3 (sortie) ne sont pas stockees car cette methode
 *       gere uniquement les connexions entrantes aux broches d'entree.
 * @note Seules les broches 1 et 2 (entrees) sont traitees. La broche 3 (sortie) est ignoree.
 */
void Xor::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    if (pin == 1) {
        _input1 = &other;
        _input1Pin = otherPin;
    } else if (pin == 2) {
        _input2 = &other;
        _input2Pin = otherPin;
    }
}

void Xor::display(std::ostream& os)
{
    os << "XOR gate: " << _name << " (output=" << _output << ")";
}