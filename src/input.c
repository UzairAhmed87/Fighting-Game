// src/input.c
#include "../include/input.h"
#include <SDL2/SDL.h>

void handlePlayerInput(SDL_Event *event, Player *player) {
    const int SPEED = 5;
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_UP: player->position.y -= SPEED; break;
            case SDLK_DOWN: player->position.y += SPEED; break;
            case SDLK_LEFT: player->position.x -= SPEED; break;
            case SDLK_RIGHT: player->position.x += SPEED; break;
        }
    }
}
