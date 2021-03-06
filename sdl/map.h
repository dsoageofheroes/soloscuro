#ifndef MAP_H
#define MAP_H

#include <SDL2/SDL.h>
#include "animate.h"
#include "../src/region.h"
#include "screen-manager.h"

typedef struct map_s {
    region_t *region;
    SDL_Texture **tiles;
} map_t;

void map_load_map(SDL_Renderer *renderer, int id);
void map_load_region(region_t *reg, SDL_Renderer *renderer);
void map_render(void *data, SDL_Renderer *renderer);
int map_handle_mouse(const uint32_t x, const uint32_t y);
int map_handle_mouse_click(const uint32_t button, const uint32_t x, const uint32_t y);

int cmap_is_block(const int row, const int column);
int cmap_is_actor(const int row, const int column);
int cmap_is_danger(const int row, const int column);

// From main.c
uint32_t getCameraX();
uint32_t getCameraY();

extern sops_t map_screen;

#endif
