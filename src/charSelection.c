#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "../include/charSelection.h"

// Function prototypes
void loadCharacterTextures(SDL_Renderer *renderer);
void cleanupCharacterTextures();
void renderText(SDL_Renderer *renderer, const char *text, TTF_Font *font, SDL_Color color, SDL_Rect *rect);

// Character textures and rectangles
SDL_Texture *characterTextures[NUM_CHARACTERS];
SDL_Rect characterRects[NUM_CHARACTERS];                                       // Array for character positions
const char *characterNames[NUM_CHARACTERS] = {"Ryu", "Bison", "Guile", "Ken"}; // Character names
int selectedCharacters[2] = {-1, -1};

void renderCharacterSelection(SDL_Renderer *renderer, TTF_Font *font)
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0); // Background color
    SDL_RenderClear(renderer);

    // Render "Select Characters" text
    SDL_Color whiteColor = {255, 255, 255};
    SDL_Rect selectTextRect = {250, 20, 300, 50}; // Position of "Select Characters" text
    renderText(renderer, "Select Characters", font, whiteColor, &selectTextRect);

    // Render character images and names
    for (int i = 0; i < NUM_CHARACTERS; i++)
    {
        SDL_RenderCopy(renderer, characterTextures[i], NULL, &characterRects[i]);

        // Draw character name below the image
        SDL_Rect nameRect = {
            characterRects[i].x,
            characterRects[i].y + characterRects[i].h + 5,
            characterRects[i].w,
            30};
        renderText(renderer, characterNames[i], font, whiteColor, &nameRect);

        // Check if the character is selected and draw a border/overlay
        if (selectedCharacters[0] == i || selectedCharacters[1] == i)
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow color for highlight
            SDL_Rect highlightRect = {
                characterRects[i].x - 5,
                characterRects[i].y - 5,
                characterRects[i].w + 10,
                characterRects[i].h + 10};
            SDL_RenderDrawRect(renderer, &highlightRect);
        }
    }

    // Render "Start Game" button if two characters are selected
    if (selectedCharacters[0] >= 0 && selectedCharacters[1] >= 0)
    {
        SDL_Rect startButton = {300, 500, 200, 50};
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color for start button
        SDL_RenderFillRect(renderer, &startButton);

        // Render the "Start Game" text on the button
        SDL_Color textColor = {0, 0, 0}; // Black color for text
        renderText(renderer, "Start Game", font, textColor, &startButton);
    }

    // Render the "Back to Main" button
    SDL_Rect backButton = {10, 10, 100, 50};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &backButton);

    // Render the "Back to Main" text on the button
    SDL_Color textColorRed = {0, 0, 0}; // Black color for text on white button
    renderText(renderer, "Back", font, textColorRed, &backButton);
}

void renderText(SDL_Renderer *renderer, const char *text, TTF_Font *font, SDL_Color color, SDL_Rect *rect)
{
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    if (!surface)
    {
        printf("Text rendering error: %s\n", TTF_GetError());
        return;
    }
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Center the text inside the rectangle
    int textWidth, textHeight;
    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
    SDL_Rect textRect = {
        rect->x + (rect->w - textWidth) / 2,
        rect->y + (rect->h - textHeight) / 2,
        textWidth,
        textHeight};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);
}

void loadCharacterTextures(SDL_Renderer *renderer)
{
    // Load character textures here
    characterTextures[0] = IMG_LoadTexture(renderer, "../assets/images/ryu-front.png");
    characterTextures[1] = IMG_LoadTexture(renderer, "../assets/images/bison-front.png");
    characterTextures[2] = IMG_LoadTexture(renderer, "../assets/images/guile-front.png");
    characterTextures[3] = IMG_LoadTexture(renderer, "../assets/images/ken-front.png");

    // Set character positions (rectangles)
    characterRects[0] = (SDL_Rect){200, 100, 150, 150}; // Character 1
    characterRects[1] = (SDL_Rect){450, 100, 150, 150}; // Character 2
    characterRects[2] = (SDL_Rect){200, 300, 150, 150}; // Character 3
    characterRects[3] = (SDL_Rect){450, 300, 150, 150}; // Character 4
}

void handleCharacterSelectionEvents(SDL_Event *event, GameState *currentState)
{
    if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        for (int i = 0; i < NUM_CHARACTERS; i++)
        {
            if (event->button.x >= characterRects[i].x && event->button.x <= characterRects[i].x + characterRects[i].w &&
                event->button.y >= characterRects[i].y && event->button.y <= characterRects[i].y + characterRects[i].h)
            {
                // Toggle selection of the character
                if (selectedCharacters[0] == i)
                {
                    selectedCharacters[0] = -1;
                }
                else if (selectedCharacters[1] == i)
                {
                    selectedCharacters[1] = -1;
                }
                else
                {
                    if (selectedCharacters[0] == -1)
                    {
                        selectedCharacters[0] = i;
                    }
                    else if (selectedCharacters[1] == -1)
                    {
                        selectedCharacters[1] = i;
                    }
                }
                break;
            }
        }

        // Check if the "Start Game" button is clicked and two characters are selected
        if (selectedCharacters[0] >= 0 && selectedCharacters[1] >= 0)
        {
            SDL_Rect startButton = {300, 500, 200, 50};
            if (event->button.x >= startButton.x && event->button.x <= startButton.x + startButton.w &&
                event->button.y >= startButton.y && event->button.y <= startButton.y + startButton.h)
            {
                *currentState = FIGHTING_GROUND;
            }
        }

        // Check if the "Back to Main" button is clicked
        SDL_Rect backButton = {10, 10, 100, 50};
        if (event->button.x >= backButton.x && event->button.x <= backButton.x + backButton.w &&
            event->button.y >= backButton.y && event->button.y <= backButton.y + backButton.h)
        {
            *currentState = MAIN_MENU; // Change the state to main menu
        }
    }
}

void cleanupCharacterTextures()
{
    for (int i = 0; i < NUM_CHARACTERS; i++)
    {
        SDL_DestroyTexture(characterTextures[i]);
    }
}
