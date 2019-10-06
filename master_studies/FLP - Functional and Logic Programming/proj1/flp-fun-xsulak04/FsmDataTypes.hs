---------------------------------------------------
--
-- Name of the project		rka-2-dka
-- Author					Ladislav Sulak
-- Login 					xsulak04
-- Date						4/2016
-- File 					FsmDataTypes.hs
-- Description				Project in Haskell for subject FLP (Funkcionální a logické programování)
--							The aim of this project is to convert the input FSM to a deterministic one.
--
----------------------------------------------------

-- export everything in this module
module FsmDataTypes where
import Data.List -- intercalate

-- synonyms help to understand semantics
type FSMInput = String
type FSMState = String
type FSMSymbol = Char


-- Structure for transitions. Show changed because of specification of this project.
data Transition = Trans
	{ fromState :: FSMState
	, fromSymbol :: FSMSymbol
	, toState :: FSMState
	} deriving (Eq)

instance Show Transition where
	show (Trans fstate fsymbol tstate) = 
		if fsymbol == '-' 
			then "\n" ++ printList [fstate, [], tstate]
			else "\n" ++ printList [fstate, [fsymbol], tstate]


-- Structure represents entire FSM including alphabet, because of Determiniz. algorithm.
-- Show also changed: All States \n Start State \n Final State \n Transitions
data FSMachine = FSM
	{ states :: [FSMState]
	, start :: FSMState
	, final :: [FSMState]
	, alphabet:: [FSMSymbol]
	, trans :: [Transition]
	} deriving (Eq)

instance Show FSMachine where
	show (FSM states start final alphabet trans) =
		printList states ++ "\n" ++
		start ++ "\n" ++ printList final ++
		showTrans trans


-- Some functions for converting List to format we like
printList :: [String] -> String
printList xs = intercalate "," xs -- Merge strings in list with each other, stick with comma

showTrans :: Show a => [a] -> String
showTrans = intercalate "" . map show
