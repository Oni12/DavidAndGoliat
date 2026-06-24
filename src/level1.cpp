#include "level1.h"
#include "game.h"
#include "utils.h"
#include <cstdlib>
#include <cmath>

// River cols 6-18, with path crossing at 11-13
// SHORE_T row6, SHORE_B row9, WATER rows 7-8
const int lvl1Map[MAP_H][MAP_W] = {
    //0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4
    { 0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0 }, // 0
    { 0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0 }, // 1
    { 0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0 }, // 2
    { 1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0 }, // 3
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, // 4
    { 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0 }, // 5
    { 0,0,0,0,0,0,6,6,6,6,6,3,3,3,6,6,6,6,6,0,0,0,0,0,0 }, // 6  SHORE_B (6) + DIRT (3)
    { 0,0,1,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,1 }, // 7  WATER (2)
    { 0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0 }, // 8  WATER
    { 0,0,0,0,0,0,7,7,7,7,7,3,3,3,7,7,7,7,7,0,0,0,1,0,0 }, // 9  SHORE_T (7) + DIRT (3)
    { 0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, // 10
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 }, // 11
    { 0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0 }, // 12
    { 0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, // 13
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 }, // 14
    { 0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 }, // 15
    { 0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0 }, // 16
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, // 17
    { 1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0 }, // 18
};

// --- Stones ---

void InitStones(Stone* stones, int count, const Bush* bushes, int bushCount) {
    for (int i = 0; i < count; i++) {
        bool valid;
        do {
            stones[i].x = (float)(rand() % (SCREEN_WIDTH - 120) + 60);
            stones[i].y = (float)(rand() % (SCREEN_HEIGHT - 120) + 60);
            valid = true;
            for (int j = 0; j < bushCount; j++) {
                float dx = stones[i].x - bushes[j].x;
                float dy = stones[i].y - bushes[j].y;
                if (dx * dx + dy * dy < 50.0f * 50.0f) { valid = false; break; }
            }
        } while (!valid);
        stones[i].collected = false;
    }
}

void DrawStones(const Stone* stones, int count) {
    for (int i = 0; i < count; i++) {
        if (!stones[i].collected) {
            int sx = (int)stones[i].x;
            int sy = (int)stones[i].y;
            DrawCircle(sx + 2, sy + 2, 6, (Color){ 20, 20, 30, 60 });
            utils::DrawCircleMidpointFill(sx - 1, sy, 5, (Color){ 90, 90, 95, 255 });
            utils::DrawCircleMidpointFill(sx + 2, sy - 1, 4, (Color){ 120, 120, 125, 255 });
            utils::DrawCircleMidpointFill(sx, sy + 1, 5, (Color){ 105, 105, 110, 255 });
            utils::DrawCircleMidpoint(sx - 1, sy, 5, (Color){ 50, 50, 55, 255 });
            utils::DrawCircleMidpoint(sx + 2, sy - 1, 4, (Color){ 50, 50, 55, 255 });
            utils::DrawCircleMidpoint(sx, sy + 1, 5, (Color){ 50, 50, 55, 255 });
            utils::DrawCircleMidpointFill(sx - 2, sy - 3, 2, (Color){ 200, 200, 205, 255 });
            DrawPixel(sx - 3, sy - 4, WHITE);
        }
    }
}

// --- Bushes ---

void InitBushes(Bush* bushes, int count) {
    for (int i = 0; i < count; i++) {
        bushes[i].x = (float)(rand() % (SCREEN_WIDTH - 100) + 50);
        bushes[i].y = (float)(rand() % (SCREEN_HEIGHT - 150) + 80);
    }
}

static void DrawBushTree(int cx, int cy) {
    DrawCircle(cx + 3, cy + 3, 14, (Color){ 20, 20, 30, 60 });
    // Dark green base
    utils::DrawCircleMidpointFill(cx - 2, cy + 1, 10, (Color){ 35, 85, 25, 255 });
    utils::DrawCircleMidpointFill(cx + 3, cy - 1, 9, (Color){ 45, 100, 30, 255 });
    utils::DrawCircleMidpointFill(cx + 1, cy + 3, 8, (Color){ 40, 95, 28, 255 });
    utils::DrawCircleMidpointFill(cx - 3, cy - 2, 7, (Color){ 50, 110, 35, 255 });
    // Mid green
    utils::DrawCircleMidpointFill(cx - 1, cy, 7, (Color){ 60, 130, 50, 255 });
    utils::DrawCircleMidpointFill(cx + 2, cy, 6, (Color){ 70, 145, 55, 255 });
    // Highlight
    utils::DrawCircleMidpointFill(cx - 4, cy - 4, 4, (Color){ 100, 200, 80, 255 });
    utils::DrawCircleMidpointFill(cx - 5, cy - 5, 2, (Color){ 130, 230, 100, 255 });
    // Outline
    Color outline = (Color){ 20, 55, 15, 255 };
    utils::DrawCircleMidpoint(cx - 2, cy + 1, 10, outline);
    utils::DrawCircleMidpoint(cx + 3, cy - 1, 9, outline);
    utils::DrawCircleMidpoint(cx + 1, cy + 3, 8, outline);
    utils::DrawCircleMidpoint(cx - 3, cy - 2, 7, outline);
    // Small berries (red dots)
    DrawPixel(cx - 1, cy - 5, (Color){ 220, 50, 50, 255 });
    DrawPixel(cx + 3, cy - 3, (Color){ 220, 50, 50, 255 });
    DrawPixel(cx - 4, cy + 1, (Color){ 220, 50, 50, 255 });
    DrawPixel(cx + 5, cy + 1, (Color){ 220, 50, 50, 255 });
    DrawPixel(cx, cy + 4, (Color){ 220, 50, 50, 255 });
}

void DrawBushes(const Bush* bushes, int count) {
    for (int i = 0; i < count; i++)
        DrawBushTree((int)bushes[i].x, (int)bushes[i].y);
}

void DrawLevel1Background() {
    DrawTileMap(lvl1Map);
}
