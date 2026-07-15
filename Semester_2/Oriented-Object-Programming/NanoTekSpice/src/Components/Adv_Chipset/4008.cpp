#include "../../../include/Components/Adv_Chipset/4008.hpp"

/**
 * @brief Constructeur du composant puce 4008 (additionneur 4 bits).
 * 
 * Initialise le composant 4008 qui effectue l'addition arithmétique de deux nombres
 * de 4 bits avec gestion de la retenue d'entrée (carry in) et génération de la retenue
 * de sortie (carry out).
 * 
 * @param name Nom du composant au sein du circuit (non utilisé mais requis par l'interface)
 * 
 * @details Le composant 4008 contient 4 additionneurs complets (full adders) en cascade,
 *          chacun traitant un bit différent avec propagation de la retenue.
 *          
 *          Structure interne:
 *          - Stage 1: Bits 0 - entrées sur pins 6, 7; sortie bit 0 sur pin 10
 *          - Stage 2: Bits 1 - entrées sur pins 4, 5; sortie bit 1 sur pin 11
 *          - Stage 3: Bits 2 - entrées sur pins 2, 3; sortie bit 2 sur pin 12
 *          - Stage 4: Bits 3 - entrées sur pins 1, 15; sortie bit 3 sur pin 13; carry out sur pin 14
 *          - Carry in: pin 9
 * 
 * @note Les pins 8 et 16 sont réservées pour GND et VCC respectivement.
 */
nts::Chip4008::Chip4008(const std::string &name)
{
    (void)name;
}


/**
 * @brief Implémente un additionneur complet (full adder) pour un bit unique.
 * 
 * Calcule la somme et la retenue de sortie étant donné deux bits d'entrée
 * et une retenue d'entrée, selon les règles de l'addition binaire.
 * 
 * @param a Le premier bit d'additionne
 * @param b Le deuxième bit d'additionne
 * @param cin La retenue d'entrée (carry in)
 * 
 * @return std::pair<nts::Tristate, nts::Tristate> Un pair contenant:
 *         - first: la somme (a ⊕ b ⊕ cin)
 *         - second: la retenue de sortie (cout)
 * 
 * @details **Table de vérité logique:**
 * 
 *          | a | b | cin | sum | cout |
 *          |---|---|-----|-----|------|
 *          | 0 | 0 |  0  |  0  |  0   |
 *          | 0 | 1 |  0  |  1  |  0   |
 *          | 1 | 0 |  0  |  1  |  0   |
 *          | 1 | 1 |  0  |  0  |  1   |
 *          | 0 | 0 |  1  |  1  |  0   |
 *          | 0 | 1 |  1  |  0  |  1   |
 *          | 1 | 0 |  1  |  0  |  1   |
 *          | 1 | 1 |  1  |  1  |  1   |
 *          | U | ? |  ?  |  U  |  U   |
 * 
 *          **Formules:**
 *          - sum = (a == b) ? cin : !cin
 *          - cout = 1 si au moins 2 entrées sont 1, sinon 0
 * 
 * @note Si l'une des entrées est Undefined, la sortie sera Undefined.
 *       Cela propage les erreurs et garantit la cohérence.
 * 
 * @see compute() qui utilise fullAdder() pour les 4 stages d'addition
 */
std::pair<nts::Tristate, nts::Tristate> nts::Chip4008::fullAdder(nts::Tristate a, nts::Tristate b, nts::Tristate cin)
{
    nts::Tristate sum;
    if (a == nts::Undefined || b == nts::Undefined || cin == nts::Undefined)
        sum = nts::Undefined;
    else
        sum = (a == b) ? cin : (cin == nts::True ? nts::False : nts::True);
    nts::Tristate cout;
    if ((a == nts::True && b == nts::True) || 
        (a == nts::True && cin == nts::True) || 
        (b == nts::True && cin == nts::True))
        cout = nts::True;
    else if ((a == nts::False && b == nts::False) || 
            (a == nts::False && cin == nts::False) || 
            (b == nts::False && cin == nts::False))
        cout = nts::False;
    else
        cout = nts::Undefined;

    return {sum, cout};
}

/**
 * @brief Récupère la valeur logique d'une broche d'entrée connectée.
 * 
 * Consulte la table de liens pour trouver le composant connecté à la broche
 * spécifiée et récupère sa valeur de sortie.
 * 
 * @param pin Numéro de la broche d'entrée du composant 4008
 * 
 * @return nts::Tristate La valeur logique provenant du composant connecté:
 *         - nts::True si la sortie connectée est haute
 *         - nts::False si la sortie connectée est basse
 *         - nts::Undefined si aucune connexion existe ou si l'autre composant retourne Undefined
 * 
 * @details Processus:
 *          1. Vérifie si la pin est listée dans _links
 *          2. Si oui, appelle compute() sur le composant connecté avec sa pin
 *          3. Si non, retourne nts::Undefined
 * 
 * @note Cette méthode transmet la requête au composant connecté amont
 *       sans effectuer de calcul logique.
 * 
 * @see setLink() pour établir les connexions dans _links
 * @see compute() qui utilise computeInput() pour les calculs d'addition
 */
nts::Tristate nts::Chip4008::computeInput(std::size_t pin)
{
    if (_links.count(pin))
        return _links[pin].component->compute(_links[pin].pin);
    return nts::Undefined;
}

/**
 * @brief Calcule la valeur logique de sortie d'une broche donnée.
 * 
 * Effectue l'addition de deux nombres de 4 bits avec retenue propagée.
 * Chaque broche de sortie correspond à l'un des bits somme ou à la retenue.
 * 
 * @param pin Numéro de la broche dont on demande la sortie
 * 
 * @return nts::Tristate La valeur logique calculée pour cette broche:
 *         - nts::Undefined pour les broches de contrôle (8=GND, 16=VCC) ou inexistantes
 *         - Bit de somme (0-3) pour les pins 10-13
 *         - Retenue de sortie (carry out) pour la pin 14
 * 
 * @details **Algorithme de calcul:**
 * 
 *          L'additionneur 4008 implémente l'addition en cascade à 4 stages:
 * 
 *          Stage 1 (Bit 0):
 *          - Entrées: pins 6, 7
 *          - Retenue d'entrée: pin 9
 *          - Sortie: pin 10 (bit 0 de la somme)
 *          - Retenue: propagée au stage 2
 * 
 *          Stage 2 (Bit 1):
 *          - Entrées: pins 4, 5
 *          - Retenue d'entrée: carry du stage 1
 *          - Sortie: pin 11 (bit 1 de la somme)
 *          - Retenue: propagée au stage 3
 * 
 *          Stage 3 (Bit 2):
 *          - Entrées: pins 2, 3
 *          - Retenue d'entrée: carry du stage 2
 *          - Sortie: pin 12 (bit 2 de la somme)
 *          - Retenue: propagée au stage 4
 * 
 *          Stage 4 (Bit 3):
 *          - Entrées: pins 1, 15
 *          - Retenue d'entrée: carry du stage 3
 *          - Sortie: pin 13 (bit 3 de la somme)
 *          - Sortie: pin 14 (retenue de sortie / overflow)
 * 
 *          Chaque stage utilise fullAdder() pour effectuer l'addition d'un bit.
 * 
 * @note **Mapping complet des pins:**
 *       - Pin 1: a[3] (bit 3 de A)
 *       - Pin 2: a[2] (bit 2 de A)
 *       - Pin 3: b[2] (bit 2 de B)
 *       - Pin 4: a[1] (bit 1 de A)
 *       - Pin 5: b[1] (bit 1 de B)
 *       - Pin 6: a[0] (bit 0 de A)
 *       - Pin 7: b[0] (bit 0 de B)
 *       - Pin 8: GND (masse) - retourne Undefined
 *       - Pin 9: cin (carry in / retenue d'entrée)
 *       - Pin 10: sum[0] (bit 0 de la somme)
 *       - Pin 11: sum[1] (bit 1 de la somme)
 *       - Pin 12: sum[2] (bit 2 de la somme)
 *       - Pin 13: sum[3] (bit 3 de la somme)
 *       - Pin 14: cout (carry out / retenue de sortie)
 *       - Pin 15: b[3] (bit 3 de B)
 *       - Pin 16: VCC (alimentation) - retourne Undefined
 * 
 * @see fullAdder() pour la logique d'addition d'un bit unique
 * @see computeInput() pour obtenir les valeurs des pins d'entrée
 */
nts::Tristate nts::Chip4008::compute(std::size_t pin)
{
    if (pin == 8 || pin == 16)
        return nts::Undefined;

    nts::Tristate current_carry = computeInput(9);
    
    auto stage1 = fullAdder(computeInput(7), computeInput(6), current_carry);
    if (pin == 10) return stage1.first;
    current_carry = stage1.second;

    auto stage2 = fullAdder(computeInput(5), computeInput(4), current_carry);
    if (pin == 11) return stage2.first;
    current_carry = stage2.second;

    auto stage3 = fullAdder(computeInput(3), computeInput(2), current_carry);
    if (pin == 12) return stage3.first;
    current_carry = stage3.second;

    auto stage4 = fullAdder(computeInput(1), computeInput(15), current_carry);
    if (pin == 13) return stage4.first;
    if (pin == 14) return stage4.second;

    return nts::Undefined;
}

/**
 * @brief Simule un cycle du composant 4008.
 * 
 * @param tick Le numéro du tick de simulation courant
 * 
 * @details Le composant 4008 est un composant combinatoire (sans état interne),
 *          donc la simulation ne nécessite aucun calcul ni mise à jour d'état.
 *          Cette méthode est une non-opération (no-op).
 * 
 * @note Les résultats de l'addition sont calculés directement dans compute()
 *       au moment où ils sont demandés, sans latence de propagation.
 */
void nts::Chip4008::simulate(std::size_t tick)
{
    (void)tick;
}

/**
 * @brief Établit une connexion entre une broche de ce composant et celle d'un autre composant.
 * 
 * Crée un lien de dépendance qui permet à ce composant de consulter la valeur
 * de sortie d'un autre composant via computeInput().
 * 
 * @param pin Numéro de la broche d'entrée de ce composant 4008
 * @param other Référence au composant à connecter en amont
 * @param otherPin Numéro de la broche de sortie du composant amont
 * 
 * @details La connexion est stockée dans la map _links:
 *          - Clé: pin (numéro de broche locale de ce composant)
 *          - Valeur: {pointeur vers other, otherPin}
 * 
 *          **Pins d'entrée valides du 4008:**
 *          - Bits d'entrée A: pins 1, 2, 4, 6 (a[3], a[2], a[1], a[0])
 *          - Bits d'entrée B: pins 3, 5, 7, 15 (b[2], b[1], b[0], b[3])
 *          - Retenue d'entrée: pin 9 (cin)
 * 
 *          Exemple:
 *          ```
 *          setLink(6, inputA, 1);  // pin 6 lue depuis inputA:1
 *          setLink(7, inputB, 1);  // pin 7 lue depuis inputB:1
 *          setLink(9, carryIn, 1); // pin 9 lue depuis carryIn:1
 *          ```
 * 
 * @note Cette méthode remplace complètement toute connexion antérieure sur la même pin.
 *       Il est possible de connecter plusieurs pins à des composants différents,
 *       ou plusieurs pins au même composant avec des numéros de broche différents.
 * 
 * @warning Les pins de contrôle (8=GND, 16=VCC) ne doivent pas être connectées
 *          via cette méthode - elles sont traitées directement dans compute().
 * 
 * @see computeInput() qui consulte les liens établis par setLink()
 * @see compute() qui utilise computeInput() pour les calculs d'addition
 */
void nts::Chip4008::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    _links[pin] = {&other, otherPin};
}