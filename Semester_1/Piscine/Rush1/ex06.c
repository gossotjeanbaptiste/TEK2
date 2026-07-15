/*
** EPITECH PROJECT, 2026
** rush1
** File description:
** Main test for List with 0 memory leaks
*/

#include <stdio.h>
#include <stdlib.h>
#include "G-PDG-300_rush1/new.h"
#include "list.h"
#include "G-PDG-300_rush1/int.h"

int main(void)
{
    Object *list = new(List, 3, Int, 10);
    if (!list)
        return (1);

    printf("--- Test 1: Contenu initial (size: %zu) ---\n", len(list));

    Iterator *it = begin(list);
    Iterator *ite = end(list);

    while (lt(it, ite)) {
        char *s = str(getval(it));
        printf("%s\n", s);
        free(s); 

        incr(it);
    }
    delete(it);
    delete(ite);
    printf("\n--- Test 2: Modification de l'index 1 (10 -> 42) ---\n");
    setitem(list, 1, 42); 
    printf("\n--- Test 3: Parcours final ---\n");
    it = begin(list);
    ite = end(list);
    while (lt(it, ite)) {
        char *s = str(getval(it));
        printf("%s\n", s);
        free(s);
        incr(it);
    }
    delete(it);
    delete(ite);
    
    printf("\n--- Test 4: Ajout d'éléments avec list_add ---\n");
    addition(list, new(Int, 100));
    addition(list, new(Int, 200));
    addition(list, new(Int, 300));
    printf("Taille après ajout: %zu\n", len(list));
    
    printf("\n--- Test 5: Parcours après ajout ---\n");
    it = begin(list);
    ite = end(list);
    while (lt(it, ite)) {
        char *s = str(getval(it));
        printf("%s\n", s);
        free(s);
        incr(it);
    }
    delete(it);
    delete(ite);
    
    printf("\n--- Test 6: Ajout sur liste vide ---\n");
    Object *empty_list = new(List, 0, Int);
    if (!empty_list)
        return (1);
    printf("Taille liste vide: %zu\n", len(empty_list));
    addition(empty_list, new(Int, 42));
    printf("Taille après ajout: %zu\n", len(empty_list));
    it = begin(empty_list);
    ite = end(empty_list);
    while (lt(it, ite)) {
        char *s = str(getval(it));
        printf("%s\n", s);
        free(s);
        incr(it);
    }
    delete(it);
    delete(ite);
    delete(empty_list);
    
    delete(list);
    printf("\nListe et contenu supprimés.\n");

    return (0);
}