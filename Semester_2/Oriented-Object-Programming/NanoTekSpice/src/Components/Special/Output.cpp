#include "../../../include/Components/Special/Output.hpp"

/**
 * @brief Constructeur de l'Output
 * @param name Nom du composant Output
 * 
 * Initialise l'Output avec une liaison vide.
 */
Output::Output(const std::string& name) : _link{nullptr, 0}
{
    (void)name;
}

/**
 * @brief Établit une liaison avec un autre composant
 * @param pin Numéro de la pin locale (ignoré, Output n'a qu'une pin)
 * @param other Composant source dont on lit la valeur
 * @param otherPin Pin du composant source
 */
void Output::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    (void)pin;
    _link = {&other, otherPin};
}

/**
 * @brief Simule le composant à un tick donné
 * @param tick Numéro du tick de simulation
 * 
 * Rien à faire pour Output : les valeurs sont calculées à la demande.
 */
void Output::simulate(std::size_t tick)
{
    (void)tick;
}

/**
 * @brief Retourne la valeur de la pin de sortie
 * @param pin Numéro de la pin (ignoré, Output n'a qu'une pin)
 * @return Valeur calculée depuis le composant lié, ou Undefined si pas de liaison
 * 
 * L'Output retourne la valeur du composant source auquel il est lié.
 */
nts::Tristate Output::compute(std::size_t pin)
{
    (void)pin;
    if (!_link.component)
        return nts::Tristate::Undefined;
    return _link.component->compute(_link.pin);
}
