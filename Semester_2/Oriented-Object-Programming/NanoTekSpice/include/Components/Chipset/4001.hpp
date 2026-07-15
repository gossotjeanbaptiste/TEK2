#ifndef INCLUDED_CHIP4001_HPP
#define INCLUDED_CHIP4001_HPP

#include "../AComponent.hpp"
#include <map>
#include <set>

namespace nts {
    class Chip4001 : public AComponent {
        private:
            /// @struct Link
            /// @brief Structure représentant une connexion entre deux broches.
            struct Link {
                IComponent* component; ///< Pointeur vers le composant connecté
                std::size_t pin;       ///< Numéro de broche du composant connecté
            };
            std::map<std::size_t, Link> _links;
            /// @brief Cache des valeurs calculées pour éviter les recalculs inutiles.
            std::map<std::size_t, Tristate> _cache;
            /// @brief Cache du tick précédent pour résoudre les cycles de feedback.
            std::map<std::size_t, Tristate> _prevCache;
            /// @brief Timestamp du dernier tick simulé, utilisé pour invalider le cache.
            std::size_t _lastComputeTick = 0;
            /// @brief Set pour tracer les pins en cours de visite (prévenir les cycles infinis).
            std::set<std::size_t> _visiting;
            Tristate norGate(Tristate a, Tristate b);
            Tristate computeInput(std::size_t pin);
            
        public:
            Chip4001(const std::string& name);
            ~Chip4001() = default;
            void simulate(std::size_t tick) override;
            Tristate compute(std::size_t pin) override;
            void setLink(std::size_t pin, IComponent &other, std::size_t otherPin) override;
    };
}
#endif