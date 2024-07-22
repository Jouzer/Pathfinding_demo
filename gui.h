#pragma once

#include <stack>    // for stack
#include <utility>  // for std::pair
#include <SDL.h>
#include <SDL_ttf.h>
#include "astar.h" // For finding out what is maps max size

using Pair = std::pair<int, int>;
#define TILE_SIZE 48
#define WIDTH TILE_SIZE * COL
#define HEIGHT TILE_SIZE * ROW

class SDLGui {
public:
    SDLGui();  // Constructor
    ~SDLGui(); // Destructor
    int initSDL();
    void loadMap(int newmap[ROW][COL]);
    void printMap();
    void render();
    int handleInput();
    void drawPath(std::stack<Pair> path);

private:
    SDL_Window* win;
    SDL_Renderer* renderer;
    SDL_Texture* textures[2];
    int map[ROW][COL];
    Pair beginPoint, endPoint;
    unsigned int lastUpdateTime = 0;
    char textMessage[128];
    TTF_Font* font;
    SDL_Color textColor = { 255, 255, 255, 255 };
    PathData pathdata;
};

