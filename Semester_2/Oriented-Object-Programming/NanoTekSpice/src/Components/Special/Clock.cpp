#include "../../../include/Components/Special/Clock.hpp"

/// @brief Constructeur du composant Clock.
/// @param name Le nom du composant horloge.
Clock::Clock(const std::string& name) : _name(name)
{
}

/// @brief Constructeur de copie du composant Clock.
/// @param other Instance Clock à copier.
Clock::Clock(const Clock& other) : _name(other._name), _value(other._value)
{
    // Copy constructor
}

/// @brief Opérateur d'assignation du composant Clock.
/// @param other L'instance Clock à copier.
/// @return Une référence à cette instance Clock (*this).
Clock& Clock::operator=(const Clock& other)
{
    if (this != &other) {
        _name = other._name;
        _value = other._value;
    }
    return *this; // * référence a cette objet
}

/// @brief Simule un cycle du composant Clock.
/// @param tick Le numéro du tick de simulation courant.
/// @details Alterne la valeur du signal d'horloge à chaque appel.
///          - Tick pair : True
///          - Tick impair : False
void Clock::simulate(std::size_t tick)
{
    if (tick % 2 == 0)
        _value = nts::True;
    else
        _value = nts::False;
}

/// @brief Calcule la valeur du signal d'horloge.
/// @param pin Le numéro de broche (non utilisé pour un Clock).
/// @return La valeur Tristate actuelle du signal d'horloge (True ou False).
nts::Tristate Clock::compute(std::size_t pin)
{
    (void)pin;
    return _value;
}

/// @brief Établit une connexion (non fonctionnelle pour Clock).
/// @param pin Le numéro de broche de ce composant.
/// @param other Référence au composant à connecter.
/// @param otherPin Le numéro de broche du composant à connecter.
/// @note Le Clock est une source autonome et ne nécessite pas de connexions entrantes.
void Clock::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    (void)pin;
    (void)other;
    (void)otherPin;
    // To implement
}

void Clock::display(std::ostream& os)
{
    (void)os;
    // Display action
}