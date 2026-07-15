#include "../../../include/Components/Adv_Chipset/2716.hpp"
#include <fstream>

/**
 * @brief Charge le fichier ROM au démarrage
 * 
 * Tente de charger le fichier ROM depuis "./roms/<name>.bin". Si le fichier n'existe pas,
 * la mémoire reste vide (tous les octets à 0).
 * 
 * @note La ROM est immuable et chargée une seule fois au démarrage
 */
void Chip2716::_loadROM()
{
    std::string filename = "./roms/" + _name + ".bin";
    std::ifstream file(filename, std::ios::binary);
    
    _memory.resize(2048, 0);
    
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(_memory.data()), 2048);
        file.close();
    }
}

/**
 * @brief Lit les 11 bits d'adresse depuis les pins connectées
 * 
 * Mappe les pins d'adresse comme suit:
 * - Pin 8: bit 0 (LSB)
 * - Pin 7: bit 1
 * - Pin 6: bit 2
 * - Pin 5: bit 3
 * - Pin 4: bit 4
 * - Pin 3: bit 5
 * - Pin 2: bit 6
 * - Pin 1: bit 7
 * - Pin 23: bit 8
 * - Pin 22: bit 9
 * - Pin 19: bit 10 (MSB)
 * 
 * @return unsigned short Adresse de 11 bits (0-2047)
 */
unsigned short Chip2716::_readAddress()
{
    unsigned short address = 0;
    
    const int addressPins[] = {8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19};
    
    for (int i = 0; i < 11; ++i) {
        if (_links.count(addressPins[i])) {
            nts::Tristate bit = _links[addressPins[i]].component->compute(_links[addressPins[i]].pin);
            if (bit == nts::Tristate::True)
                address |= (1 << i);
        }
    }
    
    return address;
}

Chip2716::Chip2716(const std::string& name) : _name(name)
{
    _loadROM();
}

Chip2716::Chip2716(const Chip2716& other) : _name(other._name), _memory(other._memory)
{
    // Copy constructor
}

Chip2716& Chip2716::operator=(const Chip2716& other)
{
    if (this != &other) {
        _name = other._name;
        _memory = other._memory;
    }
    return *this;
}

void Chip2716::simulate(std::size_t tick)
{
    (void)tick;
    // ROM is static, nothing to do
}

/**
 * @brief Retourne la valeur logique d'une broche de sortie de données
 * 
 * Lit l'adresse sur les pins d'adresse et retourne le bit correspondant du byte à cette adresse.
 * 
 * Mappage des pins de données (sortie):
 * - Pin 9: bit 0 (LSB)
 * - Pin 10: bit 1
 * - Pin 11: bit 2
 * - Pin 13: bit 3
 * - Pin 14: bit 4
 * - Pin 15: bit 5
 * - Pin 16: bit 6
 * - Pin 17: bit 7 (MSB)
 * 
 * @param pin Numéro de la broche
 * @return nts::Tristate Valeur logique du bit, ou Undefined si pas une sortie de données
 */
nts::Tristate Chip2716::compute(std::size_t pin)
{
    unsigned short address = _readAddress();
    
    const std::pair<int, int> outputPins[] = {
        {9, 0}, {10, 1}, {11, 2}, {13, 3},
        {14, 4}, {15, 5}, {16, 6}, {17, 7}
    };
    
    for (const auto& [outputPin, bitIndex] : outputPins)
        if (pin == static_cast<std::size_t>(outputPin) && address < 2048) {
            unsigned char byte = _memory[address];
            bool bit = (byte >> bitIndex) & 1;
            return bit ? nts::Tristate::True : nts::Tristate::False;
        }
    
    return nts::Tristate::Undefined;
}

void Chip2716::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    _links[pin] = {&other, otherPin};
}

void Chip2716::display(std::ostream& os)
{
    (void)os;
    // Display action
}

