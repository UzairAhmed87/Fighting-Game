#include "../include/player.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

Player *createPlayer(SDL_Renderer *renderer, const char *filePath, int x, int y) {
    // Allocate memory for the player
    Player *player = (Player *)malloc(sizeof(Player));
    if (player == NULL) {
        printf("Failed to allocate memory for player.\n");
        return NULL;
    }

    // Load texture
    SDL_Surface *surface = IMG_Load(filePath);
    if (!surface) {
        printf("IMG_Load Error: %s\n", IMG_GetError());
        free(player);
        return NULL;
    }

    player->texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!player->texture) {
        printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
        free(player);
        return NULL;
    }

    // Set the initial position and size
    player->position.x = x;
    player->position.y = y;
    player->position.w = 75; // Set width of player, e.g., 64 pixels
    player->position.h = 150; // Set height of player, e.g., 64 pixels

    return player;
}

void renderPlayer(SDL_Renderer *renderer, Player *player) {
    // Render player texture
    SDL_RenderCopy(renderer, player->texture, NULL, &player->position);
}

void destroyPlayer(Player *player) {
    if (player) {
        if (player->texture) {
            SDL_DestroyTexture(player->texture);
        }
        free(player);
    }
}

void updatePlayerPosition(Player *player, int dx, int dy, int windowWidth, int windowHeight) {
    // Update the player position based on dx, dy values
    player->position.x += dx;
    player->position.y += dy;

    // Boundary checking
    if (player->position.x < 0) {
        player->position.x = 0;
    }
    if (player->position.x + player->position.w > windowWidth) {
        player->position.x = windowWidth - player->position.w;
    }
    if (player->position.y < 0) {
        player->position.y = 0;
    }
    if (player->position.y + player->position.h > windowHeight) {
        player->position.y = windowHeight - player->position.h;
    }
}
