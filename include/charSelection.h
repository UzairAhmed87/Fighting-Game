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
     GAME_OVER,
    QUIT
} GameState;

extern int selectedCharacters[2];

void renderCharacterSelection(SDL_Renderer *renderer,TTF_Font *font);
void handleCharacterSelectionEvents(SDL_Event *event, GameState *currentState);
extern void renderText(SDL_Renderer *renderer, const char *text, TTF_Font *font, SDL_Color color, SDL_Rect *rect);
void loadCharacterTextures(SDL_Renderer *renderer);
void cleanupCharacterTextures();

#endif 
