{-
-- EPITECH PROJECT, 2026
-- Day02
-- File description:
-- My
-}

mySucc :: Int -> Int
mySucc x = x + 1

myIsNeg :: Int -> Bool
myIsNeg x = x < 0

myAbs :: Int -> Int
myAbs x | x >= 0 = x
        | otherwise = -x

myMin :: Int -> Int -> Int
myMin x y | x <= y = x
          | otherwise = y

myMax :: Int -> Int -> Int
myMax x y | x >= y = x
          | otherwise = y 

myTuple :: a -> b -> (a, b)
myTuple a b = (a, b)

myTruple :: a -> b -> c -> (a, b, c)
myTruple a b c = (a, b, c)

myFst :: (a, b) -> a
myFst (a, b) = a

mySnd :: (a, b) -> b
mySnd (a, b) = b

mySwap :: (a, b) -> (b, a)
mySwap (a, b) = (b, a)

myHead :: [a] -> a
myHead [] = error "Empty list"
myHead (x:_) = x

myTail :: [a] -> [a]
myTail [] = error "Empty list"
myTail (x:y) = y

myLength :: [a] -> Int
myLength [] = 0
myLength (_:y) = 1 + myLength y

myNth :: [a] -> Int -> a
myNth [] _ = error "Index out of bounds"
myNth (x:_) 0 = x
myNth (_:xs) n | n < 0 = error "Negative index"
                | otherwise = myNth xs (n - 1)

myTake :: Int -> [a] -> [a]
myTake _ [] = []
myTake n _ | n <= 0 = []
myTake n (x:xs) = x : myTake (n - 1) xs

myDrop :: Int -> [a] -> [a]
myDrop _ [] = []
myDrop n xs@(_:xs')
        | n > 0 = myDrop (n-1) xs'
        | otherwise = xs

myAppend :: [a] -> [a] -> [a]
myAppend [] ys = ys
myAppend (x:xs) ys = x : myAppend xs ys

myReverse :: [a] -> [a]
myReverse list = rev list []
        where
                rev [] acc = acc
                rev (x:xs) acc = rev xs (x:acc)

myInit :: [a] -> [a]
myInit [] = error "list empty"
myInit [x] = []
myInit (x:xs) = x : myInit xs