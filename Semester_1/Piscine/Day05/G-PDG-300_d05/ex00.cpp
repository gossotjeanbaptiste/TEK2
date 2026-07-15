#include "Human.hpp"

int main(void)
{
    Mondas::Human human1("Korving", 164);
    const Mondas::Human human2("Joy");

    std::cout << human1.getName() << std::endl;
    std::cout << human2.getIq() << std::endl;
    human1.setIq(193);
    human1.setIq(103);
    std::cout << human1.getIq() << std::endl;
    human1.think();
    human2.think();

    return 0;
}
