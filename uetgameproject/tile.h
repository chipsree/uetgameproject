#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <random>
#include <string>

constexpr int TILE_SIZE = 32;
constexpr int WINDOW_WIDTH = 960;
constexpr int WINDOW_HEIGHT = 800;
constexpr int GRID_COLS = WINDOW_WIDTH / TILE_SIZE;
constexpr int GRID_ROWS = WINDOW_HEIGHT / TILE_SIZE;

// Grid origin (top-left corner of grid in window coordinates)
constexpr int GRID_ORIGIN_X = 0;
constexpr int GRID_ORIGIN_Y = 0;

struct Tile {
    bool hasMine = false;
    bool revealed = false;
    bool flagged = false;
};

// Make gameOver accessible from other files
extern bool gameOver;

void revealTile(int row, int col);
int countAdjacentMines(int row, int col);
void placeMines(int numMines);
void renderTiles(SDL_Renderer* renderer, SDL_Texture* tileTex, SDL_Texture* emptyTex, SDL_Texture* numberTextures[9], SDL_Texture* flagTex, SDL_Texture* mineTex);
bool handleGameOverEvent(const SDL_Event& event, int numMines);