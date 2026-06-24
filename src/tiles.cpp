#include "tiles.h"
#include <cstdlib>
#include <cmath>

Texture2D tileTextures[TILE_COUNT];

// --- Palette constants ---
#define C(...) (Color){ __VA_ARGS__ }

static const Color COL_GRASS_SHADOW = C( 40, 80, 32, 255 );
static const Color COL_GRASS_DARK   = C( 50, 110, 42, 255 );
static const Color COL_GRASS_BASE   = C( 65, 135, 52, 255 );
static const Color COL_GRASS_MID    = C( 82, 155, 64, 255 );
static const Color COL_GRASS_LIGHT  = C( 100, 178, 78, 255 );

static const Color COL_GRASS2_BASE   = C( 78, 145, 58, 255 );
static const Color COL_GRASS2_DARK   = C( 58, 118, 44, 255 );
static const Color COL_GRASS2_MID    = C( 95, 168, 72, 255 );
static const Color COL_GRASS2_LIGHT  = C( 112, 190, 85, 255 );

static const Color COL_WATER_DARK    = C( 55, 105, 170, 255 );
static const Color COL_WATER_BASE    = C( 72, 132, 200, 255 );
static const Color COL_WATER_MID     = C( 96, 158, 222, 255 );
static const Color COL_WATER_LIGHT   = C( 130, 186, 240, 255 );
static const Color COL_WATER_FOAM    = C( 210, 238, 255, 255 );
static const Color COL_WATER_SHALLOW = C( 90, 150, 175, 255 );

static const Color COL_DIRT_DARK     = C( 118, 98, 68, 255 );
static const Color COL_DIRT_BASE     = C( 150, 128, 94, 255 );
static const Color COL_DIRT_LIGHT    = C( 178, 158, 124, 255 );
static const Color COL_DIRT_MID      = C( 164, 143, 108, 255 );

static const Color COL_SAND_BASE     = C( 210, 190, 150, 255 );
static const Color COL_SAND_DARK     = C( 180, 160, 120, 255 );
static const Color COL_SAND_LIGHT    = C( 230, 215, 180, 255 );
static const Color COL_SAND2_BASE    = C( 198, 178, 140, 255 );
static const Color COL_SAND2_DARK    = C( 168, 148, 112, 255 );
static const Color COL_FLOWER_RED    = C( 235, 70, 70, 255 );
static const Color COL_FLOWER_YLW    = C( 245, 215, 50, 255 );
static const Color COL_FLOWER_WHT    = C( 240, 240, 250, 255 );

static void ApplyNoise(Image* img, Color dark, int count) {
    for (int i = 0; i < count; i++)
        ImageDrawPixel(img, rand() % TILE_SIZE, rand() % TILE_SIZE, dark);
}

static void GrassBlades(Image* img, int ox, int oy, int count, Color mid, Color light, Color dark) {
    for (int t = 0; t < count; t++) {
        int bx = ox + rand() % (TILE_SIZE - ox * 2);
        int by = oy + rand() % (TILE_SIZE - oy * 2 - 8);
        ImageDrawPixel(img, bx, by, light);
        ImageDrawPixel(img, bx, by + 1, mid);
        ImageDrawPixel(img, bx - 1, by + 1, dark);
        ImageDrawPixel(img, bx + 1, by + 1, dark);
        ImageDrawPixel(img, bx, by - 1, mid);
    }
}

static void CelShadeEdge(Image* img, Color shadow) {
    for (int x = 0; x < TILE_SIZE; x++)
        ImageDrawPixel(img, x, TILE_SIZE - 1, shadow);
    for (int y = 0; y < TILE_SIZE; y++)
        ImageDrawPixel(img, TILE_SIZE - 1, y, shadow);
}

static void DrawFlower(Image* img, int x, int y, Color c) {
    ImageDrawPixel(img, x, y, c);
    ImageDrawPixel(img, x - 1, y, c);
    ImageDrawPixel(img, x + 1, y, c);
    ImageDrawPixel(img, x, y - 1, c);
    ImageDrawPixel(img, x, y + 1, c);
    ImageDrawPixel(img, x, y + 1, (Color){ 50, 120, 42, 255 });
}

static void WaveLines(Image* img, int yStart, int yEnd, Color mid, Color light, float phase) {
    for (int y = yStart; y < yEnd; y += 4) {
        int offset = (int)(sinf((float)(y - yStart) * 0.7f + phase) * 2.5f);
        int offset2 = (int)(sinf((float)(y - yStart) * 0.9f + phase + 2.0f) * 2.0f);
        for (int x = offset; x < TILE_SIZE + offset; x += 2) {
            if (x >= 0 && x < TILE_SIZE)
                ImageDrawPixel(img, x, y, mid);
        }
        for (int x = offset2; x < TILE_SIZE + offset2; x += 3) {
            if (x >= 0 && x < TILE_SIZE)
                ImageDrawPixel(img, x, y + 2, light);
        }
    }
}

// --- Tile generators ---

static Image GenGrassTile() {
    Image img = GenImageColor(TILE_SIZE, TILE_SIZE, COL_GRASS_BASE);
    ApplyNoise(&img, COL_GRASS_DARK, 35);
    for (int i = 0; i < 5; i++) {
        int bx = rand() % 28, by = rand() % 28;
        ImageDrawRectangle(&img, bx, by, 2, 2, COL_GRASS_MID);
    }
    for (int i = 0; i < 8; i++)
        ImageDrawPixel(&img, rand() % TILE_SIZE, rand() % TILE_SIZE, COL_GRASS_LIGHT);
    GrassBlades(&img, 2, 2, 6, COL_GRASS_MID, COL_GRASS_LIGHT, COL_GRASS_DARK);
    if (rand() % 100 < 25) {
        int fx = 4 + rand() % 24, fy = 4 + rand() % 24;
        Color fc = (rand() % 3 == 0) ? COL_FLOWER_YLW : (rand() % 2 ? COL_FLOWER_RED : COL_FLOWER_WHT);
        DrawFlower(&img, fx, fy, fc);
    }
    CelShadeEdge(&img, COL_GRASS_SHADOW);
    return img;
}

static Image GenGrassVarTile() {
    Image img = GenImageColor(TILE_SIZE, TILE_SIZE, COL_GRASS2_BASE);
    ApplyNoise(&img, COL_GRASS2_DARK, 30);
    for (int i = 0; i < 4; i++) {
        int bx = rand() % 28, by = rand() % 28;
        ImageDrawRectangle(&img, bx, by, 2, 2, COL_GRASS2_MID);
    }
    for (int i = 0; i < 6; i++)
        ImageDrawPixel(&img, rand() % TILE_SIZE, rand() % TILE_SIZE, COL_GRASS2_LIGHT);
    GrassBlades(&img, 2, 2, 5, COL_GRASS2_MID, COL_GRASS2_LIGHT, COL_GRASS2_DARK);
    if (rand() % 100 < 40) {
        int fx = 4 + rand() % 24, fy = 4 + rand() % 24;
        Color fc = (rand() % 3 == 0) ? COL_FLOWER_RED : (rand() % 2 ? COL_FLOWER_WHT : COL_FLOWER_YLW);
        DrawFlower(&img, fx, fy, fc);
    }
    CelShadeEdge(&img, COL_GRASS_SHADOW);
    return img;
}

static Image GenWaterTile() {
    Image img = GenImageColor(TILE_SIZE, TILE_SIZE, COL_WATER_BASE);
    ApplyNoise(&img, COL_WATER_DARK, 20);
    WaveLines(&img, 0, TILE_SIZE, COL_WATER_MID, COL_WATER_LIGHT, 1.2f);
    for (int i = 0; i < 8; i++) {
        int sx = rand() % 28 + 2, sy = rand() % 28 + 2;
        ImageDrawPixel(&img, sx, sy, COL_WATER_FOAM);
        if (rand() % 2) ImageDrawPixel(&img, sx + 1, sy, COL_WATER_FOAM);
    }
    CelShadeEdge(&img, COL_WATER_DARK);
    return img;
}

static Image GenDirtTile() {
    Image img = GenImageColor(TILE_SIZE, TILE_SIZE, COL_DIRT_BASE);
    ApplyNoise(&img, COL_DIRT_DARK, 40);
    for (int i = 0; i < 12; i++)
        ImageDrawPixel(&img, rand() % TILE_SIZE, rand() % TILE_SIZE, COL_DIRT_LIGHT);
    for (int t = 0; t < 3; t++) {
        int px = 4 + rand() % 24, py = 4 + rand() % 24;
        ImageDrawRectangle(&img, px, py, 2, 1, COL_DIRT_DARK);
        ImageDrawPixel(&img, px + rand() % 3 - 1, py + 1, COL_DIRT_MID);
    }
    CelShadeEdge(&img, COL_DIRT_DARK);
    return img;
}

static Image GenSandTile() {
    Image img = GenImageColor(TILE_SIZE, TILE_SIZE, COL_SAND_BASE);
    ApplyNoise(&img, COL_SAND_DARK, 55);
    for (int i = 0; i < 18; i++)
        ImageDrawPixel(&img, rand() % TILE_SIZE, rand() % TILE_SIZE, COL_SAND_LIGHT);
    for (int x = 3; x < TILE_SIZE; x += 6)
        for (int y = 3; y < TILE_SIZE; y += 6)
            if (rand() % 100 < 35)
                ImageDrawPixel(&img, x, y, COL_SAND_DARK);
    CelShadeEdge(&img, COL_SAND_DARK);
    return img;
}

static Image GenSandVarTile() {
    Image img = GenImageColor(TILE_SIZE, TILE_SIZE, COL_SAND2_BASE);
    ApplyNoise(&img, COL_SAND2_DARK, 45);
    for (int i = 0; i < 14; i++)
        ImageDrawPixel(&img, rand() % TILE_SIZE, rand() % TILE_SIZE, COL_SAND_LIGHT);
    for (int i = 0; i < 5; i++) {
        int px = 4 + rand() % 24, py = 4 + rand() % 24;
        ImageDrawRectangle(&img, px, py, 3, 2, COL_SAND2_DARK);
    }
    CelShadeEdge(&img, COL_SAND_DARK);
    return img;
}

static Image GenShoreTTile() {
    Image img = GenImageColor(TILE_SIZE, TILE_SIZE, COL_GRASS_BASE);
    ApplyNoise(&img, COL_GRASS_DARK, 15);
    for (int i = 0; i < 4; i++)
        ImageDrawPixel(&img, rand() % TILE_SIZE, rand() % 14, COL_GRASS_MID);
    GrassBlades(&img, 2, 2, 3, COL_GRASS_MID, COL_GRASS_LIGHT, COL_GRASS_DARK);
    if (rand() % 100 < 20) {
        DrawFlower(&img, 6 + rand() % 20, 3 + rand() % 8, COL_FLOWER_RED);
    }
    for (int x = 0; x < TILE_SIZE; x++) {
        ImageDrawPixel(&img, x, 14, COL_DIRT_DARK);
        ImageDrawPixel(&img, x, 15, COL_DIRT_BASE);
        ImageDrawPixel(&img, x, 16, COL_DIRT_LIGHT);
    }
    for (int y = 17; y <= 19; y++)
        for (int x = 0; x < TILE_SIZE; x++)
            ImageDrawPixel(&img, x, y, COL_WATER_SHALLOW);
    for (int y = 20; y < TILE_SIZE; y++)
        for (int x = 0; x < TILE_SIZE; x++)
            ImageDrawPixel(&img, x, y, COL_WATER_BASE);
    WaveLines(&img, 20, TILE_SIZE, COL_WATER_MID, COL_WATER_LIGHT, 0.5f);
    CelShadeEdge(&img, COL_GRASS_SHADOW);
    return img;
}

static Image GenShoreBTile() {
    Image img = GenImageColor(TILE_SIZE, TILE_SIZE, COL_WATER_BASE);
    for (int y = 0; y < 12; y++)
        for (int x = 0; x < TILE_SIZE; x++)
            ImageDrawPixel(&img, x, y, COL_WATER_BASE);
    WaveLines(&img, 0, 12, COL_WATER_MID, COL_WATER_LIGHT, 2.0f);
    for (int y = 12; y <= 14; y++)
        for (int x = 0; x < TILE_SIZE; x++)
            ImageDrawPixel(&img, x, y, COL_WATER_SHALLOW);
    for (int x = 0; x < TILE_SIZE; x++) {
        ImageDrawPixel(&img, x, 15, COL_DIRT_LIGHT);
        ImageDrawPixel(&img, x, 16, COL_DIRT_BASE);
        ImageDrawPixel(&img, x, 17, COL_DIRT_DARK);
    }
    for (int y = 18; y < TILE_SIZE; y++) {
        for (int x = 0; x < TILE_SIZE; x++)
            ImageDrawPixel(&img, x, y, COL_GRASS_BASE);
    }
    ApplyNoise(&img, COL_GRASS_DARK, 12);
    for (int i = 0; i < 3; i++)
        ImageDrawPixel(&img, rand() % TILE_SIZE, 18 + rand() % 12, COL_GRASS_MID);
    GrassBlades(&img, 2, 18, 3, COL_GRASS_MID, COL_GRASS_LIGHT, COL_GRASS_DARK);
    CelShadeEdge(&img, COL_GRASS_SHADOW);
    return img;
}

static Image GenShoreLTile() {
    Image img = GenImageColor(TILE_SIZE, TILE_SIZE, COL_GRASS_BASE);
    ApplyNoise(&img, COL_GRASS_DARK, 12);
    GrassBlades(&img, 2, 2, 2, COL_GRASS_MID, COL_GRASS_LIGHT, COL_GRASS_DARK);
    for (int y = 0; y < TILE_SIZE; y++) {
        ImageDrawPixel(&img, 8, y, COL_DIRT_DARK);
        ImageDrawPixel(&img, 9, y, COL_DIRT_BASE);
        ImageDrawPixel(&img, 10, y, COL_DIRT_LIGHT);
    }
    for (int y = 0; y < TILE_SIZE; y++)
        for (int x = 11; x < TILE_SIZE; x++)
            ImageDrawPixel(&img, x, y, COL_WATER_BASE);
    // Blend water area
    for (int y = 0; y < TILE_SIZE; y++) {
        ImageDrawPixel(&img, 11, y, COL_WATER_SHALLOW);
        ImageDrawPixel(&img, 12, y, COL_WATER_SHALLOW);
    }
    WaveLines(&img, 0, TILE_SIZE, COL_WATER_MID, COL_WATER_LIGHT, 3.0f);
    CelShadeEdge(&img, COL_GRASS_SHADOW);
    return img;
}

static Image GenShoreRTile() {
    Image img = GenImageColor(TILE_SIZE, TILE_SIZE, COL_WATER_BASE);
    WaveLines(&img, 0, TILE_SIZE, COL_WATER_MID, COL_WATER_LIGHT, 4.0f);
    for (int y = 0; y < TILE_SIZE; y++)
        for (int x = 0; x < 21; x++)
            ImageDrawPixel(&img, x, y, COL_WATER_BASE);
    for (int y = 0; y < TILE_SIZE; y++) {
        ImageDrawPixel(&img, 20, y, COL_WATER_SHALLOW);
        ImageDrawPixel(&img, 21, y, COL_WATER_SHALLOW);
    }
    for (int y = 0; y < TILE_SIZE; y++) {
        ImageDrawPixel(&img, 22, y, COL_DIRT_LIGHT);
        ImageDrawPixel(&img, 23, y, COL_DIRT_BASE);
    }
    for (int y = 0; y < TILE_SIZE; y++)
        for (int x = 24; x < TILE_SIZE; x++)
            ImageDrawPixel(&img, x, y, COL_GRASS_BASE);
    ApplyNoise(&img, COL_GRASS_DARK, 6);
    GrassBlades(&img, 24, 2, 2, COL_GRASS_MID, COL_GRASS_LIGHT, COL_GRASS_DARK);
    CelShadeEdge(&img, COL_GRASS_SHADOW);
    return img;
}

void CreateTileTextures() {
    Image imgs[TILE_COUNT];
    imgs[TILE_GRASS] = GenGrassTile();
    imgs[TILE_GRASS_VAR] = GenGrassVarTile();
    imgs[TILE_WATER] = GenWaterTile();
    imgs[TILE_DIRT] = GenDirtTile();
    imgs[TILE_SAND] = GenSandTile();
    imgs[TILE_SAND_VAR] = GenSandVarTile();
    imgs[TILE_SHORE_T] = GenShoreTTile();
    imgs[TILE_SHORE_B] = GenShoreBTile();
    imgs[TILE_SHORE_L] = GenShoreLTile();
    imgs[TILE_SHORE_R] = GenShoreRTile();
    for (int i = 0; i < TILE_COUNT; i++) {
        tileTextures[i] = LoadTextureFromImage(imgs[i]);
        UnloadImage(imgs[i]);
    }
}

void UnloadTileTextures() {
    for (int i = 0; i < TILE_COUNT; i++)
        UnloadTexture(tileTextures[i]);
}

void DrawTileMap(const int map[MAP_H][MAP_W]) {
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            int tile = map[y][x];
            if (tile >= 0 && tile < TILE_COUNT) {
                DrawTexture(tileTextures[tile], x * TILE_SIZE, y * TILE_SIZE, WHITE);
            }
        }
    }
}
