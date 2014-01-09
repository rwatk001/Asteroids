   _____            __                      .__    .___      
  /  _  \   _______/  |_  ___________  ____ |__| __| _/______
 /  /_\  \ /  ___/\   __\/ __ \_  __ \/  _ \|  |/ __ |/  ___/
/    |    \\___ \  |  | \  ___/|  | \(  <_> )  / /_/ |\___ \ 
\____|__  /____  > |__|  \___  >__|   \____/|__\____ /____  >
        \/     \/            \/                     \/    \/ 

Clone of the original Atari video game programmed in C++
	also using OpenGL and freeGLUT libraries

Developed using Microsoft Visual C++ 2010 Express
	author: Ryan Watkins

Uses the Microsoft proprietary command Sleep() for timing
	When porting to Linux systems use usleep from 
	unistd.h library.

INCLUDES:
---------------------------------------------------------
	The whole Microsoft Visual c++ Solution folder
	Recommended that you compile and run using this IDE.

	main.cpp contains the while program.

CONTROLS:
---------------------------------------------------------
	MOUSE:
     		*use the mouse to aim (a cursor is displayed at
			at current mouse position)

	LEFT MOUSE BUTTON:
		*fires the gun for asteroid destruction

	SPACEBAR:
		*moves ship (be careful, movement is weird in space!)

	ESC:
		*pauses the game







___________           __              ._.
\_   _____/ ____     |__| ____ ___.__.| |
 |    __)_ /    \    |  |/  _ <   |  || |
 |        \   |  \   |  (  <_> )___  | \|
/_______  /___|  /\__|  |\____// ____| __
        \/     \/\______|      \/      \/