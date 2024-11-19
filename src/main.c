#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include "../include/charSelection.h"
#include "../include/fightingGround.h"

// Function prototypes
void renderMainMenu(SDL_Renderer *renderer, TTF_Font *font);
void handleMainMenuEvents(SDL_Event *event, GameState *currentState);
void cleanup(SDL_Renderer *renderer, SDL_Window *window, TTF_Font *font);
void renderbgImage(SDL_Renderer *renderer);
void cleanupbgImage();
void loadBackgroundMusic();
void playBackgroundMusic();
void toggleSound();

TTF_Font *font = NULL;
SDL_Texture *backgroundImage = NULL;
Mix_Music *bgMusic = NULL;
int soundOn = 1;

int main(int argc, char *argv[])
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() < 0)
    {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Create window and renderer
    SDL_Window *window = SDL_CreateWindow("Arena Fighters", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);

    if (!window || !renderer)
    {
        printf("Window or Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        cleanup(renderer, window, font);
        return 1;
    }

    // Load the font
    font = TTF_OpenFont("ARIBLK.ttf", 24);
    if (!font)
    {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        cleanup(renderer, window, font);
        return 1;
    }

    GameState currentState = MAIN_MENU;
    SDL_Event event;

    loadCharacterTextures(renderer);
    loadCharacterFrames(renderer);
    loadBackgroundMusic();
    playBackgroundMusic();
    SDL_RenderClear(renderer);
    renderFightingGround(renderer, font);

    // Main loop

    while (currentState != QUIT)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                currentState = QUIT;
            }

            if (currentState == MAIN_MENU)
            {
                handleMainMenuEvents(&event, &currentState);
            }
            else if (currentState == CHARACTER_SELECTION)
            {
                handleCharacterSelectionEvents(&event, &currentState);
            }
            else if (currentState == FIGHTING_GROUND)
            {
                handleEvents(&event, &currentState);
            }
            updateCharacterPositions();
            
        }

        // Render based on current state
        SDL_RenderClear(renderer);
        if (currentState == MAIN_MENU)
        {
            renderbgImage(renderer);
            renderMainMenu(renderer, font);
        }
        else if (currentState == CHARACTER_SELECTION)
        {
            renderCharacterSelection(renderer, font);
        }
        else if (currentState == FIGHTING_GROUND)
        {
            renderFightingGround(renderer, font);
        }
        else if (currentState == GAME_OVER)
        {

            renderGameOver(renderer, font);
        }
        SDL_RenderPresent(renderer);
    }

    cleanup(renderer, window, font);
    return 0;
}

void renderbgImage(SDL_Renderer *renderer)
{
    if (!backgroundImage)
    {
        backgroundImage = IMG_LoadTexture(renderer, "../assets/images/bg.png");
        if (!backgroundImage)
        {
            printf("Failed to load background texture! SDL_image Error: %s\n", IMG_GetError());
            return;
        }
        else
        {
            printf("Background image loaded successfully.\n"); // Debug line
        }
    }
    SDL_RenderCopy(renderer, backgroundImage, NULL, NULL);
}

void cleanupbgImage()
{
    if (backgroundImage)
    {
        SDL_DestroyTexture(backgroundImage);
        backgroundImage = NULL;
    }
}

void loadBackgroundMusic()
{
    bgMusic = Mix_LoadMUS("../assets/music/bgmusic.mp3");
    if (!bgMusic)
    {
        printf("Failed to load background music! SDL_mixer Error: %s\n", Mix_GetError());
    }
}

void playBackgroundMusic()
{
    if (bgMusic && soundOn)
    {
        Mix_PlayMusic(bgMusic, -1); // -1 means loop indefinitely
    }
}

void toggleSound()
{
    soundOn = !soundOn;
    if (soundOn)
    {
        playBackgroundMusic(); // Play music if sound is on
    }
    else
    {
        Mix_HaltMusic(); // Stop music if sound is off
    }
}

void renderMainMenu(SDL_Renderer *renderer, TTF_Font *font)
{
    // Draw a simple button for new game
    SDL_Rect button = {300, 200, 200, 50};
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color for button
    SDL_RenderFillRect(renderer, &button);

    SDL_Rect button2 = {300, 300, 200, 50};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for button
    SDL_RenderFillRect(renderer, &button2);

    SDL_Rect button3 = {300, 400, 200, 50};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0); // Red color for button
    SDL_RenderFillRect(renderer, &button3);

    SDL_Color textColor = {0, 0, 0, 255}; // black color for text

    SDL_Rect bg = {50, 100, 700, 50};                   // dimensions
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // yellow color for button
    SDL_RenderFillRect(renderer, &bg);

    SDL_Surface *textSurface = TTF_RenderText_Blended(font, "Welcome to the Blaze of Glory: Arena Fighters!", textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    if (textTexture == NULL)
    {
        printf("Failed to create text texture! SDL_Error: %s\n", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return;
    }

    SDL_Rect bgTextRect;
    bgTextRect.x = bg.x + (bg.w - textSurface->w) / 2;
    bgTextRect.y = bg.y + (bg.h - textSurface->h) / 2;
    bgTextRect.w = textSurface->w;
    bgTextRect.h = textSurface->h;

    // Render the text texture on top of the button
    SDL_RenderCopy(renderer, textTexture, NULL, &bgTextRect);

    // Free resources for the first text
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    // Render "Start Game" text on the button
    textSurface = TTF_RenderText_Blended(font, "New Game", textColor);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    if (textTexture == NULL)
    {
        printf("Failed to create text texture! SDL_Error: %s\n", SDL_GetError());
        SDL_FreeSurface(textSurface); // Free the surface to avoid memory leaks
        return;
    }

    // Center the text on the button
    SDL_Rect buttonTextRect;
    buttonTextRect.x = button.x + (button.w - textSurface->w) / 2;
    buttonTextRect.y = button.y + (button.h - textSurface->h) / 2;
    buttonTextRect.w = textSurface->w;
    buttonTextRect.h = textSurface->h;

    // Render the text texture on top of the button
    SDL_RenderCopy(renderer, textTexture, NULL, &buttonTextRect);

    textSurface = TTF_RenderText_Blended(font, "Sound On/Off", textColor);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    if (textTexture == NULL)
    {
        printf("Failed to create text texture! SDL_Error: %s\n", SDL_GetError());
        SDL_FreeSurface(textSurface); // Free the surface to avoid memory leaks
        return;
    }

    // Center the text on the second button
    SDL_Rect button2TextRect;
    button2TextRect.x = button2.x + (button2.w - textSurface->w) / 2;
    button2TextRect.y = button2.y + (button2.h - textSurface->h) / 2;
    button2TextRect.w = textSurface->w;
    button2TextRect.h = textSurface->h;

    // Render the text texture on top of the second button
    SDL_RenderCopy(renderer, textTexture, NULL, &button2TextRect);

    // Free resources for the second text
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    // Render "Quit" text on the third button
    textSurface = TTF_RenderText_Blended(font, "Quit", textColor);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    if (textTexture == NULL)
    {
        printf("Failed to create text texture! SDL_Error: %s\n", SDL_GetError());
        SDL_FreeSurface(textSurface); // Free the surface to avoid memory leaks
        return;
    }

    // Center the text on the third button
    SDL_Rect button3TextRect;
    button3TextRect.x = button3.x + (button3.w - textSurface->w) / 2;
    button3TextRect.y = button3.y + (button3.h - textSurface->h) / 2;
    button3TextRect.w = textSurface->w;
    button3TextRect.h = textSurface->h;

    // Render the text texture on top of the third button
    SDL_RenderCopy(renderer, textTexture, NULL, &button3TextRect);

    // Free resources for the third text
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void handleMainMenuEvents(SDL_Event *event, GameState *currentState)
{
    if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        int mouseX = event->button.x;
        int mouseY = event->button.y;

        // Check if the "New Game" button is clicked
        if (mouseX >= 300 && mouseX <= 500 && mouseY >= 200 && mouseY <= 250)
        {
            *currentState = CHARACTER_SELECTION; // Go to character selection
        }

        // Check if the "Sound On/Off" button is clicked
        if (mouseX >= 300 && mouseX <= 500 && mouseY >= 300 && mouseY <= 350)
        {
            toggleSound(); // Toggle sound state
        }

        // Check if the "Quit" button is clicked
        if (mouseX >= 300 && mouseX <= 500 && mouseY >= 400 && mouseY <= 450)
        {
            *currentState = QUIT; // Quit the game
        }
    }
}

void cleanup(SDL_Renderer *renderer, SDL_Window *window, TTF_Font *font)
{
    cleanupbgImage(); // Clean up background image
    if (bgMusic)
    {
        Mix_FreeMusic(bgMusic); // Free the music resource
    }
    if (font)
    {
        TTF_CloseFont(font);
    }
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
    }
    if (window)
    {
        SDL_DestroyWindow(window);
    }
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}