/*
** EPITECH PROJECT, 2026
** GpuDraw.hpp
** File description:
** GPU-side drawing helpers: instanced models and GPU skinning
*/

#pragma once

#include <raylib.h>
#include <string>

// Le mode immédiat de raylib (DrawCube, DrawModel...) coûte un appel de
// dessin par objet, et chaque appel passe par le CPU. Ces aides regroupent
// le travail côté GPU :
//   - instanciation : N copies d'un modèle en un appel par mesh ;
//   - skinning GPU : l'animation des personnages est calculée dans le
//     vertex shader au lieu de recalculer et renvoyer tous les sommets à
//     chaque frame, et leurs meshes sont fusionnés en un seul.
// Si un shader ne compile pas (pilote trop ancien), tout repasse par les
// fonctions raylib classiques : le rendu reste identique, juste plus lent.
namespace GpuDraw {
    // À appeler après InitWindow (contexte OpenGL requis), avant le
    // chargement des modèles. unload() avant CloseWindow.
    void load();
    void unload();

    bool instancingAvailable();
    bool skinningAvailable();
    // Rendu OpenGL logiciel (llvmpipe...) : le GPU n'est pas utilisé.
    bool softwareRenderer();
    const std::string &rendererName();

    // Matrice équivalente à DrawModelEx(model, position, axe Y, yawDeg,
    // scale, ...), transformation propre du modèle comprise.
    Matrix modelTransform(const Model &model, Vector3 position, float yawDeg,
                          Vector3 scale);

    // Dessine count copies du modèle (matrices issues de modelTransform),
    // teinte appliquée comme DrawModelEx.
    void drawModelInstanced(const Model &model, const Matrix *transforms,
                            int count, Color tint);

    // Prépare un personnage animé : fusionne ses meshes en un seul (couleur
    // du matériau stockée par sommet) et, si le GPU s'en charge, lui assigne
    // le shader de skinning. Renvoie true en skinning GPU : le modèle
    // s'anime alors avec UpdateModelAnimationBones. Sinon (rendu logiciel,
    // shader indisponible, modèle atypique), il s'anime comme avant avec
    // UpdateModelAnimation.
    bool prepareSkinnedModel(Model &model);
}
