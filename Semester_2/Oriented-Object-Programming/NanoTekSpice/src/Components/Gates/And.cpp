#include "../../../include/Components/Gates/And.hpp"

And::And(const std::string& name) : _name(name)
{
    // Constructor
}

And::And(const And& other) : _name(other._name),
    _input1(other._input1), _input2(other._input2),
    _input1Pin(other._input1Pin), _input2Pin(other._input2Pin),
    _output(other._output)
{
    // Copy constructor
}

/**
 * @brief Operateur d'affectation pour le composant porte AND.
 * 
 * Effectue une copie profonde de toutes les variables membres d'une autre instance And
 * vers cette instance. Utilise l'idiome copy-and-swap pour assurer la securite des exceptions.
 * 
 * @param other L'instance And a copier.
 * @return Une reference a cette instance And (*this) pour permettre le chainace des affectations.
 * 
 * @note Cet operateur verifie l'auto-affectation avant de copier pour eviter
 *       les travaux inutiles et les problemes potentiels d'auto-affectation.
 * 
 * @details Les membres copies incluent :
 *          - _name: Le nom du composant
 *          - _input1, _input2: Les premieres et deuxiemes valeurs d'entree
 *          - _input1Pin, _input2Pin: Les references des premieres et deuxiemes broches d'entree
 *          - _output: La valeur de sortie
 */
And& And::operator=(const And& other)
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
 * @brief Simule la logique de la porte AND pour un tick d'horloge donne.
 * 
 * Calcule la sortie d'une porte AND a deux entrees en fonction de l'etat actuel de ses entrees.
 * La simulation suit la table de verite standard de la porte AND avec support de la logique
 * a trois etats (True, False, Undefined).
 * 
 * Table de verite :
 * - True ET True = True
 * - True ET False = False
 * - True ET Undefined = Undefined
 * - False ET n'importe quoi = False
 * - Undefined ET True = Undefined
 * - Undefined ET False = False
 * - Undefined ET Undefined = Undefined
 * 
 * @param tick Le numero de tick de simulation (inutilise dans cette implementation).
 * 
 * @return void
 * 
 * @note Met a jour la variable membre interne _output avec le resultat calcule.
 * @note La methode gere surement les references d'entree nulles avant le calcul.
 */
void And::simulate(std::size_t tick)
{
    (void)tick;
    if (_input1 && _input2) {
        nts::Tristate in1 = _input1->compute(_input1Pin);
        nts::Tristate in2 = _input2->compute(_input2Pin);
        if (in1 == nts::False || in2 == nts::False)
            _output = nts::False;
        else if (in1 == nts::Undefined || in2 == nts::Undefined)
            _output = nts::Undefined;
        else
            _output = nts::True;
    }
}

/**
 * @brief Calcule l'etat de sortie de la porte AND.
 * 
 * @param pin Le numero de broche pour lequel calculer la sortie.
 *            Les broches 1 et 2 sont des broches d'entree.
 *            La broche 3 est la broche de sortie.
 * 
 * @return nts::Tristate L'etat de la broche demandee.
 *                       - Retourne la sortie calculee de la porte AND si la broche est 3.
 *                       - Retourne nts::Undefined pour les numeros de broche invalides.
 */
nts::Tristate And::compute(std::size_t pin)
{
    // Pin 1 and 2 are inputs, pin 3 is output
    if (pin == 3)
        return _output;
    return nts::Undefined;
}

/**
 * @brief Etablit le lien entre ce composant porte AND et un autre composant.
 * 
 * Cette methode etablit les connexions pour les broches d'entree de la porte AND.
 * Les broches 1 et 2 sont des broches d'entree, tandis que la broche 3 est la broche de sortie.
 * 
 * @param pin Le numero de broche de cette porte AND (1 ou 2 pour les entrees, 3 pour la sortie).
 * @param other Une reference au composant a lier.
 * @param otherPin Le numero de broche de l'autre composant a lier.
 * 
 * @note Les connexions de la broche 3 (sortie) ne sont pas stockees car cette methode
 *       gere uniquement les connexions entrantes aux broches d'entree.
 * @note Seules les broches 1 et 2 (entrees) sont traitees. La broche 3 (sortie) est ignoree.
 */
void And::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    // Pin 1 and 2 are inputs, pin 3 is output
    if (pin == 1) {
        _input1 = &other;
        _input1Pin = otherPin;
    } else if (pin == 2) {
        _input2 = &other;
        _input2Pin = otherPin;
    }
    // Pin 3 is output, nothing to store for inputs
}

void And::display(std::ostream& os)
{
    os << "And gate: " << _name << " (output=" << _output << ")";
}
