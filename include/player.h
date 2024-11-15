#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>

typedef struct {
    SDL_Texture *texture;
    SDL_Rect position;
} Player;

Player *createPlayer(SDL_Renderer *renderer, const char *filePath, int x, int y);
void renderPlayer(SDL_Renderer *renderer, Player *player);
void destroyPlayer(Player *player);

// Add the updatePlayerPosition function declaration
void updatePlayerPosition(Player *player, int dx, int dy, int windowWidth, int windowHeight);

#endif
