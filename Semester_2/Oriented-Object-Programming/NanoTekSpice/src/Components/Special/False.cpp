#include "../../../include/Components/Special/False.hpp"

/**
 * @brief Constructeur du composant False
 * @param name Nom du composant (non utilisé)
 * 
 * Initialise le composant False qui retourne toujours nts::False.
 */
FalseComponent::FalseComponent(const std::string& name)
{
    (void)name;
}

/**
 * @brief Simule un cycle du composant False
 * @param tick Numéro du cycle (non utilisé, False est constant)
 * 
 * Aucune opération nécessaire, False retourne une valeur constante.
 */
void FalseComponent::simulate(std::size_t tick)
{
    (void)tick;
}

/**
 * @brief Retourne la valeur constante False
 * @param pin Numéro de la pin (ignoré, False a une seule sortie)
 * @return nts::False en toutes circonstances
 */
nts::Tristate FalseComponent::compute(std::size_t pin)
{
    (void)pin;
    return nts::Tristate::False;
}

/**
 * @brief Établit une connexion (non fonctionnelle pour False)
 * @param pin Numéro de la pin locale
 * @param other Référence au composant à connecter
 * @param otherPin Numéro de la pin du composant autre
 * 
 * @note False est une source autonome sans entrées, les connexions sont ignorées.
 */
void FalseComponent::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    (void)pin;
    (void)other;
    (void)otherPin;
}