#include "../include/fightingGround.h"
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>

#include <string.h>
#include <stdbool.h>
#include "../include/charSelection.h"
#include <SDL_ttf.h>

// Define constants
#define FRAME_DELAY 100  
#define MAX_FRAMES 100  
#define NUM_MOVES 6     
#define MOVE_SPEED 10   
#define CHARACTER_SPEED 5 
#define MAX_HEALTH 100  // Maximum health for characters
#define ATTACK_COOLDOWN 2000 // Add a cooldown period in milliseconds 
// Global variables
SDL_Texture *backgroundTexture = NULL;
SDL_Texture *characterFrames[NUM_CHARACTERS][NUM_MOVES][MAX_FRAMES] = {NULL};
int characterFrameCounts[NUM_CHARACTERS][NUM_MOVES] = {0};
CharacterMove characterStates[NUM_CHARACTERS] = {MOVE_STANDING, MOVE_STANDING};
CharacterMove previousCharacterStates[NUM_CHARACTERS] = {MOVE_STANDING, MOVE_STANDING};
int currentFrame[NUM_CHARACTERS] = {0, 0};
Uint64 lastFrameTime[NUM_CHARACTERS] = {0, 0};

 // Track the last attack time for each character
Uint64 lastAttackTime[2] = {0, 0};

// Health values for both characters
int characterHealth[2] = {MAX_HEALTH, MAX_HEALTH};

// Positions for characters in the fighting ground
SDL_Rect characterPositions[2] = {
    {100, 450, 80, 100}, // Position for the first character
    {550, 450, 80, 100}  // Position for the second character
};

SDL_Rect buttonRect = {325, 300, 150, 50};

// Function to load character frames
void loadCharacterFrames(SDL_Renderer *renderer) {
    char framePath[150];
    const char *moveNames[NUM_MOVES] = {"stand", "forward", "backward", "kick", "punch","dead"};

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
        int healthBarWidth = (characterHealth[i] * 200) / MAX_HEALTH; 
        SDL_Rect healthBarRect;

        if (i == 0) {
            healthBarRect = (SDL_Rect){20, 20, healthBarWidth, 20}; 
        } else {
            healthBarRect = (SDL_Rect){580, 20, healthBarWidth, 20}; 
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


bool gameOver = false; // To check if the game is over
int winner = -1; // To store the winner (0 or 1)

// Function to handle character attacks, health updates with pushback logic
void handleCharacterAttacks() {
   
    SDL_Rect attackRect[2];
    Uint64 currentTime = SDL_GetTicks64();

    // Define attack range offsets
    const int PUNCH_RANGE = 0;  
    const int KICK_RANGE = 0;   
    const int PUSHBACK_DISTANCE = 30;  

    // Set up attack rectangles based on current attack state
    for (int i = 0; i < 2; i++) {
        if (characterStates[i] == MOVE_PUNCH || characterStates[i] == MOVE_KICK) {
            attackRect[i] = characterPositions[i];

            if (i == 0) { // Character 1 attacks to the right
                attackRect[i].x += attackRect[i].w;
                attackRect[i].w = (characterStates[i] == MOVE_PUNCH) ? PUNCH_RANGE : KICK_RANGE;
            } else { // Character 2 attacks to the left
                attackRect[i].x -= (characterStates[i] == MOVE_PUNCH) ? PUNCH_RANGE : KICK_RANGE;
                attackRect[i].w = (characterStates[i] == MOVE_PUNCH) ? PUNCH_RANGE : KICK_RANGE;
            }
        } else {
            attackRect[i] = (SDL_Rect){0, 0, 0, 0};
        }
    }

    // Check for collisions and update health with cooldown and pushback logic
    if (checkCollision(attackRect[0], characterPositions[1]) && 
        currentTime > lastAttackTime[0] + ATTACK_COOLDOWN) {
        if (characterStates[0] == MOVE_PUNCH) {
            characterHealth[1] -= 5;
        } else if (characterStates[0] == MOVE_KICK) {
            characterHealth[1] -= 10;
        }
        if (characterHealth[1] < 0) characterHealth[1] = 0;
        lastAttackTime[0] = currentTime;

        characterPositions[1].x += PUSHBACK_DISTANCE;
        if (characterPositions[1].x > 800 - characterPositions[1].w) {
            characterPositions[1].x = 800 - characterPositions[1].w;
        }
    }

    if (checkCollision(attackRect[1], characterPositions[0]) && 
        currentTime > lastAttackTime[1] + ATTACK_COOLDOWN) {
        if (characterStates[1] == MOVE_PUNCH) {
            characterHealth[0] -= 5;
        } else if (characterStates[1] == MOVE_KICK) {
            characterHealth[0] -= 10;
        }
        if (characterHealth[0] < 0) characterHealth[0] = 0;
        lastAttackTime[1] = currentTime;

        characterPositions[0].x -= PUSHBACK_DISTANCE;
        if (characterPositions[0].x < 0) {
            characterPositions[0].x = 0;
        }
    }

    // Check if any player has died and trigger the dead animation
    if (characterHealth[0] == 0 && !gameOver) {
        gameOver = true;
        winner = 1;  // Player 2 wins
        
        characterStates[0] = MOVE_DEAD; // Assume to have a "dead" state for the character
        
        
    }
    if (characterHealth[1] == 0 && !gameOver) {
        gameOver = true;
        winner = 0;  // Player 1 wins
        
        characterStates[1] = MOVE_DEAD; // Assume we have a "dead" state for the character
    }
}

// Function to handle events and update character states
void handleEvents(SDL_Event *event,GameState *currentState) {
     if (gameOver) {
        if (event->type == SDL_MOUSEBUTTONDOWN) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            if (x >= buttonRect.x && x <= buttonRect.x + buttonRect.w &&
                y >= buttonRect.y && y <= buttonRect.y + buttonRect.h) {
                *currentState = MAIN_MENU;  // Return to main menu
                resetGameState();
            }
        }
        return;
    }
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
       if (gameOver) return;
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
SDL_Rect Messagerect = {400, 300, 0, 0};  

void renderGameOver(SDL_Renderer *renderer, TTF_Font *font) {
    // Draw a semi-transparent background over the entire window
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Black with transparency
    SDL_Rect fullWindowRect = {0, 0, 800, 600}; // Assuming your window size is 800x600
    SDL_RenderFillRect(renderer, &fullWindowRect);

    // Render the winner message
    const char *winnerMessage = (winner == 0) ? "Player 1 Wins!" : "Player 2 Wins!";
    SDL_Color whiteColor = {255, 255, 255, 255}; // White text

    // Calculate text size and center the message
    int textWidth, textHeight;
    TTF_SizeText(font, winnerMessage, &textWidth, &textHeight);
    Messagerect.x = (800 - textWidth) / 2;
    Messagerect.y = (600 - textHeight) / 2 - 50; // Adjust Y for positioning
    Messagerect.w = textWidth;
    Messagerect.h = textHeight;

    // Draw a background rectangle for the message
    SDL_Rect messageBackground = {Messagerect.x - 20, Messagerect.y - 10, textWidth + 40, textHeight + 20};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200); // Black with some transparency
    SDL_RenderFillRect(renderer, &messageBackground);

    renderText(renderer, winnerMessage, font, whiteColor, &Messagerect);

    // Render "Back to Main Menu" button
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White button background
    SDL_RenderFillRect(renderer, &buttonRect);

    // Calculate and center button text
    const char *buttonText = "Main Menu";
    SDL_Color blackColor = {0, 0, 0, 255}; // Black text
    int buttonTextWidth, buttonTextHeight;
    TTF_SizeText(font, buttonText, &buttonTextWidth, &buttonTextHeight);

    buttonRect.x = (800 - buttonRect.w) / 2; // Center horizontally
    SDL_Rect buttonTextRect;
    buttonTextRect.x = buttonRect.x + (buttonRect.w - buttonTextWidth) / 2;
    buttonTextRect.y = buttonRect.y + (buttonRect.h - buttonTextHeight) / 2;
    buttonTextRect.w = buttonTextWidth;
    buttonTextRect.h = buttonTextHeight;

    renderText(renderer, buttonText, font, blackColor, &buttonTextRect);
}



// Main function to render the fighting ground
void renderFightingGround(SDL_Renderer *renderer, TTF_Font *font) {
    if (gameOver) {
        
        renderGameOver(renderer, font);
        return; // Exit early if the game is over
    }

    // Render the background
    if (!backgroundTexture) {
        backgroundTexture = IMG_LoadTexture(renderer, "../assets/images/bg.png");
        if (!backgroundTexture) {
            printf("Failed to load background texture! SDL_image Error: %s\n", IMG_GetError());
            return;
        }
    }
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

    // Render health bars
    renderHealthBars(renderer);

    // Render characters
    for (int i = 0; i < 2; i++) {
        int characterIndex = selectedCharacters[i];
        if (characterIndex < 0) continue;

        SDL_Texture *currentTexture;
        int frameCount;
        int moveType = characterStates[i];

        if (moveType == MOVE_DEAD) {
            currentTexture = characterFrames[characterIndex][MOVE_DEAD][currentFrame[i]];
            frameCount = characterFrameCounts[characterIndex][MOVE_DEAD];
        } else if (moveType >= 0 && moveType < NUM_MOVES) {
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

    handleCharacterAttacks(); // Handle character attacks logic
}
void resetGameState() {
    // Reset health
    characterHealth[0] = MAX_HEALTH;
    characterHealth[1] = MAX_HEALTH;

    // Reset character states
    characterStates[0] = MOVE_STANDING;
    characterStates[1] = MOVE_STANDING;
    previousCharacterStates[0] = MOVE_STANDING;
    previousCharacterStates[1] = MOVE_STANDING;
    
    // Reset frames
    currentFrame[0] = 0;
    currentFrame[1] = 0;
    lastFrameTime[0] = 0;
    lastFrameTime[1] = 0;

    // Reset positions
    characterPositions[0] = (SDL_Rect){100, 450, 80, 100};
    characterPositions[1] = (SDL_Rect){550, 450, 80, 110};

    // Reset attack cooldowns
    lastAttackTime[0] = 0;
    lastAttackTime[1] = 0;

    // Reset game-over flag and winner
    gameOver = false;
    winner = -1;
}  