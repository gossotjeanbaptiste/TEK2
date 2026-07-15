#include <iostream>
#include "FruitBox.hpp"
#include "FruitUtils.hpp"
#include "Orange.hpp"
#include "Lemon.hpp"
#include "Strawberry.hpp"
#include "Almond.hpp"
#include "Grapefruit.hpp"
#include "BloodOrange.hpp"
#include "Raspberry.hpp"
#include "Coconut.hpp"

int main(void)
{
    std::cout << "=== Testing all fruits types ===" << std::endl;
    std::cout << "\nIndividual fruits info:" << std::endl;
    {
        Orange o;
        Lemon l;
        Strawberry s;
        Almond a;
        Grapefruit g;
        BloodOrange bo;
        Raspberry r;
        Coconut c;

        std::cout << "Orange: " << o << std::endl;
        std::cout << "Lemon: " << l << std::endl;
        std::cout << "Strawberry: " << s << std::endl;
        std::cout << "Almond: " << a << std::endl;
        std::cout << "Grapefruit: " << g << std::endl;
        std::cout << "BloodOrange: " << bo << std::endl;
        std::cout << "Raspberry: " << r << std::endl;
        std::cout << "Coconut: " << c << std::endl;
    }

    std::cout << "\n=== Testing peeling ===" << std::endl;
    {
        Orange o2;
        std::cout << "Orange before peel: " << o2 << std::endl;
        o2.peel();
        std::cout << "Orange after peel: " << o2 << std::endl;
        Strawberry s2;
        std::cout << "Strawberry before peel (always peeled): " << s2 << std::endl;
    }

    std::cout << "\n=== Testing FruitUtils::sort ===" << std::endl;
    {
        FruitBox unsorted(15);
        FruitBox lemonBox(3);
        FruitBox citrusBox(4);
        FruitBox berryBox(3);
        std::cout << "\nAdding fruits to unsorted box:" << std::endl;
        unsorted.pushFruit(new Orange());
        unsorted.pushFruit(new Lemon());
        unsorted.pushFruit(new Lemon());
        unsorted.pushFruit(new Strawberry());
        unsorted.pushFruit(new Almond());
        unsorted.pushFruit(new Grapefruit());
        unsorted.pushFruit(new BloodOrange());
        unsorted.pushFruit(new Raspberry());
        unsorted.pushFruit(new Coconut());
        unsorted.pushFruit(new Strawberry());
        unsorted.pushFruit(new Lemon());
        unsorted.pushFruit(new Grapefruit());
        std::cout << "Total fruits in unsorted: " << unsorted.nbFruits() << std::endl;
        FruitUtils::sort(unsorted, lemonBox, citrusBox, berryBox);
        std::cout << "\nAfter sorting:" << std::endl;
        std::cout << "Lemon box (" << lemonBox.nbFruits() << "/" << lemonBox.getSize() << "): " << lemonBox << std::endl;
        std::cout << "Citrus box (" << citrusBox.nbFruits() << "/" << citrusBox.getSize() << "): " << citrusBox << std::endl;
        std::cout << "Berry box (" << berryBox.nbFruits() << "/" << berryBox.getSize() << "): " << berryBox << std::endl;
        std::cout << "Unsorted box (" << unsorted.nbFruits() << "/" << unsorted.getSize() << "): " << unsorted << std::endl;
    }

    std::cout << "\n=== Testing overflow (fruits that don't fit) ===" << std::endl;
    {
        FruitBox unsorted2(10);
        FruitBox lemonBox2(1);
        FruitBox citrusBox2(2);
        FruitBox berryBox2(1);

        unsorted2.pushFruit(new Lemon());
        unsorted2.pushFruit(new Lemon());
        unsorted2.pushFruit(new Lemon());
        unsorted2.pushFruit(new Orange());
        unsorted2.pushFruit(new Grapefruit());
        unsorted2.pushFruit(new BloodOrange());
        unsorted2.pushFruit(new Strawberry());
        unsorted2.pushFruit(new Raspberry());
        unsorted2.pushFruit(new Almond());
        unsorted2.pushFruit(new Coconut());
        
        std::cout << "Before sort (overflow test):" << std::endl;
        std::cout << "Total unsorted: " << unsorted2.nbFruits() << std::endl;
        
        FruitUtils::sort(unsorted2, lemonBox2, citrusBox2, berryBox2);
        
        std::cout << "\nAfter sort (overflow test):" << std::endl;
        std::cout << "Lemon box (capacity 1): " << lemonBox2.nbFruits() << " fruits" << std::endl;
        std::cout << "Citrus box (capacity 2): " << citrusBox2.nbFruits() << " fruits" << std::endl;
        std::cout << "Berry box (capacity 1): " << berryBox2.nbFruits() << " fruits" << std::endl;
        std::cout << "Unsorted (overflow fruits): " << unsorted2.nbFruits() << " fruits" << std::endl;
        std::cout << "Remaining in unsorted: " << unsorted2 << std::endl;
    }

    std::cout << "\n=== Testing duplicate prevention ===" << std::endl;
    {
        FruitBox box(3);
        IFruit *fruit1 = new Orange();
        IFruit *fruit2 = new Orange();
        bool added1 = box.pushFruit(fruit1);
        bool added2 = box.pushFruit(fruit1);

        std::cout << "First add: " << (added1 ? "Success" : "Failed") << std::endl;
        std::cout << "Second add (same fruit): " << (added2 ? "Success" : "Failed") << std::endl;
        std::cout << "Box contains: " << box << std::endl;
        delete fruit2;
    }

    std::cout << "\n=== All tests completed successfully ===" << std::endl;
    return 0;
}
