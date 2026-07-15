{-
-- EPITECH PROJECT, 2026
-- G-PDG-300-NCY-3-1-PDGRUSH2-4
-- File description:
-- Position
-}

module Position where

pa :: ([a], [a]) -> ([a], [a])
pa (a, []) = (a, [])
pa (a, x : bs) = (x : a, bs)

pb :: ([a], [a]) -> ([a], [a])
pb ([], b) = ([], b)
pb (x : as, b) = (as, x : b)