#include "../../../include/Components/Chipset/4069.hpp"

/// @brief Constructeur du composant puce 4069 (portes NOT).
/// @param name Le nom du composant au sein du circuit.
nts::Chip4069::Chip4069(const std::string& name)
{
    (void)name;
}

/// @brief Simule un cycle du composant 4069.
/// @param tick Le numéro du tick de simulation courant.
/// @details Si le tick est différent du dernier tick enregistré, invalide complètement le cache.
///          Cela garantit que les calculs sont frais pour chaque nouveau cycle de simulation.
void nts::Chip4069::simulate(std::size_t tick)
{
    if (tick != _lastComputeTick) {
        _cache.clear();
        _lastComputeTick = tick;
    }
}

/// @brief Opération logique NOT pour une entrée Tristate.
/// @param a L'entrée de la porte NOT.
/// @return Le résultat de l'opération NOT :
///         - Tristate::False si a est True
///         - Tristate::True si a est False
///         - Tristate::Undefined sinon
/// @details Table de vérité NOT :
///          - NOT(F) = T
///          - NOT(T) = F
nts::Tristate nts::Chip4069::notGate(nts::Tristate a)
{
    if (a == nts::Tristate::True)
        return nts::Tristate::False;
    if (a == nts::Tristate::False)
        return nts::Tristate::True;
    return nts::Tristate::Undefined;
}

/// @brief Récupère la valeur d'entrée d'une broche connectée.
/// @param pin Le numéro de broche dont on veut récupérer l'entrée.
/// @return La valeur Tristate provenant du composant connecté à cette broche.
/// @retval Tristate::Undefined Si aucun composant n'est connecté à cette broche.
nts::Tristate nts::Chip4069::computeInput(std::size_t pin)
{
    if (_links.count(pin))
        return _links[pin].component->compute(_links[pin].pin);
    return nts::Tristate::Undefined;
}

/// @brief Calcule la valeur de sortie sur une broche donnée.
/// @param pin Le numéro de broche dont on veut connaître la valeur.
/// @return La valeur Tristate calculée pour cette broche :
///         - Pour les broches de masse (7) et alimentation (14) : Undefined
///         - Pour les broches de sortie (2, 4, 6, 8, 10, 12) : résultat de la porte NOT
///         - Pour les autres broches : Undefined
/// @details Imprime une sous-question avec le cache :
///          1. Si valeur en cache, la retourne immédiatement.
///          2. Marque la broche comme Undefined dans le cache pour casser les cycles.
///          3. Calcule la valeur réelle selon le mapping des broches.
///          4. Mémorise et retourne le résultat.
///          Mapping des broches (6 portes NOT) :
///          - Porte 1 : sortie 2 = NOT(1) - Porte 2 : sortie 4 = NOT(3)
///          - Porte 3 : sortie 6 = NOT(5) - Porte 4 : sortie 8 = NOT(9)
///          - Porte 5 : sortie 10 = NOT(11) - Porte 6 : sortie 12 = NOT(13)
nts::Tristate nts::Chip4069::compute(std::size_t pin)
{
    if (pin == 7 || pin == 14) 
        return nts::Tristate::Undefined;
    if (_cache.count(pin))
        return _cache[pin];
    _cache[pin] = nts::Tristate::Undefined;
    if (pin == 2)
        _cache[pin] = notGate(computeInput(1));
    else if (pin == 4)
        _cache[pin] = notGate(computeInput(3));
    else if (pin == 6)
        _cache[pin] = notGate(computeInput(5));
    else if (pin == 8)
        _cache[pin] = notGate(computeInput(9));
    else if (pin == 10)
        _cache[pin] = notGate(computeInput(11));
    else if (pin == 12)
        _cache[pin] = notGate(computeInput(13));
    else
        _cache[pin] = computeInput(pin);
    return _cache[pin];
}

/// @brief Établit une connexion entre deux broches de composants différents.
/// @param pin Le numéro de broche de ce composant.
/// @param other Référence au composant à connecter.
/// @param otherPin Le numéro de broche du composant à connecter.
void nts::Chip4069::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    _links[pin] = {&other, otherPin};
}
