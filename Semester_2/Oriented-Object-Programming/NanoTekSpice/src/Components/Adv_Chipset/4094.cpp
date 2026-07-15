#include "../../../include/Components/Adv_Chipset/4094.hpp"

/**
 * @brief Constructeur du Chip4094, registre à décalage 8-bits avec verrous.
 * 
 * Initialise le registre à décalage et les verrous à l'état indéfini.
 * Le 4094 est un registre à décalage 8-bits avec sorties parallèles et verrous
 * qui peut être utilisé pour les pilotes d'affichage ou la conversion de sortie parallèle.
 * 
 * @param name L'identifiant de nom pour cette instance du chip
 */
nts::Chip4094::Chip4094(const std::string &name) : _lastClock(nts::Undefined)
{
    (void)name;
    _shiftRegister.assign(8, nts::Undefined);
    _latches.assign(8, nts::Undefined);
}

/**
 * @brief Simule un cycle d'horloge du registre à décalage 4094.
 * 
 * Cette méthode :
 * - Décale le registre sur un front montant d'horloge (transition False à True)
 * - Charge l'entrée de données dans la première position du registre
 * - Verrouille les valeurs du registre lorsque strobe est actif
 * - Met à jour le suivi interne de l'état de l'horloge
 * 
 * Assignation des broches :
 * - Broche 1 : Strobe (validation du stockage)
 * - Broche 2 : Entrée de données
 * - Broche 3 : Entrée d'horloge
 * - Broche 15 : Validation de sortie
 * 
 * @param tick Le cycle de simulation actuel (inutilisé dans cette implémentation)
 */
void nts::Chip4094::simulate(std::size_t tick)
{
    (void)tick;
    nts::Tristate currentClock = computeInput(3);
    nts::Tristate data = computeInput(2);
    nts::Tristate strobe = computeInput(1);

    if (_lastClock == nts::False && currentClock == nts::True) {
        for (int i = 7; i > 0; --i)
            _shiftRegister[i] = _shiftRegister[i - 1];
        _shiftRegister[0] = data;

        if (strobe == nts::True)
            for (int i = 0; i < 8; ++i)
                _latches[i] = _shiftRegister[i];
    }
    
    if (strobe == nts::True)
        for (int i = 0; i < 8; ++i)
            _latches[i] = _shiftRegister[i];
    _lastClock = currentClock;
}

/**
 * @brief Calcule la valeur logique de l'entrée à une broche spécifiée.
 * 
 * Cette méthode récupère la valeur logique provenant du composant lié
 * à la broche d'entrée donnée, si une liaison existe.
 * 
 * @param pin Le numéro de la broche d'entrée
 * @return La valeur logique (True, False, ou Undefined) de l'entrée,
 *         ou Undefined si aucune liaison n'existe pour cette broche
 */
nts::Tristate nts::Chip4094::computeInput(std::size_t pin)
{
    if (_links.count(pin))
        return _links[pin].component->compute(_links[pin].pin);
    return nts::Undefined;
}

/**
 * @brief Calcule la valeur logique de sortie à une broche spécifiée.
 * 
 * Cette méthode retourne la valeur de sortie du registre à décalage ou des verrous
 * correspondant à la broche demandée. Les sorties sont masquées (retournent Undefined)
 * lorsque l'entrée de validation (pin 15) est à False.
 * 
 * Assignation des broches de sortie :
 * - Broches 4-7 et 11-14 : sorties latches Q0-Q7
 * - Broche 9 : sortie en cascade du registre
 * - Broche 10 : sortie en cascade avec validation (OE)
 * 
 * @param pin Le numéro de la broche de sortie
 * @return La valeur logique de sortie (True, False, ou Undefined)
 */
nts::Tristate nts::Chip4094::compute(std::size_t pin)
{
    nts::Tristate oe = computeInput(15);
    if (oe == nts::False)
        if ((pin >= 4 && pin <= 7) || (pin >= 11 && pin <= 14))
            return nts::Undefined;

    if (pin == 4) return _latches[0];
    if (pin == 5) return _latches[1];
    if (pin == 6) return _latches[2];
    if (pin == 7) return _latches[3];
    if (pin == 14) return _latches[4];
    if (pin == 13) return _latches[5];
    if (pin == 12) return _latches[6];
    if (pin == 11) return _latches[7];
    if (pin == 9) return _shiftRegister[7];
    if (pin == 10) return (oe == nts::True) ? _shiftRegister[7] : nts::Undefined;

    return nts::Undefined;
}

/**
 * @brief Établit une liaison entre une broche de ce composant et une broche d'un autre composant.
 * 
 * Cette méthode connecte une broche d'entrée de ce chip à une broche de sortie d'un autre
 * composant du circuit, permettant le passage de signaux logiques entre les composants.
 * 
 * @param pin Le numéro de la broche sur ce composant
 * @param other Référence au composant à lier
 * @param otherPin Le numéro de la broche sur le composant à lier
 */
void nts::Chip4094::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    _links[pin] = {&other, otherPin};
}