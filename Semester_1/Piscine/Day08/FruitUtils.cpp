#include "FruitUtils.hpp"
#include "Lemon.hpp"
#include "ACitrus.hpp"
#include "ABerry.hpp"

void FruitUtils::sort(FruitBox &unsorted, FruitBox &lemon, FruitBox &citrus, FruitBox &berry)
{
    FruitBox temp(unsorted.getSize());
    
    // Déplacer tous les fruits de unsorted à temp
    IFruit *fruit;
    while ((fruit = unsorted.popFruit()) != nullptr) {
        temp.pushFruit(fruit);
    }
    
    // Trier les fruits de temp vers les bonnes boîtes
    while ((fruit = temp.popFruit()) != nullptr) {
        // Vérifier si c'est un Lemon
        if (dynamic_cast<Lemon *>(fruit) != nullptr) {
            if (!lemon.pushFruit(fruit)) {
                unsorted.pushFruit(fruit);
            }
        }
        // Vérifier si c'est un ACitrus (mais pas un Lemon)
        else if (dynamic_cast<ACitrus *>(fruit) != nullptr) {
            if (!citrus.pushFruit(fruit)) {
                unsorted.pushFruit(fruit);
            }
        }
        // Vérifier si c'est un ABerry
        else if (dynamic_cast<ABerry *>(fruit) != nullptr) {
            if (!berry.pushFruit(fruit)) {
                unsorted.pushFruit(fruit);
            }
        }
        // Sinon, remettre dans unsorted
        else {
            unsorted.pushFruit(fruit);
        }
    }
}

FruitBox **FruitUtils::pack(IFruit **fruits, unsigned int boxSize)
{
    // Compter le nombre de fruits
    unsigned int fruitCount = 0;
    for (unsigned int i = 0; fruits[i] != nullptr; i++) {
        fruitCount++;
    }
    
    // Calculer le nombre de boîtes nécessaires
    unsigned int boxCount = (fruitCount + boxSize - 1) / boxSize;  // Arrondir vers le haut
    
    // Allouer le tableau de boîtes (boxCount + 1 pour le nullptr terminateur)
    FruitBox **boxes = new FruitBox *[boxCount + 1];
    
    // Créer les boîtes et y ajouter les fruits
    unsigned int fruitIndex = 0;
    for (unsigned int i = 0; i < boxCount; i++) {
        boxes[i] = new FruitBox(boxSize);
        
        // Remplir cette boîte
        for (unsigned int j = 0; j < boxSize && fruits[fruitIndex] != nullptr; j++) {
            boxes[i]->pushFruit(fruits[fruitIndex]);
            fruitIndex++;
        }
    }
    
    // Ajouter le nullptr terminateur
    boxes[boxCount] = nullptr;
    
    return boxes;
}

IFruit **FruitUtils::unpack(FruitBox **fruitBoxes)
{
    // Compter le nombre total de fruits
    unsigned int totalFruits = 0;
    for (unsigned int i = 0; fruitBoxes[i] != nullptr; i++) {
        totalFruits += fruitBoxes[i]->nbFruits();
    }
    
    // Allouer le tableau de fruits (totalFruits + 1 pour le nullptr terminateur)
    IFruit **fruits = new IFruit *[totalFruits + 1];
    
    // Extraire tous les fruits
    unsigned int fruitIndex = 0;
    for (unsigned int i = 0; fruitBoxes[i] != nullptr; i++) {
        IFruit *fruit;
        while ((fruit = fruitBoxes[i]->popFruit()) != nullptr) {
            fruits[fruitIndex++] = fruit;
        }
    }
    
    // Ajouter le nullptr terminateur
    fruits[fruitIndex] = nullptr;
    
    return fruits;
}

