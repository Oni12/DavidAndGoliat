#pragma once
#include "raylib.h"
#include "tiles.h"

#define GIANT2_PROJ_MAX 8

typedef struct {
    float x, y;
    float speedX, speedY;
    bool active;
} Giant2Proj;

typedef struct {
    float x, y;
    int hp;
    float speed;
    float shootTimer;
    Giant2Proj projectiles[GIANT2_PROJ_MAX];
    float animTimer;
} Giant2;

extern const int lvl3Map[MAP_H][MAP_W];

Giant2 CreateGiant2();
void UpdateGiant2(Giant2& g, float tx, float ty, float dt);
void DrawGiant2(const Giant2& g);
void DrawLevel3Background();
