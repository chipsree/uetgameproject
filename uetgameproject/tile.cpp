#include "tile.h"
#include <SDL3_image/SDL_image.h>
#include <random>

Tile grid[GRID_ROWS][GRID_COLS];

void revealTile(int row, int col) {
    if (row < 0 || row >= GRID_ROWS || col < 0 || col >= GRID_COLS)
        return;

    Tile& tile = grid[row][col];
    if (tile.revealed || tile.flagged)
        return;

    tile.revealed = true;

    if (tile.hasMine)
        return;

    int adjacent = countAdjacentMines(row, col);
    if (adjacent == 0) {
		// Reveal adjacent tiles if tile has no adjacent mines
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                if (dr != 0 || dc != 0) {
                    revealTile(row + dr, col + dc);
                }
            }
        }
    }
}

int countAdjacentMines(int row, int col) {
    int count = 0;
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            int r = row + dr, c = col + dc;
            if (r >= 0 && r < GRID_ROWS && c >= 0 && c < GRID_COLS) {
                if (grid[r][c].hasMine) count++;
            }
        }
    }
    return count;
}

void renderTiles(SDL_Renderer* renderer, SDL_Texture* tileTex, SDL_Texture* emptyTex, SDL_Texture* numberTextures[9], SDL_Texture* flagTex, SDL_Texture* mineTex) {
    for (int row = 0; row < GRID_ROWS; ++row) {
        for (int col = 0; col < GRID_COLS; ++col) {
            SDL_FRect dest = {
                static_cast<float>(col * TILE_SIZE),
                static_cast<float>(row * TILE_SIZE),
                static_cast<float>(TILE_SIZE),
                static_cast<float>(TILE_SIZE)
            };

            const Tile& tile = grid[row][col];

            if (!tile.revealed) {
                if (tile.flagged) {
                    SDL_RenderTexture(renderer, flagTex, nullptr, &dest);
                } else {
                    SDL_RenderTexture(renderer, tileTex, nullptr, &dest);
                }
            } else if (tile.hasMine) {
                SDL_RenderTexture(renderer, mineTex, nullptr, &dest);
            } else {
                int adjacent = countAdjacentMines(row, col);
                if (adjacent == 0) {
                    SDL_RenderTexture(renderer, emptyTex, nullptr, &dest);
                } else {
                    SDL_RenderTexture(renderer, numberTextures[adjacent], nullptr, &dest);
                }
            }
        }
    }
}

void placeMines(int numMines) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> rowDist(0, GRID_ROWS - 1);
    std::uniform_int_distribution<> colDist(0, GRID_COLS - 1);

    int placed = 0;
    while (placed < numMines) {
        int r = rowDist(gen);
        int c = colDist(gen);
        if (!grid[r][c].hasMine) {
            grid[r][c].hasMine = true;
            placed++;
        }
    }
}