#include "../../../include/Components/Chipset/4030.hpp"

/// @brief Constructeur du composant puce 4030 (portes XOR).
/// @param name Le nom du composant au sein du circuit.
nts::Chip4030::Chip4030(const std::string& name)
{
    (void)name;
}
/// @brief Simule un cycle du composant 4030.
/// @param tick Le numéro du tick de simulation courant.
/// @details Si le tick est différent du dernier tick enregistré, invalide complètement le cache.
///          Cela garantit que les calculs sont frais pour chaque nouveau cycle de simulation.
void nts::Chip4030::simulate(std::size_t tick)
{
    if (tick != _lastComputeTick) {
        _cache.clear();
        _lastComputeTick = tick;
    }
}

/// @brief Opération logique XOR pour deux entrées Tristate.
/// @param a La première entrée de la porte XOR.
/// @param b La deuxième entrée de la porte XOR.
/// @return Le résultat de l'opération XOR :
///         - Tristate::True si a != b
///         - Tristate::False si a == b
///         - Tristate::Undefined sinon
/// @details Table de vérité XOR :
///          - XOR(F, F) = F | XOR(T, T) = F
///          - XOR(T, F) = T | XOR(F, T) = T
nts::Tristate nts::Chip4030::xorGate(nts::Tristate a, nts::Tristate b)
{
    if (a == nts::Tristate::Undefined || b == nts::Tristate::Undefined)
        return nts::Tristate::Undefined;
    if (a != b)
        return nts::Tristate::True;
    return nts::Tristate::False;
}

/// @brief Récupère la valeur d'entrée d'une broche connectée.
/// @param pin Le numéro de broche dont on veut récupérer l'entrée.
/// @return La valeur Tristate provenant du composant connecté à cette broche.
/// @retval Tristate::Undefined Si aucun composant n'est connecté à cette broche.
nts::Tristate nts::Chip4030::computeInput(std::size_t pin)
{
    if (_links.count(pin)) {
        return _links[pin].component->compute(_links[pin].pin);
    }
    return nts::Tristate::Undefined;
}

/// @brief Calcule la valeur de sortie sur une broche donnée.
/// @param pin Le numéro de broche dont on veut connaître la valeur.
/// @return La valeur Tristate calculée pour cette broche :
///         - Pour les broches de masse (7) et alimentation (14) : Undefined
///         - Pour les broches de sortie (3, 4, 10, 11) : résultat de la porte XOR
///         - Pour les autres broches : Undefined
/// @details Imprime une sous-question avec le cache :
///          1. Si valeur en cache, la retourne immédiatement.
///          2. Marque la broche comme Undefined dans le cache pour casser les cycles.
///          3. Calcule la valeur réelle selon le mapping des broches.
///          4. Mémorise et retourne le résultat.
///          Mapping des broches :
///          - Porte 1 : sortie 3 = XOR(1, 2)
///          - Porte 2 : sortie 4 = XOR(5, 6)
///          - Porte 3 : sortie 10 = XOR(8, 9)
///          - Porte 4 : sortie 11 = XOR(12, 13)
nts::Tristate nts::Chip4030::compute(std::size_t pin)
{
    if (pin == 7 || pin == 14)
        return nts::Tristate::Undefined;
    if (_cache.count(pin))
        return _cache[pin];
    _cache[pin] = nts::Tristate::Undefined;
    if (pin == 3)
        _cache[pin] = xorGate(computeInput(1), computeInput(2));
    else if (pin == 4)
        _cache[pin] = xorGate(computeInput(5), computeInput(6));
    else if (pin == 10)
        _cache[pin] = xorGate(computeInput(8), computeInput(9));
    else if (pin == 11)
        _cache[pin] = xorGate(computeInput(12), computeInput(13));
    else
        _cache[pin] = nts::Tristate::Undefined;    
    return _cache[pin];
}

/// @brief Établit une connexion entre deux broches de composants différents.
/// @param pin Le numéro de broche de ce composant.
/// @param other Référence au composant à connecter.
/// @param otherPin Le numéro de broche du composant à connecter.
void nts::Chip4030::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    _links[pin] = {&other, otherPin};
}