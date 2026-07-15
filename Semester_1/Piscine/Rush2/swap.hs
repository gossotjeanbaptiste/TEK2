{-
-- EPITECH PROJECT, 2026
-- Rush2
-- File description:
-- swap
-}

module Swap (sa, sb, sc) where

sa :: [a] -> [a]
sa (x:y:xs) = y:x:xs
sa xs = xs

sb :: [a] -> [a]
sb (x:y:xs) = y:x:xs
sb xs = xs

sc :: [a] -> [a] -> ([a], [a])
sc a b = (sa a, sb b)
