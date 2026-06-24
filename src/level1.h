#pragma once
#include "raylib.h"
#include "tiles.h"

typedef struct {
    float x, y;
} Bush;

typedef struct {
    float x, y;
    bool collected;
} Stone;

extern const int lvl1Map[MAP_H][MAP_W];

void InitStones(Stone* stones, int count, const Bush* bushes, int bushCount);
void DrawStones(const Stone* stones, int count);
void InitBushes(Bush* bushes, int count);
void DrawBushes(const Bush* bushes, int count);
void DrawLevel1Background();
