#ifndef FIGHTINGGROUND_H
#define FIGHTINGGROUND_H

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include "charSelection.h"

// This is correct if the background texture is used across multiple files
extern SDL_Texture *backgroundTexture; 

// Enum for different character movements
typedef enum {
    MOVE_STANDING,
    MOVE_FORWARD,
    MOVE_BACKWARD,
    MOVE_KICK,
    MOVE_PUNCH,
    MOVE_DEAD
} CharacterMove;

// Function declarations
void renderFightingGround(SDL_Renderer *renderer,TTF_Font *font);
void handleEvents(SDL_Event *event,GameState *currentState);
void updateCharacterPositions();
void loadCharacterFrames(SDL_Renderer *renderer);
void renderGameOver(SDL_Renderer *renderer,TTF_Font *font);
void resetGameState();


#endif
