---------------------------------------------------
--
-- Name of the project		rka-2-dka
-- Author					Ladislav Sulak
-- Login 					xsulak04
-- Date						4/2016
-- File 					FsmMain.hs
-- Description				Project in Haskell for subject FLP (Funkcionální a logické programování)
--							The aim of this project is to convert the input FSM to a deterministic one.
--
----------------------------------------------------

module Main(main) where

-- library imports
import System.IO
import System.Environment -- getArgs
import System.Exit

import Data.Char
import Data.List
import Data.List.Split
import Data.String

-- our imports
import FsmDataTypes
import FsmProcessing


main :: IO ()
main = do
	args <-	getArgs
	let (determinize, input) = procArgs args 

	if null input -- If there is no input file, then load from stdin.
		then do
			contents <- hGetContents stdin
			fsm <- getFSM contents -- Create FSM object from stdin 

			if determinize
				then simulateFSM fsm -- Determinization
				else dumpFSM fsm 	 -- Just parse input, fill structures and output it

		else do
			inputHandle <- openFile input ReadMode
			contents <- hGetContents inputHandle
			fsm <- getFSM contents -- Create FSM object from stdin 

			if determinize
				then simulateFSM fsm -- Determinization
				else dumpFSM fsm 	 -- Just parse input, fill structures and output it

			hClose inputHandle
	
	return ()

-- @synopsys Function handles the input arguments
-- @param  	A list of arguments.
-- @returns A couple,
--				* first item is a boolean flag, which indicates a type of processing: 
-- 					True 	- determinize, 
--					False 	- just parse the input file and output its FSM
-- 				* second item is name of the input file or an empty list - stdin  
procArgs :: [String] -> (Bool, String)
procArgs []  = error "There was an error in arguments, requiered at least 1."
procArgs [x]
	| x == "-i" = (False, [])
	| x == "-t" = (True,  [])
	| otherwise = error "You have specified an unknown argument."
procArgs [x,y]
	| x == "-i" = (False, y)
	| x == "-t" = (True,  y)
	| otherwise = error "You have specified an unknown argument."
procArgs _ = error "There was an error in arguments, please provide 2."
