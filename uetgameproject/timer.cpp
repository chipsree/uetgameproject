// timer.cpp
#include "timer.h"
#include <string>

void Timer::render(SDL_Renderer* renderer, TTF_Font* font, int windowWidth) const {
    int seconds = getSeconds();
    std::string timerText = std::to_string(seconds) + "s";
    SDL_Color timerColor = { 255, 255, 255, 255 }; // White text

    SDL_Surface* timerSurface = TTF_RenderText_Blended(font, timerText.c_str(), timerText.length(), timerColor);
    if (timerSurface) {
        SDL_Texture* timerTexture = SDL_CreateTextureFromSurface(renderer, timerSurface);
        int textW = timerSurface->w;
        int textH = timerSurface->h;
        SDL_DestroySurface(timerSurface);

        // Center in top bar (96 pixels tall)
        SDL_FRect timerRect = {
            windowWidth / 2.0f - textW / 2.0f,
            48.0f - textH / 2.0f,  // Center vertically in 96px bar
            static_cast<float>(textW),
            static_cast<float>(textH)
        };
        SDL_RenderTexture(renderer, timerTexture, nullptr, &timerRect);
        SDL_DestroyTexture(timerTexture);
    }
}

Timer::Timer() : startTicks(0), started(false), paused(false), pausedTicks(0) {}

void Timer::start() {
    started = true;
    paused = false;
    startTicks = SDL_GetTicks();
    pausedTicks = 0;
}

void Timer::stop() {
    started = false;
    paused = false;
    startTicks = 0;
    pausedTicks = 0;
}

void Timer::pause() {
    if (started && !paused) {
        paused = true;
        pausedTicks = SDL_GetTicks() - startTicks;
        startTicks = 0;
    }
}

void Timer::unpause() {
    if (started && paused) {
        paused = false;
        startTicks = SDL_GetTicks() - pausedTicks;
        pausedTicks = 0;
    }
}

int Timer::getSeconds() const {
    if (!started) return 0;

    Uint64 time = 0;
    if (paused) {
        time = pausedTicks;
    }
    else {
        time = SDL_GetTicks() - startTicks;
    }

    return static_cast<int>(time / 1000);
}

bool Timer::isStarted() const {
    return started;
}

bool Timer::isPaused() const {
    return paused && started;
}