#ifndef CHARACTER_SELECTION_H
#define CHARACTER_SELECTION_H

#include <SDL2/SDL.h>
#include <SDL_ttf.h>


#define NUM_CHARACTERS 4

// Enum for game states
typedef enum {
    MAIN_MENU,
    CHARACTER_SELECTION,
    FIGHTING_GROUND,
    QUIT
} GameState;

extern int selectedCharacters[2];

void renderCharacterSelection(SDL_Renderer *renderer,TTF_Font *font);
void handleCharacterSelectionEvents(SDL_Event *event, GameState *currentState);
void loadCharacterTextures(SDL_Renderer *renderer);
void cleanupCharacterTextures();

#endif 
