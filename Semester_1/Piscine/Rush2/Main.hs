{-
-- EPITECH PROJECT, 2026
-- pushswap_checker
-- File description:
-- Main entry point
-}

module Main where

import PushSwapChecker (runChecker)
import System.Environment (getArgs)
import System.IO (hGetContents, stdin)

main :: IO ()
main = do
  args <- getArgs
  input <- hGetContents stdin
  runChecker args input
