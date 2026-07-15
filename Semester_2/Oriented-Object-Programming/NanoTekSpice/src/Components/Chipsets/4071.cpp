#include "../../../include/Components/Chipset/4071.hpp"

/// @brief Constructeur du composant puce 4071 (portes OR).
/// @param name Le nom du composant au sein du circuit.
nts::Chip4071::Chip4071(const std::string& name)
{
    (void)name;
}
/// @brief Simule un cycle du composant 4071.
/// @param tick Le numéro du tick de simulation courant.
/// @details Si le tick est différent du dernier tick enregistré, invalide complètement le cache.
///          Cela garantit que les calculs sont frais pour chaque nouveau cycle de simulation.
void nts::Chip4071::simulate(std::size_t tick)
{
    if (tick != _lastComputeTick) {
        _cache.clear();
        _lastComputeTick = tick;
    }
}

/// @brief Opération logique OR pour deux entrées Tristate.
/// @param a La première entrée de la porte OR.
/// @param b La deuxième entrée de la porte OR.
/// @return Le résultat de l'opération OR :
///         - Tristate::True si a OU b est True
///         - Tristate::False si a ET b sont False
///         - Tristate::Undefined sinon
/// @details Table de vérité OR :
///          - OR(F, F) = F | OR(T, T) = T
///          - OR(T, F) = T | OR(F, T) = T
nts::Tristate nts::Chip4071::orGate(nts::Tristate a, nts::Tristate b)
{
    if (a == nts::Tristate::True || b == nts::Tristate::True)
        return nts::Tristate::True;
    if (a == nts::Tristate::False && b == nts::Tristate::False)
        return nts::Tristate::False;
    return nts::Tristate::Undefined;
}

/// @brief Récupère la valeur d'entrée d'une broche connectée.
/// @param pin Le numéro de broche dont on veut récupérer l'entrée.
/// @return La valeur Tristate provenant du composant connecté à cette broche.
/// @retval Tristate::Undefined Si aucun composant n'est connecté à cette broche.
nts::Tristate nts::Chip4071::computeInput(std::size_t pin)
{
    if (_links.count(pin))
        return _links[pin].component->compute(_links[pin].pin);
    return nts::Tristate::Undefined;
}

/// @brief Calcule la valeur de sortie sur une broche donnée.
/// @param pin Le numéro de broche dont on veut connaître la valeur.
/// @return La valeur Tristate calculée pour cette broche :
///         - Pour les broches de masse (7) et alimentation (14) : Undefined
///         - Pour les broches de sortie (3, 4, 10, 11) : résultat de la porte OR
///         - Pour les autres broches : Undefined
/// @details Imprime une sous-question avec le cache :
///          1. Si valeur en cache, la retourne immédiatement.
///          2. Marque la broche comme Undefined dans le cache pour casser les cycles.
///          3. Calcule la valeur réelle selon le mapping des broches.
///          4. Mémorise et retourne le résultat.
///          Mapping des broches :
///          - Porte 1 : sortie 3 = OR(1, 2)
///          - Porte 2 : sortie 4 = OR(5, 6)
///          - Porte 3 : sortie 10 = OR(8, 9)
///          - Porte 4 : sortie 11 = OR(12, 13)
nts::Tristate nts::Chip4071::compute(std::size_t pin)
{
    if (pin == 7 || pin == 14)
        return nts::Tristate::Undefined;
    if (_cache.count(pin))
        return _cache[pin];
    _cache[pin] = nts::Tristate::Undefined;
    if (pin == 3)
        _cache[pin] = orGate(computeInput(1), computeInput(2));
    else if (pin == 4)
        _cache[pin] = orGate(computeInput(5), computeInput(6));
    else if (pin == 10)
        _cache[pin] = orGate(computeInput(8), computeInput(9));
    else if (pin == 11)
        _cache[pin] = orGate(computeInput(12), computeInput(13));
    else
        _cache[pin] = nts::Tristate::Undefined;
    return _cache[pin];
}

/// @brief Établit une connexion entre deux broches de composants différents.
/// @param pin Le numéro de broche de ce composant.
/// @param other Référence au composant à connecter.
/// @param otherPin Le numéro de broche du composant à connecter.
void nts::Chip4071::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    _links[pin] = {&other, otherPin};
}