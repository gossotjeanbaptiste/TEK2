int main(void)
{
    unsigned int energy = 1337;

    {
        Mondas::Cyberlegion cyberlegion(energy);
    }

    Mondas::Cyberlegion cyberlegion(energy);

    std::srand(42);
    while (cyberlegion.assimilate(new Mondas::Human("Citizen " + std::to_string(std::rand() % 100), 128 + std::rand() % 128)) == true);
    std::cout << cyberlegion.getSize() << std::endl;
    return 0;
}
