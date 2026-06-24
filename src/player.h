#pragma once
#include "raylib.h"

#define DAVID_FRAME_W 32
#define DAVID_FRAME_H 40
#define DAVID_SHEET_FRAMES 8

typedef enum { DIR_DOWN, DIR_LEFT, DIR_RIGHT, DIR_UP } PlayerDir;

typedef struct {
    float x, y;
    int hp;
    int maxHp;
    int ammo;
    Texture2D sheet;
    int currentFrame;
    PlayerDir dir;
    float animTimer;
    float shootCooldown;
    float invulnTimer;
} Player;

Player CreatePlayer();
void UpdatePlayer(Player& p);
void DrawPlayer(const Player& p);
void UnloadPlayer(Player& p);
Texture2D CreateDavidTexture();
Texture2D CreateJonathanTexture();
