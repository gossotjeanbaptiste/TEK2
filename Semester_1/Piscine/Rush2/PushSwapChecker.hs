{-
-- EPITECH PROJECT, 2026
-- pushswap_checker
-- File description:
-- PushSwap checker core
-}

module PushSwapChecker (runChecker) where

import Control.Monad (foldM)
import Data.Char (isSpace)
import Data.List (nub)
import System.Exit (ExitCode (ExitFailure), exitWith)
import System.IO (hPutStrLn, stderr)
import Position (pa, pb)
import qualified Rotate as R
import qualified Swap as S
import qualified ReverseReverse as RR

type Stack = [Int]

type Stacks = (Stack, Stack)

minInt32 :: Integer
minInt32 = -2147483648

maxInt32 :: Integer
maxInt32 = 2147483647

runChecker :: [String] -> String -> IO ()
runChecker args input =
  either exit84 printResult (computeFinalStacks args input)

exit84 :: String -> IO a
exit84 msg = hPutStrLn stderr msg >> exitWith (ExitFailure 84)

printResult :: Stacks -> IO ()
printResult (finalA, finalB)
  | isSorted finalA && null finalB = putStrLn "OK"
  | otherwise = putStrLn (formatKo finalA finalB) >> exitWith (ExitFailure 84)

formatKo :: Stack -> Stack -> String
formatKo stackA stackB =
  "KO: (" ++ show stackA ++ "," ++ show stackB ++ ")"

computeFinalStacks :: [String] -> String -> Either String Stacks
computeFinalStacks args input = do
  initialA <- parseArgsFix args
  applyOperations (words input) (initialA, [])

readInt32 :: String -> Maybe Int
readInt32 s =
  case reads s :: [(Integer, String)] of
    [(n, rest)]
      | isValidRead n rest -> Just (fromInteger n)
    _ -> Nothing

isValidRead :: Integer -> String -> Bool
isValidRead n rest = all isSpace rest && isInInt32Range n

isInInt32Range :: Integer -> Bool
isInInt32Range n = n >= minInt32 && n <= maxInt32

parseArgsFix :: [String] -> Either String [Int]
parseArgsFix args = do
  numbers <- traverse parseNumber args
  ensureNoDuplicates numbers
  pure numbers

parseNumber :: String -> Either String Int
parseNumber tok =
  maybe (Left (formatInvalidInteger tok)) Right (readInt32 tok)

formatInvalidInteger :: String -> String
formatInvalidInteger tok = "Error: invalid integer '" ++ tok ++ "'"

ensureNoDuplicates :: [Int] -> Either String ()
ensureNoDuplicates numbers
  | length numbers == length (nub numbers) = Right ()
  | otherwise = Left "Error: duplicate values"

opSa :: Stacks -> Stacks
opSa (a, b) = (S.sa a, b)

opSb :: Stacks -> Stacks
opSb (a, b) = (a, S.sb b)

opSc :: Stacks -> Stacks
opSc (a, b) = S.sc a b

opPa :: Stacks -> Stacks
opPa = pa

opPb :: Stacks -> Stacks
opPb = pb

opRa :: Stacks -> Stacks
opRa (a, b) = (R.ra a, b)

opRb :: Stacks -> Stacks
opRb (a, b) = (a, R.rb b)

opRr :: Stacks -> Stacks
opRr (a, b) = R.rr a b

opRra :: Stacks -> Stacks
opRra (a, b) = (RR.rra a, b)

opRrb :: Stacks -> Stacks
opRrb (a, b) = (a, RR.rrb b)

opRrr :: Stacks -> Stacks
opRrr (a, b) = RR.rrr a b

applyOperations :: [String] -> Stacks -> Either String Stacks
applyOperations ops stacks = foldM applyOperation stacks ops

applyOperation :: Stacks -> String -> Either String Stacks
applyOperation stacks op =
  case lookup op operationTable of
    Just f -> Right (f stacks)
    Nothing -> Left (formatUnknownOperation op)

formatUnknownOperation :: String -> String
formatUnknownOperation op = "Error: unknown operation '" ++ op ++ "'"

swapOps :: [(String, Stacks -> Stacks)]
swapOps = [("sa", opSa), ("sb", opSb), ("sc", opSc)]

moveOps :: [(String, Stacks -> Stacks)]
moveOps = [("pa", opPa), ("pb", opPb)]

rotateOps :: [(String, Stacks -> Stacks)]
rotateOps = [("ra", opRa), ("rb", opRb), ("rr", opRr)]

revRotateOps :: [(String, Stacks -> Stacks)]
revRotateOps = [("rra", opRra), ("rrb", opRrb), ("rrr", opRrr)]

operationTable :: [(String, Stacks -> Stacks)]
operationTable =
  swapOps ++ moveOps ++ rotateOps ++ revRotateOps

isSorted :: Stack -> Bool
isSorted [] = True
isSorted [_] = True
isSorted (x : y : rest) = x <= y && isSorted (y : rest)
