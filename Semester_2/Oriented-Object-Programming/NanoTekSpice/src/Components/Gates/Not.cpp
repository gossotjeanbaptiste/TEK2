#include "../../../include/Components/Gates/Not.hpp"

/**
 * @brief Constructeur pour la porte NOT
 * @param name L'identifiant de nom pour cette porte NOT
 * 
 * Initialise une porte NOT avec un nom unique. La porte NOT a une entree (broche 1)
 * et une sortie (broche 2). La sortie est initialement indefinie jusqu'a ce que le circuit soit simule.
 */
Not::Not(const std::string& name) : _name(name)
{
    // Constructor - gate is ready to connect
}

/**
 * @brief Constructeur de copie pour la porte NOT
 * @param other La porte NOT a copier
 * 
 * Cree une copie profonde d'une autre porte NOT, en copiant tout l'etat,
 * y compris la reference du composant d'entree lie et la valeur de sortie actuelle.
 */
Not::Not(const Not& other) : _name(other._name),
    _input(other._input), _inputPin(other._inputPin),
    _output(other._output)
{
    // Copy constructor
}

/**
 * @brief Operateur d'affectation pour la porte NOT
 * @param other La porte NOT a copier
 * @return Reference a cette porte NOT apres l'affectation
 * 
 * Assigne surement l'etat d'une autre porte NOT a celle-ci,
 * y compris le lien d'entree et la valeur de sortie.
 */
Not& Not::operator=(const Not& other)
{
    if (this != &other) {
        _name = other._name;
        _input = other._input;
        _inputPin = other._inputPin;
        _output = other._output;
    }
    return *this;
}

/**
 * @brief Simule un tick d'horloge
 * @param tick Le numero de tick actuel
 * 
 * Appele pendant la simulation. Calcule l'etat de sortie actuel en fonction
 * de la valeur d'entree en appliquant l'operation de porte logique NOT.
 * L'operation NOT inverse l'entree :
 *   - True -> False
 *   - False -> True
 *   - Undefined -> Undefined
 */
void Not::simulate(std::size_t tick)
{
    (void)tick;
    if (_input) {
        nts::Tristate in = _input->compute(_inputPin);
        if (in == nts::True) { _output = nts::False; }
        else if (in == nts::False) { _output = nts::True; }
        else { _output = nts::Undefined; }
    }
}

/**
 * @brief Obtient la valeur de sortie a une broche donnee
 * @param pin Le numero de broche a lire (1=entree, 2=sortie)
 * @return La valeur Tristate a la broche demandee
 * 
 * Retourne la valeur a une broche specifique :
 *   - Broche 1 : Broche d'entree (retournerait la valeur d'entree)
 *   - Broche 2 : Broche de sortie (retourne la sortie calculee actuelle)
 *   - Autres broches : Retourne Undefined
 */
nts::Tristate Not::compute(std::size_t pin)
{
    if (pin == 2) { return _output; }
    return nts::Undefined;
}

/**
 * @brief Cree un lien vers un autre composant
 * @param pin La broche de cette porte NOT a lier (1=entree, 2=sortie)
 * @param other Le composant a lier
 * @param otherPin La broche de l'autre composant
 * 
 * Etablit une connexion entre cette porte et un autre composant.
 * La broche 1 est la broche d'entree - nous stockons la reference a introduire dans notre logique.
 * La broche 2 est la broche de sortie - rien a stocker (la sortie va VERS d'autres composants).
 */
void Not::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    if (pin == 1) {
        _input = &other;
        _inputPin = otherPin;
    }
}

/**
 * @brief Affiche les informations de la porte
 * @param os Flux de sortie dans lequel ecrire (par defaut : std::cout)
 * 
 * Affiche une representation lisible par l'homme de l'etat actuel de la porte NOT.
 */
void Not::display(std::ostream& os)
{
    os << "NOT gate: " << _name << " (output=" << _output << ")";
}