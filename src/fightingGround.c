#include "../include/fightingGround.h"
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include "../include/charSelection.h"

// Define constants
#define FRAME_DELAY 70  
#define MAX_FRAMES 100  // Maximum frames per movement (adjust as needed)
#define NUM_MOVES 5     
#define MOVE_SPEED 10   
#define CHARACTER_SPEED 5 
#define MAX_HEALTH 100  // Maximum health for characters

// Global variables
SDL_Texture *backgroundTexture = NULL;
SDL_Texture *characterFrames[NUM_CHARACTERS][NUM_MOVES][MAX_FRAMES] = {NULL};
int characterFrameCounts[NUM_CHARACTERS][NUM_MOVES] = {0};
CharacterMove characterStates[NUM_CHARACTERS] = {MOVE_STANDING, MOVE_STANDING};
CharacterMove previousCharacterStates[NUM_CHARACTERS] = {MOVE_STANDING, MOVE_STANDING};
int currentFrame[NUM_CHARACTERS] = {0, 0};
Uint64 lastFrameTime[NUM_CHARACTERS] = {0, 0};

// Health values for both characters
int characterHealth[2] = {MAX_HEALTH, MAX_HEALTH};

// Positions for characters in the fighting ground
SDL_Rect characterPositions[2] = {
    {100, 450, 80, 100}, // Position for the first character
    {550, 450, 80, 110}  // Position for the second character
};

// Function to load character frames
void loadCharacterFrames(SDL_Renderer *renderer) {
    char framePath[150];
    const char *moveNames[NUM_MOVES] = {"stand", "forward", "backward", "kick", "punch"};

    for (int i = 0; i < NUM_CHARACTERS; i++) {
        for (int m = 0; m < NUM_MOVES; m++) {
            sprintf(framePath, "../assets/images/character%d/%s", i, moveNames[m]);
            int frameIndex = 0;
            SDL_Texture *frameTexture;

            while (frameIndex < MAX_FRAMES) {
                sprintf(framePath, "../assets/images/character%d/%s/frame (%d).png", i, moveNames[m], frameIndex + 1);
                frameTexture = IMG_LoadTexture(renderer, framePath);
                if (!frameTexture) {
                    if (frameIndex == 0) {
                        printf("No frames found for character %d, move %s! SDL_image Error: %s\n", i, moveNames[m], IMG_GetError());
                    }
                    break;
                }
                characterFrames[i][m][frameIndex] = frameTexture;
                frameIndex++;
            }
            characterFrameCounts[i][m] = frameIndex;
        }
    }
}

// Function to render health bars
void renderHealthBars(SDL_Renderer *renderer) {
    SDL_Color healthyColor = {0, 255, 0, 255};  // Green
    SDL_Color lowHealthColor = {255, 0, 0, 255}; // Red

    for (int i = 0; i < 2; i++) {
        int healthBarWidth = (characterHealth[i] * 200) / MAX_HEALTH; // Max width is 200 pixels
        SDL_Rect healthBarRect;

        if (i == 0) {
            healthBarRect = (SDL_Rect){20, 20, healthBarWidth, 20}; // Character 1's health bar on the left
        } else {
            healthBarRect = (SDL_Rect){580, 20, healthBarWidth, 20}; // Character 2's health bar on the right
        }

        if (characterHealth[i] > 30) {
            SDL_SetRenderDrawColor(renderer, healthyColor.r, healthyColor.g, healthyColor.b, healthyColor.a);
        } else {
            SDL_SetRenderDrawColor(renderer, lowHealthColor.r, lowHealthColor.g, lowHealthColor.b, lowHealthColor.a);
        }
        SDL_RenderFillRect(renderer, &healthBarRect);
    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

// Function to check collision between two SDL_Rects
bool checkCollision(SDL_Rect a, SDL_Rect b) {
    return (a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y);
}

// Function to handle character attacks and health updates
void handleCharacterAttacks() {
    SDL_Rect attackRect[2];
    bool collisionDetected[2] = {false, false};

    for (int i = 0; i < 2; i++) {
        if (characterStates[i] == MOVE_PUNCH || characterStates[i] == MOVE_KICK) {
            attackRect[i] = characterPositions[i];
            if (characterStates[i] == MOVE_PUNCH) {
                attackRect[i].w += 30; // Extend punch range
            } else if (characterStates[i] == MOVE_KICK) {
                attackRect[i].w += 40; // Extend kick range
            }
        } else {
            attackRect[i] = (SDL_Rect){0, 0, 0, 0}; // No attack
        }
    }

    // Check for collisions and update health
    if (checkCollision(attackRect[0], characterPositions[1])) {
        collisionDetected[0] = true; // Character 1 hits Character 2
        if (characterStates[0] == MOVE_PUNCH) {
            characterHealth[1] -= 5; // Punch: 5% health decrease
        } else if (characterStates[0] == MOVE_KICK) {
            characterHealth[1] -= 10; // Kick: 10% health decrease
        }
        if (characterHealth[1] < 0) characterHealth[1] = 0;
    }
    if (checkCollision(attackRect[1], characterPositions[0])) {
        collisionDetected[1] = true; // Character 2 hits Character 1
        if (characterStates[1] == MOVE_PUNCH) {
            characterHealth[0] -= 5; // Punch: 5% health decrease
        } else if (characterStates[1] == MOVE_KICK) {
            characterHealth[0] -= 10; // Kick: 10% health decrease
        }
        if (characterHealth[0] < 0) characterHealth[0] = 0;
    }
}

// Function to handle events and update character states
void handleEvents(SDL_Event *event) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_a:
                characterStates[0] = MOVE_BACKWARD;
                break;
            case SDLK_d:
                characterStates[0] = MOVE_FORWARD;
                break;
            case SDLK_s:
                characterStates[0] = MOVE_PUNCH;
                break;
            case SDLK_x:
                characterStates[0] = MOVE_KICK;
                break;
            case SDLK_j:
                characterStates[1] = MOVE_BACKWARD;
                break;
            case SDLK_l:
                characterStates[1] = MOVE_FORWARD;
                break;
            case SDLK_k:
                characterStates[1] = MOVE_PUNCH;
                break;
            case SDLK_m:
                characterStates[1] = MOVE_KICK;
                break;
        }
    }

    if (event->type == SDL_KEYUP) {
        switch (event->key.keysym.sym) {
            case SDLK_a:
            case SDLK_d:
            case SDLK_s:
            case SDLK_x:
                characterStates[0] = MOVE_STANDING;
                break;
            case SDLK_j:
            case SDLK_l:
            case SDLK_k:
            case SDLK_m:
                characterStates[1] = MOVE_STANDING;
                break;
        }
    }
}

// Function to update character positions based on states
void updateCharacterPositions() {
    for (int i = 0; i < NUM_CHARACTERS; i++) {
        if (characterStates[i] == MOVE_FORWARD) {
            characterPositions[i].x += CHARACTER_SPEED;
        } else if (characterStates[i] == MOVE_BACKWARD) {
            characterPositions[i].x -= CHARACTER_SPEED;
        }

        if (characterPositions[i].x < 0) {
            characterPositions[i].x = 0;
        }
        if (characterPositions[i].x > 800 - characterPositions[i].w) {
            characterPositions[i].x = 800 - characterPositions[i].w;
        }
    }
}

// Main function to render the fighting ground
void renderFightingGround(SDL_Renderer *renderer) {
    if (!backgroundTexture) {
        backgroundTexture = IMG_LoadTexture(renderer, "../assets/images/bg.png");
        if (!backgroundTexture) {
            printf("Failed to load background texture! SDL_image Error: %s\n", IMG_GetError());
            return;
        }
    }
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    renderHealthBars(renderer);

    for (int i = 0; i < 2; i++) {
        int characterIndex = selectedCharacters[i];
        if (characterIndex < 0) continue;

        SDL_Texture *currentTexture;
        int frameCount;
        int moveType = characterStates[i];

        if (moveType >= 0 && moveType < NUM_MOVES) {
            currentTexture = characterFrames[characterIndex][moveType][currentFrame[i]];
            frameCount = characterFrameCounts[characterIndex][moveType];
        } else {
            currentTexture = characterFrames[characterIndex][MOVE_STANDING][currentFrame[i]];
            frameCount = characterFrameCounts[characterIndex][MOVE_STANDING];
        }

        if (characterStates[i] != previousCharacterStates[i]) {
            currentFrame[i] = 0;
            previousCharacterStates[i] = characterStates[i];
        }

        if (currentTexture) {
            Uint64 currentTime = SDL_GetTicks64();
            if (currentTime > lastFrameTime[i] + FRAME_DELAY) {
                currentFrame[i] = (currentFrame[i] + 1) % frameCount;
                lastFrameTime[i] = currentTime;
            }

            int characterWidth, characterHeight;
            SDL_QueryTexture(currentTexture, NULL, NULL, &characterWidth, &characterHeight);
            SDL_Rect characterDestRect = characterPositions[i];
            characterDestRect.w = characterWidth;
            characterDestRect.h = characterHeight;

            SDL_RendererFlip flip = (i == 1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            SDL_RenderCopyEx(renderer, currentTexture, NULL, &characterDestRect, 0, NULL, flip);
        }
    }
    handleCharacterAttacks();
}
