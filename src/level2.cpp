#include "level2.h"
#include "game.h"
#include "utils.h"
#include "rlgl.h"
#include <cstdlib>
#include <cmath>

const int lvl2Map[MAP_H][MAP_W] = {
    {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,5,4,4,5,4,4,4,4,4},
    {4,4,5,4,4,5,4,4,4,4,4,4,4,5,4,4,4,4,3,3,4,4,4,5,4},
    {4,4,4,4,4,4,4,4,4,5,4,4,4,4,4,4,4,4,3,3,4,4,4,4,4},
    {4,5,4,4,4,4,5,4,4,4,4,4,4,4,4,5,4,4,3,3,3,4,4,4,4},
    {4,4,4,4,4,4,4,4,4,4,4,5,4,4,4,4,4,4,3,3,3,4,4,4,5},
    {4,4,4,5,4,4,4,4,4,4,4,4,4,4,5,4,4,4,3,3,3,4,4,4,4},
    {4,4,4,4,4,4,4,5,4,4,4,4,5,4,4,4,4,4,3,3,4,4,5,4,4},
    {4,5,4,4,4,4,4,4,4,4,4,4,4,4,4,4,5,4,4,4,4,4,4,4,4},
    {4,4,4,4,5,4,4,4,4,4,4,4,4,4,5,4,4,4,4,4,4,4,4,5,4},
    {4,4,4,4,4,4,4,4,4,4,5,4,4,4,4,4,4,4,4,5,4,4,4,4,4},
    {4,5,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,5,4,4,4,4,4,4,4},
    {4,4,4,4,4,4,5,4,4,4,4,4,4,5,4,4,4,4,4,4,4,4,5,4,4},
    {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},
    {3,4,4,4,4,4,4,4,3,3,4,4,4,4,4,3,3,4,4,4,4,4,4,3,3},
    {3,4,4,4,4,4,4,3,3,4,4,4,4,4,4,3,3,4,4,4,4,4,4,3,3},
    {4,4,4,5,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,5,4,4,4},
    {4,4,4,4,4,4,4,4,4,4,5,4,4,4,4,4,4,4,4,4,4,4,4,4,4},
    {4,4,4,4,5,4,4,4,4,4,4,4,4,4,5,4,4,4,4,4,4,5,4,4,4},
    {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},
};

static Texture2D goliathTex = { 0 };

static void CreateGoliathTexture() {
    if (goliathTex.id > 0) return;
    Image img = GenImageColor(48, 64, BLANK);

    // Helper to draw pixel
    auto px = [&](int x, int y, Color c) {
        if (x >= 0 && x < 48 && y >= 0 && y < 64)
            ImageDrawPixel(&img, x, y, c);
    };

    // Shadow (feet area)
    for (int x = 6; x < 42; x++)
        for (int y = 58; y < 62; y++)
            px(x, y, (Color){ 20, 20, 30, 60 });

    // Legs
    for (int y = 44; y < 56; y++) {
        for (int x = 12; x < 20; x++) px(x, y, (Color){ 80, 55, 30, 255 });
        for (int x = 28; x < 36; x++) px(x, y, (Color){ 80, 55, 30, 255 });
    }
    // Leg plates (armor)
    for (int y = 38; y < 46; y++) {
        for (int x = 10; x < 22; x++) px(x, y, (Color){ 120, 90, 40, 255 });
        for (int x = 26; x < 38; x++) px(x, y, (Color){ 120, 90, 40, 255 });
    }
    // Leg plate outline
    for (int x = 10; x < 22; x++) { px(x, 38, (Color){ 40, 30, 20, 255 }); px(x, 45, (Color){ 40, 30, 20, 255 }); }
    for (int x = 26; x < 38; x++) { px(x, 38, (Color){ 40, 30, 20, 255 }); px(x, 45, (Color){ 40, 30, 20, 255 }); }

    // Body (chest armor)
    for (int y = 22; y < 40; y++)
        for (int x = 12; x < 36; x++)
            px(x, y, (Color){ 130, 75, 35, 255 });
    // Body darker (left side)
    for (int y = 22; y < 40; y++)
        for (int x = 12; x < 18; x++)
            px(x, y, (Color){ 100, 55, 25, 255 });
    // Body highlight (top)
    for (int x = 14; x < 34; x++)
        px(x, 22, (Color){ 160, 100, 45, 255 });
    // Body outline
    for (int x = 12; x < 36; x++) { px(x, 22, (Color){ 40, 30, 20, 255 }); px(x, 39, (Color){ 40, 30, 20, 255 }); }
    for (int y = 22; y < 40; y++) { px(12, y, (Color){ 40, 30, 20, 255 }); px(35, y, (Color){ 40, 30, 20, 255 }); }

    // Belt
    for (int x = 12; x < 36; x++)
        px(x, 38, (Color){ 60, 45, 25, 255 });
    px(12, 38, (Color){ 40, 30, 20, 255 }); px(35, 38, (Color){ 40, 30, 20, 255 });
    // Belt buckle
    for (int x = 20; x < 28; x++)
        for (int y = 36; y < 40; y++)
            px(x, y, (Color){ 210, 170, 40, 255 });

    // Arms
    // Left arm (shield arm)
    for (int y = 24; y < 38; y++)
        for (int x = 6; x < 12; x++)
            px(x, y, (Color){ 235, 195, 155, 255 });
    // Shield
    for (int y = 22; y < 36; y++)
        for (int x = 2; x < 10; x++)
            px(x, y, (Color){ 100, 70, 40, 255 });
    for (int x = 2; x < 10; x++) { px(x, 22, (Color){ 40, 30, 20, 255 }); px(x, 35, (Color){ 40, 30, 20, 255 }); }
    for (int y = 22; y < 36; y++) { px(2, y, (Color){ 40, 30, 20, 255 }); px(9, y, (Color){ 40, 30, 20, 255 }); }
    // Shield emblem
    for (int x = 4; x < 8; x++)
        for (int y = 26; y < 32; y++)
            px(x, y, (Color){ 180, 50, 50, 255 });

    // Right arm (weapon arm)
    for (int y = 24; y < 38; y++)
        for (int x = 36; x < 42; x++)
            px(x, y, (Color){ 235, 195, 155, 255 });
    // Spear
    for (int y = 10; y < 26; y++)
        px(40, y, (Color){ 100, 65, 30, 255 });
    px(40, 10, (Color){ 40, 30, 20, 255 });
    // Spearhead
    px(39, 8, (Color){ 180, 180, 190, 255 });
    px(40, 8, (Color){ 180, 180, 190, 255 });
    px(41, 8, (Color){ 180, 180, 190, 255 });
    px(40, 7, (Color){ 180, 180, 190, 255 });
    px(40, 9, (Color){ 180, 180, 190, 255 });
    px(40, 9, (Color){ 200, 200, 210, 255 });

    // Neck
    for (int y = 18; y < 22; y++)
        for (int x = 20; x < 28; x++)
            px(x, y, (Color){ 210, 170, 130, 255 });

    // Head (helmet + face)
    // Helmet
    for (int y = 6; y < 18; y++)
        for (int x = 16; x < 32; x++)
            px(x, y, (Color){ 140, 95, 35, 255 });
    // Helmet darker (left)
    for (int y = 6; y < 18; y++)
        for (int x = 16; x < 20; x++)
            px(x, y, (Color){ 110, 70, 25, 255 });
    // Helmet outline
    for (int x = 16; x < 32; x++) { px(x, 6, (Color){ 40, 30, 20, 255 }); px(x, 17, (Color){ 40, 30, 20, 255 }); }
    for (int y = 6; y < 18; y++) { px(16, y, (Color){ 40, 30, 20, 255 }); px(31, y, (Color){ 40, 30, 20, 255 }); }
    // Helmet crest (top)
    for (int x = 20; x < 28; x++) px(x, 4, (Color){ 180, 50, 50, 255 });
    for (int x = 22; x < 26; x++) px(x, 3, (Color){ 180, 50, 50, 255 });
    px(24, 2, (Color){ 180, 50, 50, 255 });

    // Face inside helmet
    for (int y = 12; y < 17; y++)
        for (int x = 24; x < 30; x++)
            px(x, y, (Color){ 185, 120, 65, 255 });
    // Face detail
    px(26, 13, BLACK); // eye
    px(25, 15, (Color){ 180, 100, 80, 255 }); // mouth

    // Helmet visor
    for (int x = 18; x < 24; x++)
        px(x, 13, (Color){ 110, 70, 25, 255 });
    for (int x = 18; x < 24; x++)
        px(x, 14, (Color){ 90, 55, 20, 255 });

    // Cape
    for (int y = 18; y < 40; y++)
        for (int x = 6; x < 12; x++)
            px(x, y, (Color){ 120, 30, 30, 255 });
    for (int y = 20; y < 40; y++)
        for (int x = 4; x < 7; x++)
            px(x, y, (Color){ 90, 20, 20, 255 });

    goliathTex = LoadTextureFromImage(img);
    UnloadImage(img);
}

// --- Goliath's Projectiles ---

static void DrawPhilistineProj(const GoliathProj& p) {
    if (!p.active) return;
    float angle = atan2f(p.speedY, p.speedX) * RAD2DEG;
    rlPushMatrix();
    rlTranslatef(p.x, p.y, 0);
    rlRotatef(angle, 0, 0, 1);
    // Arrow shaft (pixel art)
    utils::DrawLineDDA(0, 0, 14, 0, (Color){ 80, 55, 30, 255 });
    // Arrowhead
    DrawPixel(14, 0, (Color){ 180, 180, 190, 255 });
    DrawPixel(15, 0, (Color){ 180, 180, 190, 255 });
    DrawPixel(15, -1, (Color){ 180, 180, 190, 255 });
    DrawPixel(15, 1, (Color){ 180, 180, 190, 255 });
    // Fletching
    DrawPixel(0, -2, (Color){ 180, 50, 50, 255 });
    DrawPixel(0, 2, (Color){ 180, 50, 50, 255 });
    DrawPixel(0, -3, (Color){ 180, 50, 50, 255 });
    DrawPixel(0, 3, (Color){ 180, 50, 50, 255 });
    rlPopMatrix();
}

// --- Goliath ---

Goliath CreateGoliath() {
    CreateGoliathTexture();
    Goliath g;
    g.x = 700.0f;
    g.y = 300.0f;
    g.hp = 1;
    g.speed = GOLIATH_SPEED;
    g.shootTimer = GOLIATH_SHOOT_INTERVAL;
    g.animTimer = 0.0f;
    for (int i = 0; i < GOLIATH_PROJ_MAX; i++)
        g.projectiles[i].active = false;
    return g;
}

void UpdateGoliath(Goliath& g, float tx, float ty, float dt) {
    float dx = tx - g.x;
    float dy = ty - g.y;
    float dist = sqrtf(dx * dx + dy * dy);
    if (dist > 0) {
        g.x += (dx / dist) * g.speed;
        g.y += (dy / dist) * g.speed;
    }
    if (g.x < 100) g.x = 100;
    if (g.x > SCREEN_WIDTH - 30) g.x = SCREEN_WIDTH - 30;
    if (g.y < 40) g.y = 40;
    if (g.y > SCREEN_HEIGHT - 40) g.y = SCREEN_HEIGHT - 40;

    g.shootTimer -= dt;
    if (g.shootTimer <= 0) {
        for (int i = 0; i < GOLIATH_PROJ_MAX; i++) {
            if (!g.projectiles[i].active) {
                g.projectiles[i].x = g.x;
                g.projectiles[i].y = g.y;
                float pdx = tx - g.x;
                float pdy = ty - g.y;
                float pdist = sqrtf(pdx * pdx + pdy * pdy);
                if (pdist > 0) {
                    g.projectiles[i].speedX = (pdx / pdist) * GOLIATH_PROJ_SPEED;
                    g.projectiles[i].speedY = (pdy / pdist) * GOLIATH_PROJ_SPEED;
                }
                g.projectiles[i].active = true;
                g.shootTimer = GOLIATH_SHOOT_INTERVAL;
                break;
            }
        }
    }

    for (int i = 0; i < GOLIATH_PROJ_MAX; i++) {
        if (g.projectiles[i].active) {
            g.projectiles[i].x += g.projectiles[i].speedX;
            g.projectiles[i].y += g.projectiles[i].speedY;
            if (g.projectiles[i].x < -50 || g.projectiles[i].x > SCREEN_WIDTH + 50 ||
                g.projectiles[i].y < -50 || g.projectiles[i].y > SCREEN_HEIGHT + 50) {
                g.projectiles[i].active = false;
            }
        }
    }

    g.animTimer += dt;
}

void DrawGoliath(const Goliath& g) {
    Rectangle src = { 0, 0, 48, 64 };
    Rectangle dst = { g.x, g.y, 48, 64 };
    Vector2 origin = { 24, 32 };
    DrawTexturePro(goliathTex, src, dst, origin, 0.0f, WHITE);

    for (int i = 0; i < GOLIATH_PROJ_MAX; i++)
        DrawPhilistineProj(g.projectiles[i]);
}

// --- Level 2 Background ---

void DrawLevel2Background() {
    DrawTileMap(lvl2Map);
}
