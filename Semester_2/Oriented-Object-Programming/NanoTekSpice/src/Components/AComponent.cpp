#include "../../include/Components/AComponent.hpp"

AComponent::AComponent()
{
    // Default constructor
}

AComponent::AComponent(const AComponent& other)
{
    (void)other; // Unused parameter
    // Copy constructor
}

AComponent& AComponent::operator=(const AComponent& other)
{
    (void)other; // Unused parameter
    // Copy operator
    return *this;
}

void AComponent::display(std::ostream& os)
{
    (void)os; // Unused parameter
    // Display action
}
