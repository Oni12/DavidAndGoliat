#include "utils.h"
#include <cmath>

namespace utils {
    static void DrawCirclePoints(int xc, int yc, int x, int y, Color color) {
        DrawPixel(xc + x, yc + y, color);
        DrawPixel(xc - x, yc + y, color);
        DrawPixel(xc + x, yc - y, color);
        DrawPixel(xc - x, yc - y, color);
        DrawPixel(xc + y, yc + x, color);
        DrawPixel(xc - y, yc + x, color);
        DrawPixel(xc + y, yc - x, color);
        DrawPixel(xc - y, yc - x, color);
    }

    void DrawLineDDA(float x1, float y1, float x2, float y2, Color color) {
        float dx = x2 - x1;
        float dy = y2 - y1;
        float steps = fmaxf(fabsf(dx), fabsf(dy));
        if (steps < 1.0f) steps = 1.0f;
        float xInc = dx / steps;
        float yInc = dy / steps;
        float x = x1;
        float y = y1;
        for (int i = 0; i <= (int)steps; i++) {
            DrawPixel((int)roundf(x), (int)roundf(y), color);
            x += xInc;
            y += yInc;
        }
    }

    void DrawCircleMidpoint(int xc, int yc, int r, Color color) {
        if (r <= 0) { DrawPixel(xc, yc, color); return; }
        int x = 0;
        int y = r;
        int d = 1 - r;
        DrawCirclePoints(xc, yc, x, y, color);
        while (x < y) {
            x++;
            if (d < 0) d += 2 * x + 1;
            else { y--; d += 2 * (x - y) + 1; }
            DrawCirclePoints(xc, yc, x, y, color);
        }
    }

    void DrawCircleMidpointFill(int xc, int yc, int r, Color color) {
        for (int y = -r; y <= r; y++) {
            int x = (int)sqrtf((float)(r * r - y * y));
            for (int i = -x; i <= x; i++)
                DrawPixel(xc + i, yc + y, color);
        }
    }
}
