#include "player.h"
#include "game.h"
#include <cmath>

// ---- Palette ----
#define C(...) (Color){ __VA_ARGS__ }
static const Color SKIN      = C( 235, 195, 155, 255 );
static const Color SKIN_SHADE= C( 200, 165, 130, 255 );
static const Color HAIR      = C( 85, 45, 15, 255 );
static const Color HAIR_LIGHT= C( 110, 60, 22, 255 );
static const Color HAIR_DARK = C( 60, 30, 10, 255 );
static const Color TUNIC     = C( 60, 60, 180, 255 );
static const Color TUNIC_DARK= C( 42, 42, 150, 255 );
static const Color TUNIC_LT  = C( 75, 75, 200, 255 );
static const Color TUNIC_GREEN  = C( 60, 140, 60, 255 );
static const Color TUNIC_GREEN_DARK = C( 42, 110, 42, 255 );
static const Color TUNIC_GREEN_LT = C( 75, 165, 75, 255 );
static const Color BELT      = C( 130, 90, 50, 255 );
static const Color BUCKLE    = C( 210, 170, 40, 255 );
static const Color PANTS     = C( 100, 62, 40, 255 );
static const Color PANTS_DK  = C( 75, 46, 30, 255 );
static const Color SHOES     = C( 90, 60, 40, 255 );
static const Color SHOES_DK  = C( 55, 35, 22, 255 );
static const Color OUTLINE   = C( 30, 22, 16, 255 );
static const Color MOUTH     = C( 190, 105, 80, 255 );

static void FillCircleOutline(Image* img, int cx, int cy, int r, Color fill, Color outline) {
    ImageDrawCircle(img, cx, cy, r, outline);
    if (r > 1) ImageDrawCircle(img, cx, cy, r - 1, fill);
}

Texture2D CreateDavidTexture() {
    const int w = DAVID_FRAME_W * DAVID_SHEET_FRAMES;
    const int h = DAVID_FRAME_H;
    Image img = GenImageColor(w, h, BLANK);

    auto px = [&](int x, int y, Color c) {
        if (x >= 0 && x < w && y >= 0 && y < h)
            ImageDrawPixel(&img, x, y, c);
    };

    auto drawBody = [&](int ox, bool walk, bool faceLeft, bool faceRight, bool faceUp) {
        int cx = ox + DAVID_FRAME_W / 2;
        int legS = walk ? 3 : 0;

        // Shadow
        for (int sx = ox + 8; sx < ox + 24; sx++)
            for (int sy = 38; sy < 40; sy++)
                px(sx, sy, (Color){ 20, 20, 30, 40 });

        // ---- LEGS ----
        // Left leg
        for (int y = 28; y < 36; y++)
            for (int x = ox + 10 + legS; x < ox + 16 + legS; x++)
                px(x, y, PANTS);
        // Right leg
        for (int y = 28; y < 36; y++)
            for (int x = ox + 17 - legS; x < ox + 23 - legS; x++)
                px(x, y, PANTS);
        // Leg shading
        for (int x = ox + 10 + legS; x < ox + 12 + legS; x++)
            for (int y = 28; y < 36; y++)
                px(x, y, PANTS_DK);

        // ---- SHOES ----
        for (int y = 35; y < 39; y++) {
            px(ox + 9 + legS, y, SHOES);
            px(ox + 10 + legS, y, SHOES);
            px(ox + 14 + legS, y, SHOES);
            px(ox + 15 + legS, y, SHOES);
            px(ox + 16 - legS, y, SHOES);
            px(ox + 17 - legS, y, SHOES);
            px(ox + 21 - legS, y, SHOES);
            px(ox + 22 - legS, y, SHOES);
        }
        // Shoe sole
        px(ox + 8 + legS, 38, SHOES_DK);
        px(ox + 11 + legS, 38, SHOES_DK);
        px(ox + 13 + legS, 38, SHOES_DK);
        px(ox + 16 - legS, 38, SHOES_DK);
        px(ox + 19 - legS, 38, SHOES_DK);
        px(ox + 22 - legS, 38, SHOES_DK);

        // ---- TUNIC (BODY) ----
        int tLeft = ox + 7, tRight = ox + 25;
        for (int y = 12; y < 29; y++)
            for (int x = tLeft; x < tRight; x++)
                px(x, y, TUNIC);
        // Tunic left shading
        for (int y = 12; y < 29; y++)
            for (int x = tLeft; x < tLeft + 4; x++)
                px(x, y, TUNIC_DARK);
        // Tunic right shading
        for (int y = 12; y < 29; y++)
            for (int x = tRight - 4; x < tRight; x++)
                px(x, y, TUNIC_DARK);
        // Tunic folds (vertical lines)
        for (int y = 14; y < 27; y++) {
            px(ox + 13, y, TUNIC_DARK);
            px(ox + 20, y, TUNIC_DARK);
        }
        // Collar (V-neck)
        px(ox + 14, 12, TUNIC_DARK);
        px(ox + 15, 13, TUNIC_DARK);
        px(ox + 16, 14, TUNIC_DARK);
        px(ox + 17, 14, TUNIC_DARK);
        px(ox + 18, 13, TUNIC_DARK);
        px(ox + 19, 12, TUNIC_DARK);

        // ---- BELT ----
        for (int x = tLeft + 1; x < tRight - 1; x++)
            px(x, 26, BELT);
        // Buckle
        px(ox + 14, 25, BUCKLE);
        px(ox + 15, 25, BUCKLE);
        px(ox + 16, 25, BUCKLE);
        px(ox + 14, 26, BUCKLE);
        px(ox + 15, 26, BUCKLE);
        px(ox + 16, 26, BUCKLE);
        px(ox + 14, 27, BUCKLE);
        px(ox + 15, 27, BUCKLE);
        px(ox + 16, 27, BUCKLE);

        // ---- ARMS ----
        // Left arm
        for (int y = 14; y < 24; y++) {
            px(ox + 4, y, SKIN);
            px(ox + 5, y, SKIN);
            px(ox + 6, y, SKIN);
        }
        // Left arm shading
        for (int y = 14; y < 24; y++)
            px(ox + 4, y, SKIN_SHADE);
        // Right arm
        for (int y = 14; y < 24; y++) {
            px(ox + 26, y, SKIN);
            px(ox + 27, y, SKIN);
            px(ox + 28, y, SKIN);
        }
        for (int y = 14; y < 24; y++)
            px(ox + 28, y, SKIN_SHADE);

        // ---- HEAD ----
        // Neck
        for (int y = 8; y < 13; y++)
            for (int x = cx - 3; x < cx + 4; x++)
                px(x, y, SKIN);
        for (int x = cx - 3; x < cx + 4; x++)
            px(x, 12, TUNIC_DARK); // neck shadow

        if (faceUp) {
            // Back of head - full hair
            FillCircleOutline(&img, cx, 6, 10, HAIR, OUTLINE);
            for (int y = 0; y < 12; y++)
                for (int x = cx - 8; x <= cx + 8; x++)
                    if (x >= 0 && x < w) px(x, y, HAIR);
        } else {
            // Face
            FillCircleOutline(&img, cx, 7, 10, SKIN, OUTLINE);
            // Hair top
            for (int y = 0; y < 6; y++)
                for (int x = cx - 8 + y/2; x <= cx + 8 - y/2; x++)
                    px(x, y, HAIR);
            // Hair fringe (front sweep)
            for (int x = cx - 7; x <= cx + 7; x++) {
                px(x, 5, HAIR_LIGHT);
                if (x >= cx - 5 && x <= cx + 3) px(x, 6, HAIR_LIGHT);
            }
            // Hair side strands
            for (int y = 6; y < 12; y++) {
                px(cx - 9, y, HAIR);
                px(cx + 9, y, HAIR);
            }
            // Hair highlight
            for (int x = cx - 6; x <= cx - 2; x++)
                px(x, 2, HAIR_LIGHT);
            for (int x = cx - 4; x <= cx; x++)
                px(x, 1, HAIR_LIGHT);

            if (!faceLeft && !faceRight) {
                // ---- FRONT FACE ----
                // Eyes (left)
                px(cx - 4, 7, WHITE);
                px(cx - 3, 7, WHITE);
                px(cx - 4, 8, WHITE);
                px(cx - 3, 8, WHITE);
                px(cx - 3, 8, (Color){ 35, 25, 15, 255 }); // pupil
                px(cx - 4, 7, WHITE); // eye sparkle
                // Eyes (right)
                px(cx + 2, 7, WHITE);
                px(cx + 3, 7, WHITE);
                px(cx + 2, 8, WHITE);
                px(cx + 3, 8, WHITE);
                px(cx + 3, 8, (Color){ 35, 25, 15, 255 }); // pupil
                px(cx + 2, 7, WHITE); // eye sparkle
                // Eyebrows
                for (int x = cx - 5; x <= cx - 2; x++) px(x, 5, HAIR_DARK);
                for (int x = cx + 2; x <= cx + 5; x++) px(x, 5, HAIR_DARK);
                // Mouth
                px(cx - 1, 12, MOUTH);
                px(cx, 12, MOUTH);
                px(cx + 1, 12, MOUTH);
                // Cheek blush
                px(cx - 6, 9, (Color){ 230, 160, 150, 120 });
                px(cx - 6, 10, (Color){ 230, 160, 150, 120 });
                px(cx + 6, 9, (Color){ 230, 160, 150, 120 });
                px(cx + 6, 10, (Color){ 230, 160, 150, 120 });
            } else if (faceLeft) {
                // ---- LEFT PROFILE ----
                px(cx - 4, 7, WHITE);
                px(cx - 3, 7, WHITE);
                px(cx - 4, 8, WHITE);
                px(cx - 3, 8, WHITE);
                px(cx - 3, 8, (Color){ 35, 25, 15, 255 });
                for (int x = cx - 5; x <= cx - 2; x++) px(x, 5, HAIR_DARK);
            } else if (faceRight) {
                // ---- RIGHT PROFILE ----
                px(cx + 2, 7, WHITE);
                px(cx + 3, 7, WHITE);
                px(cx + 2, 8, WHITE);
                px(cx + 3, 8, WHITE);
                px(cx + 3, 8, (Color){ 35, 25, 15, 255 });
                for (int x = cx + 2; x <= cx + 5; x++) px(x, 5, HAIR_DARK);
            }
        }

        // ---- SLING (diagonal across body) ----
        px(ox + 6, 12, BELT);
        px(ox + 5, 13, BELT);
        px(ox + 4, 14, BELT);
        px(ox + 3, 15, BELT);

        // ---- FULL BODY OUTLINE ----
        // Head outline is done via FillCircleOutline
        // Tunic sides outline
        px(tLeft, 14, OUTLINE); px(tLeft, 15, OUTLINE); px(tLeft, 16, OUTLINE);
        px(tRight - 1, 14, OUTLINE); px(tRight - 1, 15, OUTLINE); px(tRight - 1, 16, OUTLINE);
        // Tunic bottom outline
        for (int x = tLeft; x < tRight; x++)
            px(x, 28, OUTLINE);
        // Belt outline
        px(tLeft + 1, 27, OUTLINE);
        px(tRight - 2, 27, OUTLINE);
        // Leg outline
        px(ox + 9 + legS, 30, OUTLINE); px(ox + 9 + legS, 31, OUTLINE);
        px(ox + 16 - legS, 30, OUTLINE); px(ox + 16 - legS, 31, OUTLINE);
        // Shoe outline
        px(ox + 7 + legS, 37, OUTLINE); px(ox + 8 + legS, 38, OUTLINE);
        px(ox + 15 + legS, 37, OUTLINE); px(ox + 14 + legS, 38, OUTLINE);
        px(ox + 17 - legS, 37, OUTLINE); px(ox + 16 - legS, 38, OUTLINE);
        px(ox + 23 - legS, 37, OUTLINE); px(ox + 22 - legS, 38, OUTLINE);
    };

    drawBody(0, false, false, false, false);   // 0: DOWN idle
    drawBody(32, true, false, false, false);   // 1: DOWN walk
    drawBody(64, false, true, false, false);   // 2: LEFT idle
    drawBody(96, true, true, false, false);    // 3: LEFT walk
    drawBody(128, false, false, true, false);  // 4: RIGHT idle
    drawBody(160, true, false, true, false);   // 5: RIGHT walk
    drawBody(192, false, false, false, true);  // 6: UP idle
    drawBody(224, true, false, false, true);   // 7: UP walk

    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

Texture2D CreateJonathanTexture() {
    Texture2D davidTex = CreateDavidTexture();
    Image img = LoadImageFromTexture(davidTex);
    UnloadTexture(davidTex);

    ImageColorReplace(&img, TUNIC, TUNIC_GREEN);
    ImageColorReplace(&img, TUNIC_DARK, TUNIC_GREEN_DARK);
    ImageColorReplace(&img, TUNIC_LT, TUNIC_GREEN_LT);

    for (int f = 0; f < 8; f++) {
        int ox = f * DAVID_FRAME_W;
        ImageDrawPixel(&img, ox + 6, 12, TUNIC_GREEN);
        ImageDrawPixel(&img, ox + 5, 13, TUNIC_GREEN);
        ImageDrawPixel(&img, ox + 4, 14, TUNIC_GREEN);
        ImageDrawPixel(&img, ox + 3, 15, TUNIC_GREEN);
    }

    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

Player CreatePlayer() {
    Player p;
    p.x = 100.0f;
    p.y = 300.0f;
    p.hp = PLAYER_MAX_HP;
    p.maxHp = PLAYER_MAX_HP;
    p.ammo = 0;
    p.sheet = CreateDavidTexture();
    p.currentFrame = 0;
    p.dir = DIR_DOWN;
    p.animTimer = 0.0f;
    p.shootCooldown = 0.0f;
    p.invulnTimer = 0.0f;
    return p;
}

void UpdatePlayer(Player& p) {
    float dt = GetFrameTime();

    bool moving = false;
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) { p.y -= PLAYER_SPEED; p.dir = DIR_UP; moving = true; }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) { p.y += PLAYER_SPEED; p.dir = DIR_DOWN; moving = true; }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) { p.x -= PLAYER_SPEED; p.dir = DIR_LEFT; moving = true; }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) { p.x += PLAYER_SPEED; p.dir = DIR_RIGHT; moving = true; }

    if (p.x < 20) p.x = 20;
    if (p.x > SCREEN_WIDTH - 20) p.x = SCREEN_WIDTH - 20;
    if (p.y < 20) p.y = 20;
    if (p.y > SCREEN_HEIGHT - 20) p.y = SCREEN_HEIGHT - 20;

    if (moving) {
        p.animTimer += dt;
        if (p.animTimer > 0.15f) {
            p.currentFrame = (p.currentFrame + 1) % 2;
            p.animTimer = 0.0f;
        }
    } else {
        p.currentFrame = 0;
        p.animTimer = 0.0f;
    }

    if (p.invulnTimer > 0) p.invulnTimer -= dt;
    if (p.shootCooldown > 0) p.shootCooldown -= dt;
}

void DrawPlayer(const Player& p) {
    if (p.invulnTimer > 0 && std::fmod(p.invulnTimer * 10.0f, 2.0f) < 1.0f) return;

    int frameIndex = (int)p.dir * 2 + p.currentFrame;
    Rectangle src = {
        (float)(frameIndex * DAVID_FRAME_W), 0.0f,
        (float)DAVID_FRAME_W, (float)DAVID_FRAME_H
    };
    Rectangle dst = { p.x, p.y, (float)DAVID_FRAME_W, (float)DAVID_FRAME_H };
    Vector2 origin = { (float)DAVID_FRAME_W / 2, (float)DAVID_FRAME_H / 2 };
    DrawTexturePro(p.sheet, src, dst, origin, 0.0f, WHITE);
}

void UnloadPlayer(Player& p) {
    UnloadTexture(p.sheet);
}
