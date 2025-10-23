#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

class Timer {
private:
    Uint64 startTicks;
    bool started;
    bool paused;
    Uint64 pausedTicks;

public:
    Timer();
    void start();
    void stop();
    void pause();
    void unpause();
    int getSeconds() const;
    bool isStarted() const;
    bool isPaused() const;
    void render(SDL_Renderer* renderer, TTF_Font* font, int windowWidth) const;
};