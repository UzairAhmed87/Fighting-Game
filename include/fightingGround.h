#ifndef FIGHTINGGROUND_H
#define FIGHTINGGROUND_H

#include <SDL2/SDL.h>

// This is correct if the background texture is used across multiple files
extern SDL_Texture *backgroundTexture; 

// Enum for different character movements
typedef enum {
    MOVE_STANDING,
    MOVE_FORWARD,
    MOVE_BACKWARD,
    MOVE_KICK,
    MOVE_PUNCH
} CharacterMove;

// Function declarations
void renderFightingGround(SDL_Renderer *renderer);
void handleEvents(SDL_Event *event);
void updateCharacterPositions();
void loadCharacterFrames(SDL_Renderer *renderer);
void cleanupFightingGround();

#endif
