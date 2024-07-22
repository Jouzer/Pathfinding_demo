#include "gui.h"
#include "astar.h"
#include <iostream>
#include <stdio.h>

/* Sprites are opensource. Credit for sprites George Bailey
https://opengameart.org/content/16x16-game-assets */

/* Demo to implement A-star pathfinding with SDL2 GFX 
TO-DO:
	* Flags
IDEAS/NOTES:
	* Map Value checking, other than 0 or 1 error
	* Accept smaller maps, ROW and COL maximum size
	* Resize window on load map
	* I'd rather have the result of an input action outside of SDLGui, so that SDLGui::handleInput would only return the x and y of mouse etc.
	  but it serves no purpose in this demo and rn I will not spend time on it   
	* Play more with return objects; text creation could be a function, also handle input shouldn't contain logic, but just return the input
	* The tiles you used were free to use but I forgot to save the author for credits, look it up or change
	* Conversion to SDL_RenderDrawLines and converting the stack to the proper format would be neater and possibly more efficient
*/

int main(int argc, char* argv[]) {
	bool running = true;
	SDLGui gui;
	printf("%d\n",gui.initSDL());
	if (gui.initSDL() == -1) { running = false; }
	int testmap[ROW][COL] = {
	{ 1, 0, 1, 1, 1, 1, 0, 1, 1, 1 },
	{ 1, 1, 1, 0, 1, 1, 1, 0, 1, 1 },
	{ 1, 1, 1, 0, 1, 1, 0, 1, 0, 1 },
	{ 0, 0, 1, 0, 1, 0, 0, 0, 0, 1 },
	{ 1, 1, 1, 0, 1, 1, 1, 0, 1, 0 },
	{ 1, 0, 1, 1, 1, 1, 0, 1, 0, 0 },
	{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
	{ 1, 0, 1, 1, 1, 1, 0, 1, 1, 1 },
	{ 1, 1, 1, 0, 0, 0, 1, 0, 0, 1 }};
	gui.loadMap(testmap);
	gui.printMap(); // print to console test
	
	// main loop
	while (running) {
		//handle inputs
		switch (gui.handleInput()) {
			case SDL_QUIT:
				running = false;
				break;
		}

		//render graphics
		gui.render();
	}
	return 0;
}