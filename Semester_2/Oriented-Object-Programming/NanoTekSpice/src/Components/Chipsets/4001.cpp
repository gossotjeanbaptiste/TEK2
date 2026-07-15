#include "../../../include/Components/Chipset/4001.hpp"
#include <set>

/**
 * @brief Constructeur du composant puce 4001 (quadruple porte NOR).
 * 
 * Initialise le composant 4001 qui contient 4 portes NOR indépendantes.
 * Le composant gère les cycles de feedback typiques des flip-flops RS.
 * 
 * @param name Nom du composant au sein du circuit (non utilisé mais requis par l'interface)
 * 
 * @details Le constructeur initialise l'état stable prédéfini du circuit:
 *          - Pin 3 (sortie 1) = False
 *          - Pin 4 (sortie 1 inversée) = True
 *          - Pin 10 (sortie 2) = False
 *          - Pin 11 (sortie 2 inversée) = True
 *          
 *          Cet état initial permet à un flip-flop RS de converger correctement
 *          dès le premier tick de simulation, même avec des cycles de feedback.
 * 
 * @note Les pins 7 et 14 sont réservées pour GND et VCC respectivement
 *       et ne sont pas traitées par le constructeur.
 */
nts::Chip4001::Chip4001(const std::string& name)
{
    (void)name;
    _prevCache[3] = nts::Tristate::False;
    _prevCache[4] = nts::Tristate::True;
    _prevCache[10] = nts::Tristate::False;
    _prevCache[11] = nts::Tristate::True;
}


/**
 * @brief Simule un cycle du composant 4001 pour un tick donné.
 * 
 * Effectue une simulation complète en deux passes pour gérer correctement
 * les cycles de feedback internes du composant 4001 (notamment pour les flip-flops RS).
 * 
 * @param tick Le numéro du tick de simulation courant
 * 
 * @details Processus de simulation en deux passes:
 * 
 *          **Première passe (feed from previous state):**
 *          - Invalide le cache actuel
 *          - Recalcule toutes les sorties (pins 3, 4, 10, 11)
 *          - Les entrées des feedbacks proviennent de _prevCache (état du tick précédent)
 *          - Cela garantit une convergence progresssive sans cycles infinis
 * 
 *          **Deuxième passe (stabilization):**
 *          - Sauvegarde les résultats de la première passe dans _prevCache
 *          - Invalide à nouveau le cache
 *          - Recalcule les sorties avec les nouvelles valeurs du feedback
 *          - Cela assure que l'état final est stable et converge correctement
 * 
 * @note Ne fait rien si le tick est identique au dernier tick enregistré,
 *       ce qui évite les recalculs inutiles lors d'appels successifs avec le même tick.
 * 
 * @see compute() pour la logique de calcul des sorties individuelles
 * @see _prevCache pour la gestion du feedback inter-ticks
 */
void nts::Chip4001::simulate(std::size_t tick)
{
    if (tick != _lastComputeTick) {
        _prevCache = _cache;        
        _cache.clear();
        _visiting.clear();
        _lastComputeTick = tick;

        compute(3);
        compute(4);
        compute(10);
        compute(11);

        _prevCache = _cache;
        _cache.clear();
        _visiting.clear();
        compute(3);
        compute(4);
        compute(10);
        compute(11);
    }
}


/**
 * @brief Effectue une opération logique NOR sur deux entrées Tristate.
 * 
 * Cette fonction est la base logique du composant 4001.
 * Elle implémente la porte NOR avec support complet du tri-état logique.
 * 
 * @param a La première entrée de la porte NOR
 * @param b La deuxième entrée de la porte NOR
 * 
 * @return nts::Tristate Le résultat de l'opération NOR:
 * 
 *         | a | b | Result |
 *         |---|---|--------|
 *         | 0 | 0 |   1    |  (deux entrées basses → sortie haute)
 *         | 0 | 1 |   0    |  (une entrée haute → sortie basse)
 *         | 1 | 0 |   0    |  (une entrée haute → sortie basse)
 *         | 1 | 1 |   0    |  (deux entrées hautes → sortie basse)
 *         | U | ? |   U    |  (entrée indéfinie → sortie indéfinie)
 * 
 * @details Logique implémentée:
 *          - Si l'une des entrées est True → retourne False
 *          - Si les deux entrées sont False → retourne True
 *          - Sinon (au moins une est Undefined) → retourne Undefined
 * 
 * @note Cette fonction est utilisée par compute() pour calculer les sorties
 *       des quatre portes NOR du composant.
 * 
 * @see compute() utilise norGate() pour les pins de sortie 3, 4, 10, 11
 */
nts::Tristate nts::Chip4001::norGate(nts::Tristate a, nts::Tristate b)
{
    if (a == nts::Tristate::True || b == nts::Tristate::True)
        return nts::Tristate::False;
    if (a == nts::Tristate::False && b == nts::Tristate::False)
        return nts::Tristate::True;
    return nts::Tristate::Undefined;
}


/**
 * @brief Récupère la valeur logique d'une broche d'entrée connectée.
 * 
 * Cette méthode utilitaire consulte la table de liens pour trouver le composant
 * connecté à la broche spécifiée, puis demande sa valeur de sortie.
 * 
 * @param pin Numéro de la broche d'entrée de ce composant
 * 
 * @return nts::Tristate La valeur logique provenant du composant connecté:
 *         - Tristate::True si la sortie connectée est haute
 *         - Tristate::False si la sortie connectée est basse
 *         - Tristate::Undefined si aucune connexion existe ou si l'autre composant retourne Undefined
 * 
 * @details Processus:
 *          1. Vérifie si la pin est listée dans _links
 *          2. Si oui, appelle compute() sur le composant connecté avec sa pin
 *          3. Si non, retourne Tristate::Undefined
 * 
 * @note Cette méthode n'effectue pas de calcul logique, elle transmet simplement
 *       la requête au composant connecté amont dans le circuit.
 * 
 * @see setLink() pour établir les connexions dans _links
 * @see compute() qui utilise cette fonction pour les entrées des portes
 */
nts::Tristate nts::Chip4001::computeInput(std::size_t pin)
{
    if (_links.count(pin))
        return _links[pin].component->compute(_links[pin].pin);
    return nts::Tristate::Undefined;
}


/**
 * @brief Calcule la valeur logique de sortie d'une broche donnée.
 * 
 * Fonction principale du composant qui calcule la sortie d'une broche en fonction
 * de ses entrées. Gère les cycles de feedback typiques des flip-flops RS grâce
 * à un mécanisme de détection des cycles.
 * 
 * @param pin Numéro de la broche à calculer
 * 
 * @return nts::Tristate La valeur logique calculée pour cette broche:
 *         - Tristate::Undefined pour les broches de contrôle (7=GND, 14=VCC)
 *         - Résultat NOR pour les broches de sortie (3, 4, 10, 11)
 *         - Tristate::Undefined pour les autres broches
 * 
 * @details **Optimisations et gestion des cycles:**
 * 
 *          **Cache local:** Si la pin a déjà été calculée dans ce tick, retourne immédiatement
 *          la valeur en cache pour éviter les recalculs inutiles.
 * 
 *          **Détection des cycles de feedback:** 
 *          - Utilise _visiting pour tracer les pins en cours de calcul
 *          - Si une pin est déjà en cours de visite, détecte un cycle
 *          - En cas de cycle, retourne la valeur du tick précédent (_prevCache)
 *          - Cela évite les boucles infinies et permet la convergence progressive
 * 
 *          **Mapping des broches du composant 4001:**
 * 
 *          | NOR Gate | Pins entrée | Pin sortie | Formule          |
 *          |----------|-------------|------------|------------------|
 *          | 1        | 1, 2        | 3          | NOR(pin1, pin2)  |
 *          | 2        | 5, 6        | 4          | NOR(pin5, pin6)  |
 *          | 3        | 8, 9        | 10         | NOR(pin8, pin9)  |
 *          | 4        | 12, 13      | 11         | NOR(pin12, pin13)|
 * 
 * @note **Ordre de priorité des appels:**
 *       1. Vérifier les pins de contrôle (GND/VCC)
 *       2. Vérifier le cache local
 *       3. Détecter les cycles (pins en cours de visite)
 *       4. Calculer la valeur réelle
 *       5. Mémoriser et retourner
 * 
 * @warning Les cycles de feedback (ex: pin 3 → pin 4 → pin 3) sont gérés,
 *          mais le premier appel dans un cycle retournera la valeur du tick précédent
 *          via _prevCache, permettant la stabilisation progressive.
 * 
 * @see simulate() qui appelle compute() et gère les deux passes
 * @see norGate() pour la logique de calcul des portes
 * @see computeInput() pour obtenir les valeurs des broches d'entrée
 */
nts::Tristate nts::Chip4001::compute(std::size_t pin)
{
    if (pin == 7 || pin == 14)
        return nts::Tristate::Undefined;
    if (_cache.count(pin))
        return _cache[pin];
    if (_visiting.count(pin)) {
        if (_prevCache.count(pin))
            return _prevCache[pin];
        return nts::Tristate::Undefined;
    }
    _visiting.insert(pin);
    nts::Tristate result = nts::Tristate::Undefined;
    if (pin == 3)
        result = norGate(computeInput(1), computeInput(2));
    else if (pin == 4)
        result = norGate(computeInput(5), computeInput(6));
    else if (pin == 10)
        result = norGate(computeInput(8), computeInput(9));
    else if (pin == 11)
        result = norGate(computeInput(12), computeInput(13));
    _visiting.erase(pin);
    _cache[pin] = result;
    return result;
}


/**
 * @brief Établit une connexion entre une broche de ce composant et celle d'un autre composant.
 * 
 * Crée un lien de dépendance qui permet à ce composant de consulter la valeur
 * de sortie d'un autre composant via computeInput().
 * 
 * @param pin Numéro de la broche d'entrée de ce composant 4001
 * @param other Référence au composant à connecter en amont
 * @param otherPin Numéro de la broche de sortie du composant amont
 * 
 * @details La connexion est stockée dans la structure _links:
 *          - Clé: pin (numéro de broche locale de ce 4001)
 *          - Valeur: {pointeur vers other, numéro de broche amont}
 * 
 *          **Pins d'entrée valides du 4001:**
 *          - Porte NOR 1: pins 1, 2
 *          - Porte NOR 2: pins 5, 6
 *          - Porte NOR 3: pins 8, 9
 *          - Porte NOR 4: pins 12, 13
 * 
 * @note Cette méthode remplace complètement toute connexion antérieure sur la même pin.
 *       Il est possible (et courant) de connecter plusieurs pins à des composants différents,
 *       ou plusieurs pins au même composant avec des numéros de broche différents.
 * 
 * @warning Les pins de contrôle (7=GND, 14=VCC) ne doivent pas être connectées
 *          via cette méthode - elles sont traitées directement dans compute().
 * 
 * @see computeInput() qui consulte les liens établis par setLink()
 * @see compute() qui utilise computeInput() pour calculer les sorties
 */
void nts::Chip4001::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    _links[pin] = {&other, otherPin};
}