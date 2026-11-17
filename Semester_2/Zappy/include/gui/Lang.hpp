/*
** EPITECH PROJECT, 2026
** Lang.hpp
** File description:
** Display language of the GUI (French default, English option)
*/

#pragma once

#include <cstdlib>

// Langue des textes affichés par le GUI. Au démarrage : ZAPPY_LANG=en (ou
// EN, english...) pour l'anglais, français sinon. La touche L bascule en
// cours de partie (Lang::toggle).
namespace Lang {
    inline bool &englishFlag()
    {
        static bool flag = [] {
            const char *lang = std::getenv("ZAPPY_LANG");
            return lang && (lang[0] == 'e' || lang[0] == 'E');
        }();
        return flag;
    }

    inline bool english() { return englishFlag(); }
    inline void setEnglish(bool on) { englishFlag() = on; }
    inline void toggle() { englishFlag() = !englishFlag(); }

    // Choisit le texte selon la langue : T("français", "english").
    inline const char *T(const char *fr, const char *en)
    {
        return english() ? en : fr;
    }
}
