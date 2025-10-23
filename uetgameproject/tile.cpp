#include "tile.h"

Tile grid[GRID_ROWS][GRID_COLS];
// Definition of gameOver
bool gameOver = false;
bool gameWin = false;

// Global font pointer defined in main and used for rendering overlays
TTF_Font* gFont = nullptr;

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
    gameWin = false; // Reset win state
}

bool checkWin() {
    for (int r = 0; r < GRID_ROWS; ++r) {
        for (int c = 0; c < GRID_COLS; ++c) {
            if (!grid[r][c].hasMine && !grid[r][c].revealed) {
                return false;
            }
        }
    }
    return true;
}

void revealTile(int row, int col) {
    if (gameOver || gameWin) return;
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

    if (checkWin()) {
        gameWin = true;
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
    std::string gameover = "Game Over!";
    std::string resetText = "Reset";
    SDL_Color textColor = { 255, 255, 255, 255 }; // White
    SDL_Color buttonTextColor = { 0, 0, 0, 255 }; // Black for button

    TTF_Font* largeFont = TTF_OpenFont("assets/ARIAL.TTF", 96);  // Larger for "Game Over"
    TTF_Font* smallFont = TTF_OpenFont("assets/ARIAL.TTF", 32);  // Smaller for button

    SDL_Surface* textSurface = TTF_RenderText_Blended(largeFont, gameover.c_str(), gameover.length(), textColor);
    SDL_Surface* buttonTextSurface = TTF_RenderText_Blended(smallFont, resetText.c_str(), resetText.length(), buttonTextColor);

    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        int textW = textSurface->w;
        int textH = textSurface->h;
        SDL_DestroySurface(textSurface);

        // Draw semi-transparent overlay
        SDL_FRect overlay = { 0, 0, static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT) };
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, 240); // Gray, semi-transparent
        SDL_RenderFillRect(renderer, &overlay);

        // Draw the lose text
        SDL_FRect textDestRect = {
            WINDOW_WIDTH / 2.0f - textW / 2.0f,
            WINDOW_HEIGHT / 2.0f - textH / 2.0f - 100,
            static_cast<float>(textW),
            static_cast<float>(textH)
        };
        SDL_RenderTexture(renderer, textTexture, nullptr, &textDestRect);

        SDL_DestroyTexture(textTexture);
    }

    // Draw reset button background
    SDL_FRect buttonRect = { WINDOW_WIDTH / 2.0f - 60, WINDOW_HEIGHT / 2.0f + 10, 120, 40 };
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &buttonRect);

    if (buttonTextSurface) {
        SDL_Texture* buttonTexture = SDL_CreateTextureFromSurface(renderer, buttonTextSurface);
        int buttonTextW = buttonTextSurface->w;
        int buttonTextH = buttonTextSurface->h;
        SDL_DestroySurface(buttonTextSurface);

        // Draw the button text
        SDL_FRect buttonTextRect = {
            WINDOW_WIDTH / 2.0f - buttonTextW / 2.0f,
            WINDOW_HEIGHT / 2.0f + 10 + 20 - buttonTextH / 2.0f,
            static_cast<float>(buttonTextW),
            static_cast<float>(buttonTextH)
        };
        SDL_RenderTexture(renderer, buttonTexture, nullptr, &buttonTextRect);

        SDL_DestroyTexture(buttonTexture);
    }

    TTF_CloseFont(largeFont);
    TTF_CloseFont(smallFont);
}

void renderGameWinScreen(SDL_Renderer* renderer) {
    std::string winText = "You Win!";
    std::string resetText = "Reset";
    SDL_Color textColor = { 255, 255, 255, 255 }; // White
    SDL_Color buttonTextColor = { 0, 0, 0, 255 };

    TTF_Font* largeFont = TTF_OpenFont("assets/ARIAL.TTF", 96);  // Larger for "You Win!"
    TTF_Font* smallFont = TTF_OpenFont("assets/ARIAL.TTF", 32);  // Smaller for button

    SDL_Surface* textSurface = TTF_RenderText_Blended(largeFont, winText.c_str(), winText.length(), textColor);
    SDL_Surface* buttonTextSurface = TTF_RenderText_Blended(smallFont, resetText.c_str(), resetText.length(), buttonTextColor);

    // Draw semi-transparent overlay
    SDL_FRect overlay = { 0, 0, static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT) };
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 240); // Gray, semi-transparent
    SDL_RenderFillRect(renderer, &overlay);

    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        int textW = textSurface->w;
        int textH = textSurface->h;
        SDL_DestroySurface(textSurface);

        // Draw the win text
        SDL_FRect textDestRect = {
            WINDOW_WIDTH / 2.0f - textW / 2.0f,
            WINDOW_HEIGHT / 2.0f - textH / 2.0f - 100,
            static_cast<float>(textW),
            static_cast<float>(textH)
        };
        SDL_RenderTexture(renderer, textTexture, nullptr, &textDestRect);

        SDL_DestroyTexture(textTexture);
    }

    // Draw reset button background
    SDL_FRect buttonRect = { WINDOW_WIDTH / 2.0f - 60, WINDOW_HEIGHT / 2.0f + 10, 120, 40 };
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &buttonRect);

    if (buttonTextSurface) {
        SDL_Texture* buttonTexture = SDL_CreateTextureFromSurface(renderer, buttonTextSurface);
        int buttonTextW = buttonTextSurface->w;
        int buttonTextH = buttonTextSurface->h;
        SDL_DestroySurface(buttonTextSurface);

        // Draw the reset button text
        SDL_FRect buttonTextRect = {
            WINDOW_WIDTH / 2.0f - buttonTextW / 2.0f,
            WINDOW_HEIGHT / 2.0f + 10 + 20 - buttonTextH / 2.0f,
            static_cast<float>(buttonTextW),
            static_cast<float>(buttonTextH)
        };
        SDL_RenderTexture(renderer, buttonTexture, nullptr, &buttonTextRect);

        SDL_DestroyTexture(buttonTexture);
    }

    TTF_CloseFont(largeFont);
    TTF_CloseFont(smallFont);
}

void renderTiles(SDL_Renderer* renderer, SDL_Texture* tileTex, SDL_Texture* emptyTex, SDL_Texture* numberTextures[9], SDL_Texture* flagTex, SDL_Texture* mineTex) {
    for (int row = 0; row < GRID_ROWS; ++row) {
        for (int col = 0; col < GRID_COLS; ++col) {
            SDL_FRect dest = {
                static_cast<float>(GRID_ORIGIN_X + col * TILE_SIZE),
                static_cast<float>(GRID_ORIGIN_Y + row * TILE_SIZE),
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
    } else if (gameWin) {
        renderGameWinScreen(renderer);
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

bool handleGameOverEvent(const SDL_Event& event, int numMines) {
    if (!gameOver) return false;
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        int mx = event.button.x;
        int my = event.button.y;
        SDL_FRect buttonRect = {WINDOW_WIDTH / 2.0f - 60, WINDOW_HEIGHT / 2.0f + 10, 120, 40};
        if (mx >= buttonRect.x && mx <= buttonRect.x + buttonRect.w &&
            my >= buttonRect.y && my <= buttonRect.y + buttonRect.h) {
            resetBoard(numMines);
            return true;
        }
    }
    return false;
}

bool handleGameWinEvent(const SDL_Event& event, int numMines) {
    if (!gameWin) return false;
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        int mx = event.button.x;
        int my = event.button.y;
        SDL_FRect buttonRect = {WINDOW_WIDTH / 2.0f - 60, WINDOW_HEIGHT / 2.0f + 10, 120, 40};
        if (mx >= buttonRect.x && mx <= buttonRect.x + buttonRect.w &&
            my >= buttonRect.y && my <= buttonRect.y + buttonRect.h) {
            resetBoard(numMines);
            return true;
        }
    }
    return false;
}