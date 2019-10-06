---------------------------------------------------
--
-- Name of the project		rka-2-dka
-- Author					Ladislav Sulak
-- Login 					xsulak04
-- Date						4/2016
-- File 					FsmProcessing.hs
-- Description				Project in Haskell for subject FLP (Funkcionální a logické programování)
--							The aim of this project is to convert the input FSM to a deterministic one.
--
----------------------------------------------------

-- export only selected functions
module FsmProcessing
( getFSM
, simulateFSM
, dumpFSM
) where

-- library imports
import System.IO
import Data.List
import Data.List.Split
import Data.String

import Debug.Trace

-- our modules imports
import FsmDataTypes

getFSM :: String -> IO FSMachine
getFSM content = do
	let lns = lines content
	let fsm = procLns lns
 	--print $ concat $ (map (\x -> if (null $ head $ drop 1 (splitOn "," x)) then "-" else head $ drop 1 (splitOn "," x)) ["1,,3", "a,b,c"])

 	--let transTmp = 
 	print "EXPERIMENTY"


 	--print (det (start fsm) (trans fsm))
 	
 	--print ((\x -> getEpsClosure x (trans fsm)) ["1", "2"])


 	--print ((\x -> [("_1")] == x) ["_1", "a", "_1"])


 	--print $ concat $ map (\x -> 
	--		 (head $ drop 1 (splitOn "," x) )) (trans fromState fsm)

 	{-print (
 		if( ((\(a:xs) -> (fromSymbol a ) ) (trans fsm))) == '-'
 			then
 				"ANO"
 			else
 				"NIE")
 	--print $ concat $ map (\x -> 
	--		 (head $ drop 1 (splitOn "," x) )) (trans fromState fsm)
	-}
	putStrLn "\nStavy"
	print (states fsm)
	putStrLn ""

	putStrLn "Abeceda"
	print (alphabet fsm)
	putStrLn ""

	if null (nub (alphabet fsm) \\ ['a'.. 'z']) 
		then
		--	print (trans fsm)
			return fsm
		else
			error "An error, the alphabet from the input FSM is not from 'a' to 'z'."
	
		where
			--det :: FSMState -> [Transition] -> [[String]]
			--det startq (x:xs) = do
			--	let eNodeStart = [(getEpsClosure start (x:xs))]
			--	let allStates = findAllStates eNodeStart (x:xs)

			-- Find e-closure for start symbol and launch a determinization
			test :: String -> [FSMSymbol] -> [Transition] -> [[String]]
			test start alphabet trans = do
				[["A"]]
				{-
				let firstEz = (getEpsClosureFirst start trans)
				
				let epsClosureList = ("1", firstEz):[]
				let toProcess = findNewStates "1" alphabet  
				
				findFinal epsClosureList toProcess alphabet trans
-}
				--(head (fst epsClosureList))  
				
				--(show toProcess)

				--let statesAll = trace ("TU SME: " ++ (show firstEz)) $ findAllStates firstEz trans
				
			--findFinal :: [(FSMState, [FSMState])] -> [(FSMState, FSMSymbol)] -> [FSMSymbol] -> [Transition] -> [[FSMState]]
			findFinal _ [] _ _ _= []
			findFinal epsList procList alph (x:xs) ret = do					
				let toEps = trace "start" $ finish (snd $ head $ epsList) (snd $ head $ procList) (x:xs)
				--toEps
				let tmp = trace (show (((procList)))) $  getEpsClosure (head toEps) (x:xs)
				---tmp
				--(show ((show (length epsList + 1), tmp):epsList))
				let procList2 = procList ++ (findNewStates (show ((length epsList)+1)) alph) 
				let epsList2 = trace (show procList2) $ ((show (length epsList + 1), tmp):epsList)
				let retNext = (toEps, (snd $ head $ procList), tmp, (show ((length epsList)+1))):ret
			{-	[] : -}
				findFinal epsList2 (delete (head procList2) procList2) alph (x:xs) retNext


			finish _ _ [] = []
			finish states symbol (x:xs) = 
				if(( head states) == (fromState x)) then
					if (symbol == fromSymbol x)	then
						[toState x] : finish states symbol xs
					else
						finish states symbol xs
				else 
					finish states symbol xs






			findNewStates :: FSMState -> [FSMSymbol] -> [(FSMState, FSMSymbol)]
			findNewStates _ [] = []
			findNewStates i (alpha:x) = (i, alpha) : findNewStates i x

				--determinize statesAll trans

			-- napr [[_1, a, 3], [_1, b, 2]] - stavy. Postupne nad nimi sprav E-uzaver
			determinize :: [[String]] ->  [Transition]-> [[String]]
			determinize [] trans = []
			determinize (x:xs) trans = do 
				let firstEz = (getEpsClosure x trans) -- napr teda [_1, a, 3] TODO - to HEAD JE LEN NA SKUSKU
				let statesAll = findAllStates firstEz  trans -- napr empty
				statesAll	
				--(firstEz) : (determinize (xs ++ statesAll) trans)
			
				--[(firstEz)] : (determinize (xs, statesAll) trans)

			{-
			--determinize (x:[]) trans = do --Only 1 input symbol
			--	let firstEz = (getEpsClosure x trans) -- E-closure
			--	let nextEz = findAllStates firstEz trans -- Find states from the current one
			--	(firstEz : determinize (nextEz) trans)
			
			determinize [] trans = []
			determinize (x:xs) trans = do 
				findNewState x 
				let firstEz = (getEpsClosure x trans)
				let statesAll = findAllStates firstEz trans
				(firstEz : (determinize statesAll trans))
-}	
			-- napr dostane [_1, a, 3] a z toho spravi [_1, a, 3] (teda nic nove, nieje tam '-')
			--getEpsClosure:: [String] -> [Transition] ->  [String]
			getEpsClosure sym [] = sym -- TODO - sym je list, da sa cez neho iterovat!

			--getEpsClosure (st1:stN) [] = getEpsClosure stN []
			--getEpsClosure [] (x:xs) = 
			getEpsClosure sym (x:xs) = 
				if (compare sym x) -- iteruje cez vsetky prechody a ak najde ten zaciatocny...
					then
						if (fromSymbol x == '-') -- Eps prechod
							then
								nub $ (toState x) :  getEpsClosure sym xs -- Save trans and do next
							else
								getEpsClosure sym xs -- Next trans
					else
						getEpsClosure sym xs -- Next trans


			compare :: [String] -> Transition -> Bool
			compare [] tran = False
			compare (x:xs) tran =
				if(( (fromState tran)) == x) 
					then
						True
					else
						compare xs tran

			--getEpsClosureFirst :: String -> [Transition] -> [String]
			getEpsClosureFirst sym [] = [sym]
			getEpsClosureFirst sym (x:xs) = 
				if ((fromState x) == sym)
					then
						if ([fromSymbol x] == "-") -- Eps prechod
							then
								nub $ (toState x) :  getEpsClosureFirst sym xs -- Save trans and do next
							else
								getEpsClosureFirst sym xs -- Next trans
					else
						getEpsClosureFirst sym xs -- Next trans


			findAllStates :: [String] -> [Transition] -> [[String]]
			findAllStates [] [] = []
			findAllStates [start] [] = []
			findAllStates [start] (x:xs) = do
				let tmpState = what start
				if (( fromState x) == start)
					then
						if ([fromSymbol x] /= "-") 
							then
								[ "_" ++ start, [fromSymbol x], (toState x) ] : findAllStates [start] xs
							else
								findAllStates [start] xs
					else
						findAllStates [start] xs
			findAllStates _ _ = []
			what :: String -> IO ()
			what state = do
				print state 

-- process input lines by dividing them into 4 parts
procLns :: [String] -> FSMachine
procLns (states:start:final:trans) =
	if null trans
		then 
			error "An error, in the input file there are no transitions."
		else 
			FSM (geStates states) start (geStates final) (getAlphabet) (map getRule trans) 
	where
		geStates st = splitOn "," st

		getAlphabet = nub $ concat $ map (\x -> 
			 (tmpList x)) trans
			where
				tmpList :: String -> String
				tmpList str = head $ drop 1 (splitOn "," str)

		getRule rule = getRule' $ splitOn "," rule
		getRule' :: [String] -> Transition
		getRule' [q1,[sym],q2] = 
			Trans q1 (
				if sym == '-' 
					then  error "An error, the alphabet from the input FSM is not from 'a to 'z'." 
					else sym
				) q2
		getRule' [q1,[],q2] = Trans q1 '-' q2
		getRule' _ = error "An error, the transitions in the input file are in bad format."
procLns _ = error "An error, the input file is in bad format."


-- dump TS to stdout if option '-i' selected
dumpFSM :: FSMachine -> IO ()
dumpFSM fsm = do
	print fsm
	--print (alphabet fsm)

simulateFSM :: FSMachine -> IO ()
simulateFSM fsm = do
	--putStrLn " == Simulating TS == \n"
	let fsmDeterm = runDeterm fsm

	--putStrLn "Stavy"
	--print (start fsmDeterm)
	--putStrLn ""

	dumpFSM fsmDeterm
	--print (alphabet fsm)
	--putStrLn "\n == Determinized =="


runDeterm :: FSMachine -> FSMachine
runDeterm fsm = do
	let eUz = detStartEps (start fsm)
	FSM detStates eUz detFinal (alphabet fsm) detTrans
	where
		detStartEps :: FSMState -> FSMState
		detStartEps state = "1"--concat $ map (\x -> 
			 --(head $ drop 1 (splitOn "," x) )) (trans fsm)

		detStates = (states fsm) --["10","212"]
		detStart  = (start fsm)
		detFinal  = (final fsm)
		detTrans  = (trans fsm)
	--(states fsm) (start fsm) (final fsm) (trans fsm)
	--putStrLn (start fsm)
	--putStrLn (final fsm)


{-
output :: FSMachine -> IO ()
output ts = do
	putStrLn ts.start

-- simulate TS on input
simulateTuringMachine :: FSMachine -> FSMInput -> IO ()
simulateTuringMachine ts input = do
	putStrLn "\nsimulating TS ...\n"
	runUTM (trans ts) (start ts) (end ts)-- (Tape '$' [] input)  -- rules expect $ as the first tape symbol

-- run TM in UTM
runUTM :: [Transition] -> FSMState -> FSMState -> IO () --Tape FSMSymbol -> IO ()
runUTM rules start stop --tape@(Tape x lts rts)
	| start == stop = do
		--printTMconfig start --tape
		putStrLn "\n===> ACCEPT\n"
	| otherwise = do
	---	printTMconfig start-- tape
		let apply = findRule rules start --x
		putStrLn $ (show apply) ++ "\n"
		runUTM rules (toState apply) 
		--runUTM rules (toState apply) stop (step (toAction apply) tape)

-- find rule for state and symbol
findRule :: [Transition] -> FSMState -> FSMSymbol -> Transition
findRule [] _ _ = error "no rule found"
findRule (r:rs) q x = if (fromState r) == q && (fromSymbol r) == x then r else findRule rs q x

-- do one step on tape
--step :: Action -> Tape FSMSymbol -> Tape FSMSymbol
--step (ALeft)    (Tape _ [] _)      = error "can not move left"
--step (ALeft)    (Tape x (l:ls) rs) = Tape l ls (x:rs)
--step (ARight)   (Tape x ls [])     = Tape '$' (x:ls) []
--step (ARight)   (Tape x ls (r:rs)) = Tape r (x:ls) rs
-- WTF step (AWrite w) (Tape _ ls rs)     = Tape w ls rs

-- debug function for TS config printing
--printTMconfig :: FSMState -> FSMSymbol -> IO ()
--printTMconfig q  = do putStrLn ++ q 


-}