#pragma once
#include "raylib.h"

namespace utils {
    void DrawLineDDA(float x1, float y1, float x2, float y2, Color color);
    void DrawCircleMidpoint(int xc, int yc, int r, Color color);
    void DrawCircleMidpointFill(int xc, int yc, int r, Color color);
}
