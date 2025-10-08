#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include "tile.h"
#include <string>
using namespace std;

struct SDLState
{
    SDL_Window* window;
    SDL_Renderer* renderer;
};

void cleanup(SDLState &state)
{
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_Quit();
}

int main(int argc, char *argv[])
{
    SDLState state;

    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing SDL3", nullptr);
        return 1;
    }

    // Create a window
    state.window = SDL_CreateWindow("SDL3 Demo", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!state.window)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating window", nullptr);
        cleanup(state);
        return 1;
    }

    // Create a renderer
    state.renderer = SDL_CreateRenderer(state.window, nullptr);
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
    SDL_Texture* numberTextures[9] = { nullptr };
    for (int i = 0; i < 9; ++i) {
        std::string path = "assets/" + std::to_string(i) + ".png";
        numberTextures[i] = IMG_LoadTexture(state.renderer, path.c_str());
    }

    const int numMines = 60;
    placeMines(numMines);

    // The game loop
    bool running = true;
    while (running)
    {
        SDL_Event event{ 0 };
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                {
                    int mouseX = static_cast<int>(event.button.x);
                    int mouseY = static_cast<int>(event.button.y);
                    int col = mouseX / TILE_SIZE;
                    int row = mouseY / TILE_SIZE;

                    if (event.button.button == SDL_BUTTON_LEFT) {
                        revealTile(row, col);
                    } else if (event.button.button == SDL_BUTTON_RIGHT) {
                        // Toggle flag only if tile is not revealed
                        extern Tile grid[GRID_ROWS][GRID_COLS];
                        if (!grid[row][col].revealed) {
                            grid[row][col].flagged = !grid[row][col].flagged;
                        }
                    }
                    break;
                }
            }
        }
        // Clear the screen
        SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 255);
        SDL_RenderClear(state.renderer);

        // Pass all required textures to renderTiles
        renderTiles(state.renderer, tileTex, emptyTex, numberTextures, flagTex, mineTex);

        // Present the backbuffer
        SDL_RenderPresent(state.renderer);
    }

    cleanup(state);
    return 0;
}

