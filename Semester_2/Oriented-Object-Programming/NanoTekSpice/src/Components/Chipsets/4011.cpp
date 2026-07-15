#include "../../../include/Components/Chipset/4011.hpp"

/// @brief Constructeur du composant puce 4011 (portes NAND).
/// @param name Le nom du composant au sein du circuit.
nts::Chip4011::Chip4011(const std::string& name)
{
    (void)name;
}

/// @brief Simule un cycle du composant 4011.
/// @param tick Le numéro du tick de simulation courant.
/// @details Si le tick est différent du dernier tick enregistré, invalide complètement le cache.
///          Cela garantit que les calculs sont frais pour chaque nouveau cycle de simulation.
void nts::Chip4011::simulate(std::size_t tick)
{
    if (tick != _lastComputeTick) {
        _cache.clear();
        _lastComputeTick = tick;
    }
}

/// @brief Opération logique NAND pour deux entrées Tristate.
/// @param a La première entrée de la porte NAND.
/// @param b La deuxième entrée de la porte NAND.
/// @return Le résultat de l'opération NAND :
///         - Tristate::False si a ET b sont True
///         - Tristate::True si a OU b est False
///         - Tristate::Undefined sinon
/// @details Table de vérité NAND :
///          - NAND(F, F) = T | NAND(T, T) = F
///          - NAND(T, F) = T | NAND(F, T) = T
nts::Tristate nts::Chip4011::nandGate(nts::Tristate a, nts::Tristate b)
{
if (a == nts::Tristate::False || b == nts::Tristate::False)
        return nts::Tristate::True;
    if (a == nts::Tristate::True && b == nts::Tristate::True)
        return nts::Tristate::False;
    return nts::Tristate::Undefined;
}
/// @brief Récupère la valeur d'entrée d'une broche connectée.
/// @param pin Le numéro de broche dont on veut récupérer l'entrée.
/// @return La valeur Tristate provenant du composant connecté à cette broche.
/// @retval Tristate::Undefined Si aucun composant n'est connecté à cette broche.
nts::Tristate nts::Chip4011::computeInput(std::size_t pin)
{
    if (_links.count(pin))
        return _links[pin].component->compute(_links[pin].pin);
    return nts::Tristate::Undefined;
}

/// @brief Calcule la valeur de sortie sur une broche donnée.
/// @param pin Le numéro de broche dont on veut connaître la valeur.
/// @return La valeur Tristate calculée pour cette broche :
///         - Pour les broches de masse (7) et alimentation (14) : Undefined
///         - Pour les broches de sortie (3, 4, 10, 11) : résultat de la porte NAND
///         - Pour les autres broches : Undefined
/// @details Imprime une sous-question avec le cache :
///          1. Si valeur en cache, la retourne immédiatement.
///          2. Marque la broche comme Undefined dans le cache pour casser les cycles.
///          3. Calcule la valeur réelle selon le mapping des broches.
///          4. Mémorise et retourne le résultat.
///          Mapping des broches :
///          - Porte 1 : sortie 3 = NAND(1, 2)
///          - Porte 2 : sortie 4 = NAND(5, 6)
///          - Porte 3 : sortie 10 = NAND(8, 9)
///          - Porte 4 : sortie 11 = NAND(12, 13)
nts::Tristate nts::Chip4011::compute(std::size_t pin)
{
    if (pin == 7 || pin == 14)
        return nts::Tristate::Undefined;
    if (_cache.count(pin))
        return _cache[pin];
    _cache[pin] = nts::Tristate::Undefined;
    if (pin == 3)
        _cache[pin] = nandGate(computeInput(1), computeInput(2));
    else if (pin == 4)
        _cache[pin] = nandGate(computeInput(5), computeInput(6));
    else if (pin == 10)
        _cache[pin] = nandGate(computeInput(8), computeInput(9));
    else if (pin == 11)
        _cache[pin] = nandGate(computeInput(12), computeInput(13));
    else
        _cache[pin] = nts::Tristate::Undefined;
    return _cache[pin];
}

/// @brief Établit une connexion entre deux broches de composants différents.
/// @param pin Le numéro de broche de ce composant.
/// @param other Référence au composant à connecter.
/// @param otherPin Le numéro de broche du composant à connecter.
void nts::Chip4011::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    _links[pin] = {&other, otherPin};
}
