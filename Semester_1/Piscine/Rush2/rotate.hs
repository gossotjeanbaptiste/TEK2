{-
-- EPITECH PROJECT, 2026
-- Rush2
-- File description:
-- rotate
-}

module Rotate (ra, rb, rr) where

ra :: [a] -> [a]
ra [] = []
ra (x:xs) = xs ++ [x]

rb :: [a] -> [a]
rb [] = []
rb (x:xs) = xs ++ [x]

rr :: [a] -> [a] -> ([a], [a])
rr a b = (ra a, rb b)
