#pragma once
#include "raylib.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define PLAYER_SPEED 3.0f

// === VIDA DE LOS PERSONAJES ===
#define PLAYER_MAX_HP 5    // David (Niveles 1 y 2)
#define JONATHAN_MAX_HP 5  // Jonathan (Nivel 3)

// GIGANTES
#define GOLIATH_HP 1       // Goliat (Nivel 2)
#define GIANT2_HP 3        // Gigante filisteo (Nivel 3)

#define STONES_TOTAL 7
#define STONES_NEEDED 5
#define BUSH_COUNT 5

#define AMMO_MAX 5
#define GOLIATH_SPEED 1.2f
#define GOLIATH_SHOOT_INTERVAL 2.5f
#define GOLIATH_PROJ_SPEED 3.0f
#define PLAYER_PROJ_SPEED 8.0f
#define GOLIATH_HITBOX_W 50
#define GOLIATH_HITBOX_H 60

#define GIANT2_SPEED 0.8f
#define GIANT2_SHOOT_INTERVAL 4.0f
#define GIANT2_PROJ_SPEED 3.5f
#define GIANT2_PROJ_MAX 8
#define GIANT2_MELEE_RANGE 80
#define GIANT2_MELEE_COOLDOWN 2.5f

#define JONATHAN_SPEED 3.5f

#define PLAYER_COL_W 20
#define PLAYER_COL_H 24
#define DIALOG_H 200
#define DIALOG_Y (SCREEN_HEIGHT - DIALOG_H)

typedef enum { MENU, HISTORIA, DIALOGO, INSTRUCCIONES, NIVEL_1, NIVEL_1_COMPLETO, MAPA_MUNDI, NIVEL_2, VICTORIA, VICTORIA_HISTORIA, NIVEL_3_INTRO, NIVEL_3, NIVEL_3_VICTORIA, NIVEL_3_OUTRO, DERROTA } GameState;

void UnloadMapTerrain();
void DrawHistoria(int page, int totalPages);
void DrawMenu(int sel);
void DrawDialogo(int page, Texture2D* portrait);
void DrawNivel1Completo(int score);
void DrawGameOver(int score);
void DrawVictory(int score);
void DrawHUDLevel1(int collected, int hp, int maxHp, int score);
void DrawHUDLevel2(int ammo, int hp, int maxHp, int score);
void DrawHealthBar(int x, int y, int hp, int maxHp);

#define PORTRAIT_W 96
#define PORTRAIT_H 120
void DrawVictoriaHistoria(int page, int totalPages);
void DrawNivel3Intro(int page, int totalPages);
void DrawNivel3Outro(int page, int totalPages);
void DrawNivel3Victory(int score);
void DrawHUDLevel3(int hp, int maxHp, int giantHp, int giantMaxHp);
void DrawMapaMundi(int pathIndex, float progress, bool arrived);
void DrawInstrucciones();
Texture2D CreateDavidPortrait();
