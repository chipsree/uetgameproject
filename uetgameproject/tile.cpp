#include "tile.h"

Tile grid[GRID_ROWS][GRID_COLS];
// Definition of gameOver
bool gameOver = false;

// Helper to reset the board
void resetBoard(int numMines) {
    for (int r = 0; r < GRID_ROWS; ++r) {
        for (int c = 0; c < GRID_COLS; ++c) {
            grid[r][c].hasMine = false;
            grid[r][c].revealed = false;
            grid[r][c].flagged = false;
        }
    }
    placeMines(numMines);
    gameOver = false;
}

void revealTile(int row, int col) {
    if (gameOver) return;
    if (row < 0 || row >= GRID_ROWS || col < 0 || col >= GRID_COLS)
        return;

    Tile& tile = grid[row][col];
    if (tile.revealed || tile.flagged)
        return;

    tile.revealed = true;

    if (tile.hasMine) {
        for (int r = 0; r < GRID_ROWS; ++r) {
            for (int c = 0; c < GRID_COLS; ++c) {
                if (grid[r][c].hasMine) {
                    grid[r][c].revealed = true;
                }
            }
        }
        gameOver = true;
        return;
    }

    int adjacent = countAdjacentMines(row, col);
    if (adjacent == 0) {
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if (i != 0 || j != 0) {
                    revealTile(row + i, col + j);
                }
            }
        }
    }
}

int countAdjacentMines(int row, int col) {
    int count = 0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            int r = row + i, c = col + j;
            if (r >= 0 && r < GRID_ROWS && c >= 0 && c < GRID_COLS) {
                if (grid[r][c].hasMine) count++;
            }
        }
    }
    return count;
}

void renderGameOverScreen(SDL_Renderer* renderer) {
    // Draw semi-transparent overlay
    SDL_FRect overlay = {0, 0, static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT)};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180); // Black, semi-transparent
    SDL_RenderFillRect(renderer, &overlay);

    // Draw "Game Over" text background (rectangle placeholder)
    SDL_FRect textRect = {WINDOW_WIDTH / 2.0f - 100, WINDOW_HEIGHT / 2.0f - 60, 200, 40};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
    SDL_RenderFillRect(renderer, &textRect);

    // Draw reset button background (rectangle placeholder)
    SDL_FRect buttonRect = {WINDOW_WIDTH / 2.0f - 60, WINDOW_HEIGHT / 2.0f + 10, 120, 40};
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Light gray
    SDL_RenderFillRect(renderer, &buttonRect);

    // For actual text, use SDL_ttf (not shown here)
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

    if (gameOver) {
        renderGameOverScreen(renderer);
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

// Call this in your main event loop to handle reset button click
bool handleGameOverEvent(const SDL_Event& event, int numMines) {
    if (!gameOver) return false;
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        int mx = event.button.x;
        int my = event.button.y;
        SDL_FRect buttonRect = {WINDOW_WIDTH / 2.0f - 60, WINDOW_HEIGHT / 2.0f + 10, 120, 40};
        if (mx >= buttonRect.x && mx <= buttonRect.x + buttonRect.w &&
            my >= buttonRect.y && my <= buttonRect.y + buttonRect.h) {
            resetBoard(numMines);
            return true; // Reset occurred
        }
    }
    return false;
}