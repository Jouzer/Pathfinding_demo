
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include "gui.h"
#include "astar.h"


SDLGui::SDLGui() {
	win = NULL;
	renderer = NULL;

	// Array size in .h
	for (auto& texture : textures) {
		if (texture) {
			texture = NULL;
		}
	}

	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			map[i][j] = 0;
		}
	}

	beginPoint = { -1,-1 };
	endPoint = { -1,-1 };
	snprintf(textMessage, sizeof(textMessage), "Select route begin point");

}

SDLGui::~SDLGui() {
	for (auto& texture : textures) {
		if (texture) {
			SDL_DestroyTexture(texture);
		}
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	printf("SDL destructed\n");
}

int SDLGui::initSDL() {
	// Initialize SDL.
	if ((SDL_Init(SDL_INIT_VIDEO) < 0)) {
		printf("Could not initialize SDL: %s.\n", SDL_GetError());
		return(-1);
	}
	printf("SDL initialized.\n");

	if (TTF_Init() == -1) {
		printf("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
		return(-1);
	}
	// create the window and renderer
	win = SDL_CreateWindow("SDL2 + Astar pathfinding CPP demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT + 50, 0);
	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	printf("Window and renderer created\n");

	// Load the textures
	textures[0] = IMG_LoadTexture(renderer, "tile0.png");
	textures[1] = IMG_LoadTexture(renderer, "tile1.png");
	for (auto& texture : textures) {
		if (texture == nullptr) {
			printf("IMG_LoadTexture Error: %s \n", SDL_GetError());
			return(-1);
		}
	}
	printf("Textures loaded\n");

	// Load font
	font = TTF_OpenFont("DejaVuSansMono.ttf", 18);
	if (font == nullptr) {
		printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
		return(-1);
	}
	printf("Font loaded\n");
	return(1);

}

void SDLGui::loadMap(int newmap[ROW][COL]) {

	// Assign values using nested loops
	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			map[i][j] = newmap[i][j];
		}
	}
}

// Print map for debug
void SDLGui::printMap() {
	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			std::cout << map[i][j] << " ";
		}
		std::cout << std::endl;
	}
}


void SDLGui::render() {
	
	if (lastUpdateTime + 30 < SDL_GetTicks()) {
		lastUpdateTime = SDL_GetTicks();

		SDL_RenderClear(renderer);

		// Render map
		for (int i = 0; i < ROW; ++i) {
			for (int j = 0; j < COL; ++j) {
				// Create an SDL_Rect for the current tile
				SDL_Rect destRect = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };

				// Get the appropriate texture for the tile
				SDL_Texture* texture = textures[map[i][j]];

				// Render the tile
				SDL_RenderCopy(renderer, texture, nullptr, &destRect);
			}
		}

		// Render text message
		SDL_Surface* textSurface = TTF_RenderText_Solid(font, textMessage, textColor);
		if (textSurface == nullptr) {
			std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
		}
		// Create texture from surface pixels
		SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
		if (textTexture == nullptr) {
			printf("Unable to create texture from rendered text! SDL Error: %s \n", SDL_GetError());
		}
		else {
			SDL_Rect renderQuad = { 10, HEIGHT + 12, textSurface->w, textSurface->h };
			SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
		}

		SDL_FreeSurface(textSurface);
		SDL_DestroyTexture(textTexture);

		// Render flags at begin and end points

		// Render pathline if available
		drawPath(pathdata.path);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

		// Render the prepared view
		SDL_RenderPresent(renderer);

	}
}


void SDLGui::drawPath(std::stack<Pair> path) {
	SDL_SetRenderDrawColor(renderer, 255, 128, 128, 255);
	std::pair<int, int> oldP = { -1,-1 };
	while (!path.empty()) {
		std::pair<int, int> p = path.top();
		path.pop();
//		printf("-> (%d,%d) ", p.second, p.first);
		
		if (oldP.first != -1) { 
			// Drawing 3 lines so it can be thicker -- apparently doesn't support line thickness. This is ugly though.
			SDL_RenderDrawLine(renderer, oldP.second * TILE_SIZE + (TILE_SIZE/2), oldP.first * TILE_SIZE + (TILE_SIZE / 2), p.second * TILE_SIZE + (TILE_SIZE / 2), p.first * TILE_SIZE + (TILE_SIZE / 2)); 
			SDL_RenderDrawLine(renderer, oldP.second * TILE_SIZE + (1 + TILE_SIZE / 2), oldP.first * TILE_SIZE + (TILE_SIZE / 2), p.second * TILE_SIZE + (1 + TILE_SIZE / 2), p.first * TILE_SIZE + (TILE_SIZE / 2));
			SDL_RenderDrawLine(renderer, oldP.second * TILE_SIZE + (TILE_SIZE / 2), oldP.first * TILE_SIZE + (-1 + TILE_SIZE / 2), p.second * TILE_SIZE + (TILE_SIZE / 2), p.first * TILE_SIZE + (-1 + TILE_SIZE / 2));
		}
		oldP = p;
	}
}

int SDLGui::handleInput() {
	// event handling
	SDL_Event e;
	if (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT)
			return (SDL_QUIT);
		else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
			return (SDL_QUIT);
		//		else if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
		else if (e.type == SDL_MOUSEBUTTONDOWN) {
			int x, y;
			SDL_GetMouseState(&x, &y);
			// Choose beginning point
			if (beginPoint.first == -1 and beginPoint.second == -1) {
				beginPoint.first = y / TILE_SIZE; beginPoint.second = x / TILE_SIZE;
				snprintf(textMessage, sizeof(textMessage), "Select route end point");
			}
			// Choose end point (and show result)
			else if (endPoint.first == -1 and endPoint.second == -1) {
				endPoint.first = y / TILE_SIZE; endPoint.second = x / TILE_SIZE;
				pathdata = aStarSearch(map, beginPoint, endPoint);
				snprintf(textMessage, sizeof(textMessage), pathdata.message.c_str());
			}
			// Reset and ready for beginning point
			else {
				beginPoint = { -1,-1 }; endPoint = { -1,-1 };
				std::stack<Pair> emptyPath;
				pathdata.path = emptyPath;
				snprintf(textMessage, sizeof(textMessage), "Select route beginning point");
			}
		}
	}
}
