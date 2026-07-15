#include "../../../include/Components/Special/Input.hpp"

/**
 * @brief Constructeur du composant Input
 * @param name Nom du composant Input
 */
Input::Input(const std::string& name) : _name(name)
{
    // Constructor
}

/**
 * @brief Constructeur de copie du composant Input
 * @param other Instance Input à copier
 */
Input::Input(const Input& other) : _name(other._name), _value(other._value)
{
    // Copy constructor
}

/**
 * @brief Opérateur d'assignation du composant Input
 * 
 * Effectue une copie profonde de toutes les variables membres d'une autre
 * instance Input vers cette instance.
 * 
 * @param other L'instance Input à copier
 * @return Une référence à cette instance Input (*this) pour permettre le chaînage
 * 
 * @note Cet opérateur vérifie l'auto-assignation avant de copier
 */
Input& Input::operator=(const Input& other)
{
    if (this != &other) {
        _name = other._name;
        _value = other._value;
    }
    return *this;
}

/**
 * @brief Simule le composant Input pour un tick d'horloge donné
 * 
 * Le composant Input est un composant passif qui ne fait aucune simulation.
 * Il détient simplement une valeur qui peut être définie de manière externe
 * via la méthode setValue().
 * 
 * @param tick Le numéro du tick de simulation (non utilisé)
 * 
 * @return void
 * 
 * @note Cette méthode est une non-opération pour les composants Input
 */
void Input::simulate(std::size_t tick)
{
    (void)tick;
    // Inputs don't need to compute anything
}

/**
 * @brief Établit une liaison entre ce composant Input et un autre composant
 * 
 * Les composants Input sont des composants source et n'acceptent pas de liaisons
 * entrantes. Cette méthode est une non-opération car les entrées ne reçoivent pas
 * de connexions d'autres composants.
 * 
 * @param pin Le numéro de la pin (non utilisé pour les composants Input)
 * @param other Une référence au composant à lier (non utilisée)
 * @param otherPin Le numéro de la pin du composant distant (non utilisé)
 * 
 * @note Les composants Input sont des points terminaux du circuit et ne peuvent pas
 *       recevoir d'entrées
 */
void Input::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    (void)pin;
    (void)other;
    (void)otherPin;
    // Input components don't accept incoming links
}

/**
 * @brief Calcule l'état de sortie du composant Input
 * 
 * Retourne la valeur actuelle détenue par ce composant Input.
 * Le composant Input n'a qu'une seule pin (pin 1) qui fournit la valeur stockée.
 * 
 * @param pin Le numéro de la pin pour laquelle calculer la sortie
 *            La pin 1 est la seule pin de sortie pour les composants Input
 * 
 * @return nts::Tristate La valeur actuelle stockée dans ce Input
 *                       Retourne _value indépendamment de la pin demandée
 *                       La valeur initiale est nts::Undefined
 * 
 * @note Les composants Input ignorent le paramètre pin et retournent toujours
 *       leur valeur stockée
 * @note La valeur peut être définie avec la méthode setValue()
 */
nts::Tristate Input::compute(std::size_t pin)
{
    (void)pin;
    return _value;
}

/**
 * @brief Affiche les informations du composant Input
 * @param os Flux de sortie pour l'affichage
 */
void Input::display(std::ostream& os)
{
    os << "Input: " << _name << " (value=" << _value << ")";
}
