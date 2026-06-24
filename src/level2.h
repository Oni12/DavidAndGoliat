#pragma once
#include "raylib.h"
#include "tiles.h"

#define GOLIATH_PROJ_MAX 10

typedef struct {
    float x, y;
    float speedX, speedY;
    bool active;
} GoliathProj;

typedef struct {
    float x, y;
    int hp;
    float speed;
    float shootTimer;
    GoliathProj projectiles[GOLIATH_PROJ_MAX];
    float animTimer;
} Goliath;

extern const int lvl2Map[MAP_H][MAP_W];

Goliath CreateGoliath();
void UpdateGoliath(Goliath& g, float tx, float ty, float dt);
void DrawGoliath(const Goliath& g);
void DrawLevel2Background();
