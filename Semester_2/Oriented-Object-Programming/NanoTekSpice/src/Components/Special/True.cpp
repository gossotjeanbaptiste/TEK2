// True.cpp
#include "../../../include/Components/Special/True.hpp"

/**
 * @brief Constructeur du composant True
 * @param name Nom du composant (non utilisé)
 * 
 * Initialise le composant True qui retourne toujours nts::True.
 */
TrueComponent::TrueComponent(const std::string& name)
{
    (void)name;
}

/**
 * @brief Simule un cycle du composant True
 * @param tick Numéro du cycle (non utilisé, True est constant)
 * 
 * Aucune opération nécessaire, True retourne une valeur constante.
 */
void TrueComponent::simulate(std::size_t tick)
{
    (void)tick;
}

/**
 * @brief Retourne la valeur constante True
 * @param pin Numéro de la pin (ignoré, True a une seule sortie)
 * @return nts::True en toutes circonstances
 */
nts::Tristate TrueComponent::compute(std::size_t pin)
{
    (void)pin;
    return nts::Tristate::True;
}

/**
 * @brief Établit une connexion (non fonctionnelle pour True)
 * @param pin Numéro de la pin locale
 * @param other Référence au composant à connecter
 * @param otherPin Numéro de la pin du composant autre
 * 
 * @note True est une source autonome sans entrées, les connexions sont ignorées.
 */
void TrueComponent::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    (void)pin;
    (void)other;
    (void)otherPin;
}