# Reversi
A simple Reversi game

# Installation
1. Download the game
2. Navigate to the directory you downloaded the game to.
3. Use make to compile the game
4. Use make play to compile and start the game.

	This can fail, if the GameID provided with the makefile is already used.
	You can avoid this to fail by replacing the ID in the makefile with your own.

# Playing
1. Navigate to the directory you installed the game to.
2. Use ./reversi to start the game

	You have several options to custmise your experience:
	
		-g <11 digits>:	Provide the ID of the game you want to join.
		
		-c <256 characters max>: 	You can provide an external configuration file here.
				The filepath can only include a maximum of 256 characters.
		
		-d <integer>:	You can choose the difficulty of the AI here.
				Currently a range from 0 to 2 is supported, where 0 is the easiest AI.
