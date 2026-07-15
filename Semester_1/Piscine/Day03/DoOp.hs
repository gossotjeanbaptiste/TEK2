{-
-- EPITECH PROJECT, 2026
-- Day03
-- File description:
-- DoOp
-}

myElem :: Eq a => a -> [a] -> Bool
myElem _ [] = False
myElem element (x:xs)
    | element == x = True
    | otherwise = myElem element xs 

safeDiv :: Int -> Int -> Maybe Int 
safeDiv _ 0 = Nothing
safeDiv a b = Just (a `div` b)
