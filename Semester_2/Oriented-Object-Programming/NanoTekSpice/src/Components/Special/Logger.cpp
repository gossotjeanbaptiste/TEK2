#include "../../../include/Components/Special/Logger.hpp"
#include <fstream>

/**
 * @brief Constructeur du Logger
 * @param name Nom du composant Logger
 * 
 * Initialise le Logger avec un état initial de clock à False.
 */
Logger::Logger(const std::string& name) : _lastClock(nts::Tristate::False)
{
    (void)name;
}


/**
 * @brief Établit une liaison entre une pin du Logger et une pin d'un autre composant
 * 
 * @param pin Index de la pin du Logger à lier
 * @param other Référence vers l'autre IComponent à lier
 * @param otherPin Index de la pin du composant distant
 * 
 * @details Cette méthode établit une connexion entre la pin du Logger et celle
 *          d'un autre composant en stockant la relation dans la map interne _links.
 *          Le lien permet le flux de données entre les deux composants.
 */
void Logger::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    _links[pin] = {&other, otherPin};
}


/**
 * @brief Récupère la valeur d'entrée d'un composant lié à la pin spécifiée
 * 
 * @param pin Le numéro de la pin à lire
 * 
 * @return nts::Tristate La valeur tristate calculée depuis le composant lié,
 *         ou nts::Tristate::Undefined si aucune liaison n'existe pour cette pin
 * 
 * @details Cette méthode vérifie si une liaison existe pour la pin spécifiée et,
 *          le cas échéant, calcule et retourne la valeur du composant connecté.
 *          Si aucune liaison n'est trouvée, elle retourne Undefined.
 */
nts::Tristate Logger::getInput(std::size_t pin)
{
    if (_links.count(pin))
        return _links[pin].component->compute(_links[pin].pin);
    return nts::Tristate::Undefined;
}


/**
 * @brief Écrit un octet caractère dans un fichier binaire de log
 * 
 * Lit 8 bits depuis les entrées 1-8 du composant Logger, les combine en un seul
 * octet unsigned char (où l'entrée 1 représente le LSB), et ajoute l'octet
 * résultant à "./log.bin" en mode binaire.
 * 
 * Le mappage des bits est le suivant:
 * - Entrée 1 -> bit 0 (LSB*)
 * - Entrée 2 -> bit 1
 * - ...
 * - Entrée 8 -> bit 7 (MSB*)
 * 
 * * LSB : Least Significant Bit
 * * MSB : Most Significant Bit
 * 
 * @note Si le fichier de log ne peut pas être ouvert, l'opération échoue silencieusement
 * @note Le fichier est ouvert en mode append, préservant le contenu existant
 * 
 * @return void
 * 
 * @see getInput()
 * @see nts::Tristate
 */
void Logger::writeCharacter()
{
    unsigned char byte = 0;
    for (int i = 1; i <= 8; ++i) {
        nts::Tristate bit = getInput(i);
        if (bit == nts::Tristate::True)
            byte |= (1 << (i - 1));
    }
    std::ofstream file("./log.bin", std::ios::app | std::ios::binary); // * append + binaire
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
        file.close();
    }
}


/**
 * @brief Simule le composant Logger pour un tick d'horloge donné
 * 
 * Cette méthode détecte une montée sur l'entrée d'horloge (pin 9) et écrit
 * un caractère quand la montée est détectée ET le signal d'inhibition (pin 10)
 * n'est pas actif (False).
 * 
 * @param tick Le numéro du tick de simulation actuel (non utilisé)
 * 
 * @note La méthode suit l'état précédent de l'horloge dans _lastClock pour
 *       détecter les montées. Un front montant est défini comme une transition
 *       explicite de False vers True.
 * @note L'écriture du caractère est déclenchée uniquement quand:
 *       - Une transition de False vers True est détectée sur l'horloge ET
 *       - Le signal d'inhibition est False (désactivé)
 * 
 * @see writeCharacter()
 * @see getInput()
 */
void Logger::simulate(std::size_t tick)
{
    (void)tick;
    nts::Tristate currentClock = getInput(9);
    nts::Tristate inhibit = getInput(10);
    bool clockRising = (_lastClock == nts::Tristate::False && currentClock == nts::Tristate::True);
    if (clockRising && inhibit == nts::Tristate::False)
        writeCharacter();
    _lastClock = currentClock;
}

nts::Tristate Logger::compute(std::size_t pin)
{
    (void)pin;
    return nts::Tristate::Undefined;
}
