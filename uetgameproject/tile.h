#pragma once
#include <SDL3/SDL.h>

constexpr int TILE_SIZE = 32;
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr int GRID_COLS = WINDOW_WIDTH / TILE_SIZE;
constexpr int GRID_ROWS = WINDOW_HEIGHT / TILE_SIZE;

struct Tile {
    bool hasMine = false;
    bool revealed = false;
    bool flagged = false;
};

void revealTile(int row, int col);
int countAdjacentMines(int row, int col);
void placeMines(int numMines);
void renderTiles(SDL_Renderer* renderer, SDL_Texture* tileTex, SDL_Texture* emptyTex, SDL_Texture* numberTextures[9], SDL_Texture* flagTex, SDL_Texture* mineTex);