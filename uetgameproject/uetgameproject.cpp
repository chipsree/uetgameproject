#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include "tile.h"
#include "timer.h"
#include <string>
using namespace std;

// Structure to hold SDL state
struct SDLState
{
    SDL_Window* window;
    SDL_Renderer* renderer;
};

// Cleanup function to free resources
void cleanup(SDLState& state)
{
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_Quit();
    TTF_Quit();
}

int main(int argc, char* argv[])
{
    SDLState state;

	// Initialize SDL3
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing SDL3", nullptr);
        return 1;
    }

	// Initialize SDL_ttf
    if (!TTF_Init()) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing SDL_ttf", nullptr);
        SDL_Quit();
        return 1;
    }

	// Create window
    state.window = SDL_CreateWindow("Minesweeper", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!state.window)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating window", nullptr);
        cleanup(state);
        return 1;
    }

	// Create renderer
    state.renderer = SDL_CreateRenderer(state.window, nullptr);
    SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_BLEND);
    if (!state.renderer)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating renderer", state.window);
        cleanup(state);
        return 1;
    }

    // Asset loading
    SDL_Texture* tileTex = IMG_LoadTexture(state.renderer, "assets/tile.png");
    SDL_Texture* emptyTex = IMG_LoadTexture(state.renderer, "assets/empty_tile.png");
    SDL_Texture* flagTex = IMG_LoadTexture(state.renderer, "assets/flag.png");
    SDL_Texture* mineTex = IMG_LoadTexture(state.renderer, "assets/mine.png");
    SDL_Texture* numberTextures[9];
    for (int i = 1; i < 8; ++i) {
        std::string path = "assets/" + std::to_string(i) + ".png";
        numberTextures[i] = IMG_LoadTexture(state.renderer, path.c_str());
    }

    TTF_Font* font = TTF_OpenFont("assets/ARIAL.TTF", 32);

    // The game loop
    extern GameState currentGameState;
	currentGameState = GameState::MINE_MENU; // Start at mine selection menu
    int numMines = 100;
    placeMines(numMines);  // Remove this line or move it after first menu selection

    Timer gameTimer;
    // Don't start timer yet - gameTimer.start();

    bool running = true;
    while (running)
    {
        SDL_Event event{ 0 };
        while (SDL_PollEvent(&event))
        {
            // Handle mine menu selection FIRST, before checking game state
            if (currentGameState == GameState::MINE_MENU) {
                int selectedMines = handleMineMenuEvent(event);
                if (selectedMines != -1) {
                    numMines = selectedMines;
                    gameTimer.stop();
                    gameTimer.start();
                }
            }
            else if (currentGameState == GameState::GAME_OVER) {
                gameTimer.pause();
                handleGameOverEvent(event, numMines);
            }
            else if (currentGameState == GameState::GAME_WIN) {
                gameTimer.pause();
                handleGameWinEvent(event, numMines);
            }
            else if (currentGameState == GameState::PLAYING) {
                // mouse clicks during gameplay
                int mouseX = static_cast<int>(event.button.x);
                int mouseY = static_cast<int>(event.button.y);
                int col = (mouseX - GRID_ORIGIN_X) / TILE_SIZE;
                int row = (mouseY - GRID_ORIGIN_Y) / TILE_SIZE;

                if (col >= 0 && col < GRID_COLS && row >= 0 && row < GRID_ROWS) {
                    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                        if (event.button.button == SDL_BUTTON_LEFT) {
                            revealTile(row, col);
                        }
                        else if (event.button.button == SDL_BUTTON_RIGHT) {
                            extern Tile grid[GRID_ROWS][GRID_COLS];
                            if (!grid[row][col].revealed) {
                                grid[row][col].flagged = !grid[row][col].flagged;
                            }
                        }
                    }
                }
            }

            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            }
        }

        // Clear the screen
        SDL_SetRenderDrawColor(state.renderer, 195, 195, 195, 255);
        SDL_RenderClear(state.renderer);

        // Render game elements
        renderTiles(state.renderer, tileTex, emptyTex, numberTextures, flagTex, mineTex);

        // Only render timer during gameplay
        if (currentGameState == GameState::PLAYING) {
            gameTimer.render(state.renderer, font, WINDOW_WIDTH);
        }

		// Present the rendered frame
        SDL_RenderPresent(state.renderer);
    }

    cleanup(state);
    return 0;
}