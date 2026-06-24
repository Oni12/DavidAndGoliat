#pragma once
#include "raylib.h"

#define TILE_SIZE 32
#define MAP_W 25
#define MAP_H 19

typedef enum {
    TILE_GRASS = 0,
    TILE_GRASS_VAR,
    TILE_WATER,
    TILE_DIRT,
    TILE_SAND,
    TILE_SAND_VAR,
    TILE_SHORE_T,
    TILE_SHORE_B,
    TILE_SHORE_L,
    TILE_SHORE_R,
    TILE_COUNT
} TileType;

void CreateTileTextures();
void UnloadTileTextures();
void DrawTileMap(const int map[MAP_H][MAP_W]);

extern Texture2D tileTextures[TILE_COUNT];
