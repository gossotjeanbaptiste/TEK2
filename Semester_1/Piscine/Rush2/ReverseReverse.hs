{-
-- EPITECH PROJECT, 2026
-- G-PDG-300-NCY-3-1-PDGRUSH2-4
-- File description:
-- ReverseReverse
-}

module ReverseReverse where

rra :: [a] -> [a]
rra [] = []
rra a = last a : init a

rrb :: [a] -> [a]
rrb [] = []
rrb b = last b : init b

rrr :: [a] -> [a] -> ([a], [a])
rrr a b = (rra a, rrb b)