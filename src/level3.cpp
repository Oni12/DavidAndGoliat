#include "level3.h"
#include "game.h"
#include <cmath>
#include <cstdlib>

static Texture2D giant2Tex = { 0 };
static bool giant2TexLoaded = false;

static void CreateGiant2Texture() {
    if (giant2TexLoaded) return;
    const int w = 64, h = 64;
    Image img = GenImageColor(w, h, BLANK);
    auto px = [&](int x, int y, Color c) {
        if (x >= 0 && x < w && y >= 0 && y < h) ImageDrawPixel(&img, x, y, c);
    };
    Color SKIN   = { 200, 160, 130, 255 };
    Color SKIN_DK= { 160, 125, 100, 255 };
    Color ARMOR  = { 120, 30, 30, 255 };
    Color ARMOR_DK={ 80, 18, 18, 255 };
    Color ARMOR_LT={ 160, 50, 50, 255 };
    Color HELMET = { 90, 85, 80, 255 };
    Color HELMET_DK={ 60, 55, 50, 255 };
    Color METAL  = { 180, 175, 165, 255 };
    Color METAL_DK={ 130, 125, 115, 255 };
    Color BELT2  = { 100, 70, 40, 255 };
    Color BUCKLE2= { 200, 170, 50, 255 };
    Color PANTS2 = { 70, 50, 35, 255 };
    Color BOOTS  = { 55, 40, 25, 255 };
    Color OUTLINE= { 25, 18, 12, 255 };
    Color SHIELD = { 60, 55, 90, 255 };
    Color SHIELD_LT={ 85, 80, 120, 255 };
    Color SHIELD_G= { 180, 150, 40, 255 };

    // ---- LEGS ----
    for (int y = 42; y < 54; y++) {
        for (int x = 20; x < 30; x++) px(x, y, PANTS2);
        for (int x = 34; x < 44; x++) px(x, y, PANTS2);
    }
    // Leg shading
    for (int y = 42; y < 54; y++) {
        for (int x = 20; x < 23; x++) px(x, y, BOOTS);
        for (int x = 41; x < 44; x++) px(x, y, BOOTS);
    }
    // Boots
    for (int y = 54; y < 62; y++) {
        for (int x = 18; x < 32; x++) px(x, y, BOOTS);
        for (int x = 32; x < 46; x++) px(x, y, BOOTS);
    }
    // Boot soles
    for (int x = 18; x < 46; x++) px(x, 61, OUTLINE);
    for (int x = 18; x < 46; x++) px(x, 62, (Color){ 35, 22, 12, 255 });

    // ---- TORSO (armor) ----
    for (int y = 20; y < 42; y++)
        for (int x = 14; x < 50; x++)
            px(x, y, ARMOR);
    // Armor shading
    for (int y = 20; y < 42; y++) {
        for (int x = 14; x < 18; x++) px(x, y, ARMOR_DK);
        for (int x = 46; x < 50; x++) px(x, y, ARMOR_DK);
    }
    // Armor chest plate highlight
    for (int y = 22; y < 32; y++)
        for (int x = 22; x < 42; x++)
            px(x, y, ARMOR_LT);
    // Armor detail lines
    for (int y = 24; y < 30; y++) {
        px(24, y, ARMOR_DK); px(26, y, ARMOR_DK);
        px(37, y, ARMOR_DK); px(39, y, ARMOR_DK);
    }
    // Belt
    for (int x = 14; x < 50; x++) px(x, 39, BELT2);
    for (int x = 14; x < 50; x++) px(x, 40, BELT2);
    // Buckle
    for (int x = 28; x < 36; x++) { px(x, 38, BUCKLE2); px(x, 41, BUCKLE2); }
    for (int y = 39; y < 41; y++) { px(28, y, BUCKLE2); px(35, y, BUCKLE2); }
    px(30, 39, (Color){ 255, 210, 60, 255 }); px(33, 39, (Color){ 255, 210, 60, 255 });

    // ---- SKIRT / KILT ----
    for (int y = 40; y < 46; y++)
        for (int x = 16; x < 48; x++)
            px(x, y, ARMOR_DK);
    for (int y = 42; y < 46; y++) {
        px(16, y, ARMOR); px(20, y, ARMOR); px(24, y, ARMOR);
        px(28, y, ARMOR); px(32, y, ARMOR); px(36, y, ARMOR);
        px(40, y, ARMOR); px(44, y, ARMOR); px(47, y, ARMOR);
    }

    // ---- ARMS ----
    // Left arm (shield side)
    for (int y = 18; y < 34; y++)
        for (int x = 6; x < 14; x++) px(x, y, SKIN);
    for (int y = 18; y < 34; y++)
        for (int x = 6; x < 9; x++) px(x, y, SKIN_DK);
    // Right arm (weapon side)
    for (int y = 18; y < 34; y++)
        for (int x = 50; x < 58; x++) px(x, y, SKIN);
    for (int y = 18; y < 34; y++)
        for (int x = 55; x < 58; x++) px(x, y, SKIN_DK);
    // Shoulder pads
    for (int y = 16; y < 22; y++) {
        for (int x = 4; x < 16; x++) px(x, y, METAL);
        for (int x = 48; x < 60; x++) px(x, y, METAL);
    }
    for (int y = 16; y < 22; y++) {
        px(4, y, METAL_DK); px(5, y, METAL_DK);
        px(58, y, METAL_DK); px(59, y, METAL_DK);
    }

    // ---- HANDS (6 fingers each) ----
    // Left hand (shield hand)
    for (int y = 32; y < 38; y++)
        for (int x = 4; x < 10; x++) px(x, y, SKIN);
    // 6 fingers on left hand
    px(2, 33, SKIN); px(3, 33, SKIN);
    px(2, 34, SKIN); px(3, 34, SKIN);
    px(2, 35, SKIN); px(3, 35, SKIN);
    px(2, 36, SKIN); px(3, 36, SKIN);
    px(4, 32, SKIN); px(1, 34, SKIN); // extra fingers for 6
    // Right hand (weapon hand)
    for (int y = 32; y < 38; y++)
        for (int x = 54; x < 60; x++) px(x, y, SKIN);
    // 6 fingers on right hand
    px(60, 33, SKIN); px(61, 33, SKIN);
    px(60, 34, SKIN); px(61, 34, SKIN);
    px(60, 35, SKIN); px(61, 35, SKIN);
    px(60, 36, SKIN); px(61, 36, SKIN);
    px(59, 32, SKIN); px(62, 34, SKIN); // extra fingers for 6
    // Finger outlines
    for (int x = 2; x <= 3; x++) { px(x, 33, OUTLINE); px(x, 36, OUTLINE); }
    for (int x = 60; x <= 61; x++) { px(x, 33, OUTLINE); px(x, 36, OUTLINE); }

    // ---- WEAPON (club) ----
    // Handle
    for (int y = 20; y < 34; y++) {
        px(62, y, (Color){ 120, 80, 40, 255 });
        px(63, y, (Color){ 100, 65, 30, 255 });
    }
    // Club head
    for (int y = 14; y < 22; y++)
        for (int x = 58; x < 68; x++)
            px(x, y, (Color){ 140, 95, 45, 255 });
    for (int y = 14; y < 22; y++)
        for (int x = 58; x < 61; x++)
            px(x, y, (Color){ 100, 65, 30, 255 });
    // Spikes on club
    px(60, 13, METAL); px(63, 13, METAL); px(66, 13, METAL);
    px(67, 15, METAL); px(67, 18, METAL);
    px(58, 15, METAL); px(58, 18, METAL);

    // ---- SHIELD ----
    for (int y = 20; y < 40; y++)
        for (int x = 2; x < 12; x++) px(x, y, SHIELD);
    // Shield rim
    for (int x = 2; x < 12; x++) { px(x, 20, METAL); px(x, 39, METAL); }
    for (int y = 20; y < 40; y++) { px(2, y, METAL); px(11, y, METAL); }
    // Shield emblem (star)
    px(6, 26, SHIELD_G); px(7, 26, SHIELD_G);
    px(5, 27, SHIELD_G); px(8, 27, SHIELD_G);
    px(6, 28, SHIELD_G); px(7, 28, SHIELD_G);
    px(5, 29, SHIELD_G); px(8, 29, SHIELD_G);
    px(6, 30, SHIELD_G); px(7, 30, SHIELD_G);

    // ---- HEAD & HELMET ----
    // Neck
    for (int y = 12; y < 18; y++)
        for (int x = 26; x < 38; x++) px(x, y, SKIN);
    // Neck shading
    for (int x = 26; x < 29; x++)
        for (int y = 12; y < 18; y++) px(x, y, SKIN_DK);

    // Face
    for (int y = 4; y < 14; y++)
        for (int x = 24; x < 40; x++) px(x, y, SKIN);
    // Helmet base (head top)
    for (int y = 0; y < 6; y++)
        for (int x = 22; x < 42; x++) px(x, y, HELMET);
    // Helmet dome
    for (int y = 0; y < 4; y++)
        for (int x = 24; x < 40; x++) px(x, y, HELMET_DK);
    // Helmet ridge
    for (int y = 0; y < 4; y++) {
        px(30, y, METAL); px(31, y, METAL);
        px(32, y, METAL); px(33, y, METAL);
    }
    // Horns
    for (int y = 0; y < 8; y++) {
        if (22 - y >= 0) px(22 - y/2, y, METAL);
        if (42 + y/2 < 64) px(42 + y/2, y, METAL);
    }
    // Horn tips
    px(21, 1, METAL_DK); px(20, 2, METAL_DK);
    px(46, 1, METAL_DK); px(47, 2, METAL_DK);

    // ---- FACE DETAILS ----
    // Eyes (angry)
    px(28, 8, (Color){ 255, 220, 50, 255 });
    px(29, 8, (Color){ 255, 220, 50, 255 });
    px(34, 8, (Color){ 255, 220, 50, 255 });
    px(35, 8, (Color){ 255, 220, 50, 255 });
    // Pupils
    px(29, 8, (Color){ 200, 20, 20, 255 });
    px(34, 8, (Color){ 200, 20, 20, 255 });
    // Angry eyebrows
    for (int x = 26; x <= 30; x++) px(x, 6, OUTLINE);
    for (int x = 33; x <= 37; x++) px(x, 6, OUTLINE);
    // Beard
    for (int y = 12; y < 18; y++)
        for (int x = 26; x < 38; x++) px(x, y, (Color){ 60, 35, 20, 255 });
    for (int y = 14; y < 18; y++) {
        px(25, y, (Color){ 60, 35, 20, 255 });
        px(38, y, (Color){ 60, 35, 20, 255 });
    }
    // Mouth (snarling)
    px(30, 12, (Color){ 180, 60, 50, 255 });
    px(31, 12, (Color){ 180, 60, 50, 255 });
    px(32, 12, (Color){ 180, 60, 50, 255 });
    px(33, 12, (Color){ 180, 60, 50, 255 });

    // ---- OUTLINES ----
    for (int x = 14; x < 50; x++) { px(x, 20, OUTLINE); px(x, 41, OUTLINE); }
    for (int y = 20; y < 41; y++) { px(14, y, OUTLINE); px(49, y, OUTLINE); }
    for (int x = 18; x < 46; x++) { px(x, 61, OUTLINE); }

    giant2Tex = LoadTextureFromImage(img);
    UnloadImage(img);
    giant2TexLoaded = true;
}

// ---- MAP: Fortress of Gat ----
const int lvl3Map[MAP_H][MAP_W] = {
    {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
    {3,0,0,0,0,0,3,4,4,4,4,4,4,4,4,4,4,4,4,3,0,0,0,0,3},
    {3,0,0,0,0,0,3,4,5,4,5,4,4,5,4,5,4,5,4,3,0,0,0,0,3},
    {3,0,0,0,0,0,3,4,4,4,4,4,4,4,4,4,4,4,4,3,0,0,0,0,3},
    {3,0,0,0,0,0,3,5,4,5,4,5,4,5,4,5,4,5,4,3,0,0,0,0,3},
    {3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,3,3,3,3,3,3},
    {3,4,4,4,4,4,4,4,5,4,5,4,5,4,5,4,5,4,5,4,4,4,4,4,3},
    {3,4,5,4,5,4,4,5,4,5,4,4,4,4,4,5,4,5,4,5,4,5,4,4,3},
    {3,4,4,4,4,4,5,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,3},
    {3,4,5,4,5,4,4,5,4,5,4,4,4,4,4,5,4,5,4,5,4,5,4,4,3},
    {3,4,4,4,4,4,4,4,5,4,5,4,5,4,5,4,5,4,5,4,4,4,4,4,3},
    {3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,3,3,3,3,3,3},
    {3,0,0,0,0,0,3,5,4,5,4,5,4,5,4,5,4,5,4,3,0,0,0,0,3},
    {3,0,0,0,0,0,3,4,4,4,4,4,4,4,4,4,4,4,4,3,0,0,0,0,3},
    {3,0,0,0,0,0,3,4,5,4,5,4,4,5,4,5,4,5,4,3,0,0,0,0,3},
    {3,0,0,0,0,0,3,4,4,4,4,4,4,4,4,4,4,4,4,3,0,0,0,0,3},
    {3,0,0,0,0,0,3,4,5,4,5,4,5,4,5,4,5,4,5,3,0,0,0,0,3},
    {3,0,0,0,0,0,3,4,4,4,4,4,4,4,4,4,4,4,4,3,0,0,0,0,3},
    {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
};

Giant2 CreateGiant2() {
    Giant2 g;
    g.x = 700.0f;
    g.y = 300.0f;
    g.hp = GIANT2_HP;
    g.speed = GIANT2_SPEED;
    g.shootTimer = GIANT2_SHOOT_INTERVAL;
    g.animTimer = 0.0f;
    g.meleeTimer = 0.0f;
    g.meleeAnimTimer = 0.0f;
    g.meleeActive = false;
    for (int i = 0; i < GIANT2_PROJ_MAX; i++)
        g.projectiles[i].active = false;
    return g;
}

void UpdateGiant2(Giant2& g, float tx, float ty, float dt) {
    if (g.hp <= 0) return;

    float dx = tx - g.x;
    float dy = ty - g.y;
    float dist = sqrtf(dx * dx + dy * dy);
    if (dist > 0) {
        dx /= dist;
        dy /= dist;
    }

    g.x += dx * g.speed;
    g.y += dy * g.speed;

    if (g.x < 100) g.x = 100;
    if (g.x > SCREEN_WIDTH - 30) g.x = SCREEN_WIDTH - 30;
    if (g.y < 40) g.y = 40;
    if (g.y > SCREEN_HEIGHT - 40) g.y = SCREEN_HEIGHT - 40;

    g.animTimer += dt;
    g.shootTimer -= dt;

    g.meleeTimer -= dt;
    g.meleeAnimTimer -= dt;
    if (g.meleeAnimTimer <= 0.0f) g.meleeActive = false;

    // Melee attack when player is within range
    if (dist < GIANT2_MELEE_RANGE && g.meleeTimer <= 0.0f) {
        g.meleeTimer = GIANT2_MELEE_COOLDOWN;
        g.meleeAnimTimer = 0.35f;
        g.meleeActive = true;
    }
    // Keep meleeActive true for the first 0.15s of animation
    if (g.meleeAnimTimer <= 0.2f && g.meleeActive) g.meleeActive = false;

    if (g.shootTimer <= 0 && dist < 400.0f) {
        g.shootTimer = GIANT2_SHOOT_INTERVAL;
        for (int i = 0; i < GIANT2_PROJ_MAX; i++) {
            if (!g.projectiles[i].active) {
                g.projectiles[i].x = g.x;
                g.projectiles[i].y = g.y + 10;
                if (dist > 0) {
                    g.projectiles[i].speedX = (dx / dist) * GIANT2_PROJ_SPEED;
                    g.projectiles[i].speedY = (dy / dist) * GIANT2_PROJ_SPEED;
                } else {
                    g.projectiles[i].speedX = 0;
                    g.projectiles[i].speedY = GIANT2_PROJ_SPEED;
                }
                g.projectiles[i].active = true;
                break;
            }
        }
    }

    for (int i = 0; i < GIANT2_PROJ_MAX; i++) {
        if (g.projectiles[i].active) {
            g.projectiles[i].x += g.projectiles[i].speedX;
            g.projectiles[i].y += g.projectiles[i].speedY;
            if (g.projectiles[i].x < 0 || g.projectiles[i].x > SCREEN_WIDTH ||
                g.projectiles[i].y < 0 || g.projectiles[i].y > SCREEN_HEIGHT) {
                g.projectiles[i].active = false;
            }
        }
    }
}

void DrawGiant2(const Giant2& g, float playerX, float playerY) {
    if (g.hp <= 0) return;
    CreateGiant2Texture();

    Rectangle src = { 0, 0, 64, 64 };
    Rectangle dst = { g.x, g.y, 64, 64 };
    Vector2 origin = { 32, 32 };
    DrawTexturePro(giant2Tex, src, dst, origin, 0.0f, WHITE);

    // Sword swipe animation
    if (g.meleeAnimTimer > 0.0f) {
        float angle = atan2f(playerY - g.y, playerX - g.x) * RAD2DEG;
        float arcHalf = 45.0f;
        float alpha = (g.meleeAnimTimer / 0.35f) * 200.0f;
        if (alpha > 200.0f) alpha = 200.0f;
        Color swipeColor = { 255, 255, 220, (unsigned char)alpha };
        DrawRing((Vector2){ g.x, g.y }, 30.0f, GIANT2_MELEE_RANGE,
                 angle - arcHalf, angle + arcHalf, 16, swipeColor);
    }
}

void DrawLevel3Background() {
    DrawTileMap(lvl3Map);
}
