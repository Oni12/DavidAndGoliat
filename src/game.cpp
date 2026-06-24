#include "game.h"
#include <cmath>
#include <cstdio>

// ---- Palette ----
#define C(...) (Color){ __VA_ARGS__ }
static const Color COL_OUTER  = C( 67, 54, 49, 255 );  // #433631
static const Color COL_GOLD   = C( 214, 181, 95, 255 ); // #D6B55F
static const Color COL_BG     = C( 255, 247, 231, 255 ); // #FFF7E7
static const Color COL_TEXT   = C( 50, 40, 36, 255 );   // #322824
static const Color COL_NAME   = C( 100, 80, 70, 255 );

static Texture2D mapTerrainTex = { 0 };
static bool mapTerrainLoaded = false;

static void DrawDialogBox() {
    int dy = DIALOG_Y, dh = DIALOG_H, dw = SCREEN_WIDTH;
    // Outer thick border (3px) with rounded corners
    DrawRectangle(2, dy, dw - 4, 3, COL_OUTER);
    DrawRectangle(2, dy + dh - 3, dw - 4, 3, COL_OUTER);
    DrawRectangle(0, dy + 3, 3, dh - 6, COL_OUTER);
    DrawRectangle(dw - 3, dy + 3, 3, dh - 6, COL_OUTER);
    DrawPixel(2, dy + 2, COL_OUTER);
    DrawPixel(dw - 3, dy + 2, COL_OUTER);
    DrawPixel(2, dy + dh - 3, COL_OUTER);
    DrawPixel(dw - 3, dy + dh - 3, COL_OUTER);
    // Inner gold border (1px)
    DrawRectangle(3, dy + 3, dw - 6, 1, COL_GOLD);
    DrawRectangle(3, dy + dh - 4, dw - 6, 1, COL_GOLD);
    DrawRectangle(3, dy + 4, 1, dh - 8, COL_GOLD);
    DrawRectangle(dw - 4, dy + 4, 1, dh - 8, COL_GOLD);
    // Background
    DrawRectangle(4, dy + 4, dw - 8, dh - 8, COL_BG);
}

static void DrawContinueArrow() {
    if ((int)(GetTime() * 3) % 2 == 0) {
        DrawText("▼", SCREEN_WIDTH - 22, DIALOG_Y + DIALOG_H - 22, 12, COL_TEXT);
    }
}

void DrawHealthBar(int x, int y, int hp, int maxHp) {
    DrawRectangle(x, y, 104, 14, (Color){ 30, 30, 40, 255 });
    DrawRectangle(x + 1, y + 1, 102, 12, (Color){ 60, 50, 50, 255 });
    Color barColor = (hp > 2) ? (Color){ 50, 200, 50, 255 } : (hp > 1) ? (Color){ 230, 160, 30, 255 } : (Color){ 220, 40, 40, 255 };
    if (hp > 0)
        DrawRectangle(x + 2, y + 2, (int)(100.0f * hp / maxHp), 10, barColor);
    DrawRectangleLines(x, y, 104, 14, (Color){ 180, 170, 150, 255 });
    char hpText[8];
    snprintf(hpText, sizeof(hpText), "%d/%d", hp, maxHp);
    DrawText(hpText, x + 52 - MeasureText(hpText, 8) / 2, y + 3, 8, WHITE);
}

void DrawHUDLevel1(int collected, int hp, int maxHp, int score) {
    DrawRectangle(0, 0, SCREEN_WIDTH, 30, (Color){ 20, 22, 35, 220 });
    DrawRectangle(0, 28, SCREEN_WIDTH, 2, COL_GOLD);
    DrawText(TextFormat("Piedras: %d/%d", collected, STONES_NEEDED), 10, 7, 12, WHITE);
    DrawText("Nivel 1", SCREEN_WIDTH / 2 - 30, 7, 12, COL_GOLD);
    DrawHealthBar(SCREEN_WIDTH / 2 + 60, 7, hp, maxHp);
    DrawText(TextFormat("Puntaje: %d", score), SCREEN_WIDTH - 140, 7, 12, WHITE);
}

void DrawHUDLevel2(int ammo, int hp, int maxHp, int score) {
    DrawRectangle(0, 0, SCREEN_WIDTH, 30, (Color){ 20, 22, 35, 220 });
    DrawRectangle(0, 28, SCREEN_WIDTH, 2, COL_GOLD);
    DrawText(TextFormat("Piedras: %d/%d", ammo, AMMO_MAX), 10, 7, 12, WHITE);
    DrawText("Nivel 2 - Derrota a Goliat!", SCREEN_WIDTH / 2 - 130, 7, 12, COL_GOLD);
    DrawHealthBar(SCREEN_WIDTH / 2 + 100, 7, hp, maxHp);
    DrawText(TextFormat("Puntaje: %d", score), SCREEN_WIDTH - 140, 7, 12, WHITE);
}

void DrawDialogo(int page, Texture2D* portrait) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 20, 22, 45, 255 });
    if (portrait != NULL && portrait->id > 0) {
        int px = SCREEN_WIDTH - PORTRAIT_W - 14;
        int py = DIALOG_Y - 70;
        DrawTexturePro(*portrait,
            (Rectangle){ 0, 0, (float)portrait->width, (float)portrait->height },
            (Rectangle){ (float)px, (float)py, (float)PORTRAIT_W, (float)PORTRAIT_H },
            (Vector2){ 0, 0 }, 0, WHITE);
    }
    DrawDialogBox();
    DrawText("La Historia de David y Goliat", 14, DIALOG_Y + 12, 28, COL_GOLD);
    const char* textos[4];
    textos[0] = "David era un joven pastor de Belen, el menor de los\nhijos de Isai. Mientras cuidaba las ovejas de su\npadre, Dios lo preparaba para una mision especial.";
    textos[1] = "Los filisteos, enemigos de Israel, desafiaban al\nejercito del rey Saul. Su campeon, Goliat, un\ngigante de casi 3 metros de altura, se burlaba\nde los israelitas dia tras dia.";
    textos[2] = "David confio en Dios y tomo 5 piedras lisas del\narroyo. Con su honda y una fe inquebrantable,\nsalio al encuentro del gigante que aterrorizaba\na su pueblo.";
    textos[3] = "Ayuda a David a recolectar las 5 piedras y a\nderrotar a Goliat. Demuestra que con fe y\nvalentia, hasta el gigante mas grande puede caer.";
    DrawText(textos[page], 14, DIALOG_Y + 48, 26, COL_TEXT);
    DrawText(TextFormat("Pagina %d/4", page + 1), 14, DIALOG_Y + DIALOG_H - 44, 16, (Color){ 140, 130, 120, 255 });
    DrawText("Presiona ENTER para continuar", 14, DIALOG_Y + DIALOG_H - 24, 16, COL_GOLD);
    DrawContinueArrow();
}

void DrawNivel1Completo(int score) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 20, 22, 45, 230 });
    DrawRectangle(SCREEN_WIDTH / 2 - 200, 140, 400, 250, COL_OUTER);
    DrawRectangle(SCREEN_WIDTH / 2 - 198, 142, 396, 246, COL_GOLD);
    DrawRectangle(SCREEN_WIDTH / 2 - 196, 144, 392, 242, COL_BG);
    DrawText("NIVEL 1 COMPLETO!", SCREEN_WIDTH / 2 - MeasureText("NIVEL 1 COMPLETO!", 28) / 2, 170, 28, COL_GOLD);
    DrawText("David ha recolectado 5 piedras", SCREEN_WIDTH / 2 - MeasureText("David ha recolectado 5 piedras", 12) / 2, 230, 12, COL_TEXT);
    DrawText("en el arroyo", SCREEN_WIDTH / 2 - MeasureText("en el arroyo", 12) / 2, 250, 12, COL_TEXT);
    DrawText(TextFormat("Puntaje: %d", score), SCREEN_WIDTH / 2 - MeasureText(TextFormat("Puntaje: %d", score), 14) / 2, 290, 14, COL_GOLD);
    DrawText("Presiona ENTER para enfrentar a Goliat", SCREEN_WIDTH / 2 - MeasureText("Presiona ENTER para enfrentar a Goliat", 12) / 2, 340, 12, (Color){ 140, 130, 120, 255 });
}

static void DrawContinueArrowSimple(int y) {
    if ((int)(GetTime() * 3) % 2 == 0)
        DrawText("▼", SCREEN_WIDTH / 2 - 6, y, 20, COL_GOLD);
}

void DrawHistoria(int page, int totalPages) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 20, 22, 45, 255 });
    DrawText("La Unción de David", SCREEN_WIDTH / 2 - MeasureText("La Unción de David", 30) / 2, 50, 30, COL_GOLD);
    DrawLine(SCREEN_WIDTH / 2 - 120, 90, SCREEN_WIDTH / 2 + 120, 90, COL_GOLD);

    const char* textos[5];
    textos[0] = "\"Y aconteció que cuando ellos vinieron, él vio a\nEliab, y dijo: De cierto delante de Jehová está\nsu ungido... E hizo pasar Isaí sus siete hijos\ndelante de Samuel; pero Samuel dijo a Isaí:\nJehová no ha elegido a éstos.\"\n(1 Samuel 16:6, 10)";
    textos[1] = "\"Entonces dijo Samuel a Isaí: ¿Se han acabado\ntus jóvenes? Y él respondió: Queda aún el menor,\nque apacienta las ovejas... Y Jehová dijo:\nLevántate y úngelo, porque éste es.\"\n(1 Samuel 16:11, 12)";
    textos[2] = "\"Salió entonces del campamento de los filisteos un\npaladín, el cual se llamaba Goliat, de Gat, y\ntenía de altura seis codos y un palmo... Y se\nparaba y daba voces a los escuadrones de\nIsrael... Viendo Saúl y todo Israel estas\npalabras del filisteo, se turbaron y tuvieron\ngran miedo.\"\n(1 Samuel 17:4, 8, 11)";
    textos[3] = "\"Entonces habló David a los que estaban junto a\nél, diciendo: ¿Qué harán al hombre que venciere\na este filisteo, y quitare el oprobio de Israel?\nPorque ¿quién es este filisteo incircunciso, para\nque provoque a los escuadrones del Dios\nviviente?... Y dijo Saúl a David: Ve, y Jehová\nesté contigo.\"\n(1 Samuel 17:26, 37)";
    textos[4] = "\"David se dio prisa, y corrió a la línea de batalla\ncontra el filisteo. Y metiendo David su mano en\nla bolsa, tomó de allí una piedra, y la tiró con\nla honda...\"\n(1 Samuel 17:48, 49)\n\n¡Ayúdalo a vencer a Goliat!";

    DrawText(textos[page], SCREEN_WIDTH / 2 - MeasureText(textos[page], 20) / 2, 140, 20, (Color){ 255, 247, 231, 220 });
    DrawText(TextFormat("Pagina %d/%d", page + 1, totalPages),
        SCREEN_WIDTH / 2 - 30, 540, 16, (Color){ 140, 130, 120, 255 });
    DrawText("Presiona ENTER para continuar",
        SCREEN_WIDTH / 2 - MeasureText("Presiona ENTER para continuar", 14) / 2,
        565, 14, COL_GOLD);
}

void DrawVictoriaHistoria(int page, int totalPages) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 20, 22, 45, 255 });
    DrawText("Victoria sobre Goliat", SCREEN_WIDTH / 2 - MeasureText("Victoria sobre Goliat", 30) / 2, 50, 30, COL_GOLD);
    DrawLine(SCREEN_WIDTH / 2 - 120, 90, SCREEN_WIDTH / 2 + 120, 90, COL_GOLD);

    const char* textos[4];
    textos[0] = "\"Entonces corrió David y se puso sobre el filisteo,\ny tomando la espada de él y sacándola de su\nvaina, lo mató, y le cortó con ella la cabeza.\nY cuando los filisteos vieron muerto a su\npaladín, huyeron.\"\n(1 Samuel 17:51)";
    textos[1] = "\"Levantándose luego los de Israel y de Judá,\ndieron gritos, y siguieron a los filisteos hasta\nel valle, y hasta las puertas de Ecrón. Y cayeron\nlos heridos de los filisteos por el camino de\nSaaraim hasta Gat y Ecrón.\"\n(1 Samuel 17:52)";
    textos[2] = "\"Y David tomó la cabeza del filisteo y la trajo\na Jerusalén... Y cantaban las mujeres que\ndanzaban, y decían: Saúl hirió a sus miles, y\nDavid a sus diez miles.\"\n(1 Samuel 17:54; 18:7)";
    textos[3] = "\"Así venció David al filisteo con honda y piedra;\ne hirió al filisteo y lo mató, sin tener David\nespada en su mano... Y Saúl le tomó aquel día,\ny no le dejó volver a casa de su padre... porque\nJehová estaba con él.\"\n(1 Samuel 17:50; 18:2, 12)";

    DrawText(textos[page], SCREEN_WIDTH / 2 - MeasureText(textos[page], 20) / 2, 140, 20, (Color){ 255, 247, 231, 220 });
    DrawText(TextFormat("Pagina %d/%d", page + 1, totalPages),
        SCREEN_WIDTH / 2 - 30, 540, 16, (Color){ 140, 130, 120, 255 });
    DrawText("Presiona ENTER para continuar",
        SCREEN_WIDTH / 2 - MeasureText("Presiona ENTER para continuar", 14) / 2,
        565, 14, COL_GOLD);
}

void DrawNivel3Intro(int page, int totalPages) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 20, 22, 45, 255 });
    DrawText("El Desafio en Gat", SCREEN_WIDTH / 2 - MeasureText("El Desafio en Gat", 30) / 2, 50, 30, COL_GOLD);
    DrawLine(SCREEN_WIDTH / 2 - 120, 90, SCREEN_WIDTH / 2 + 120, 90, COL_GOLD);

    const char* textos[4];
    textos[0] = "\"Después hubo otra guerra en Gat,\ndonde había un hombre de gran\nestatura...\"\n(2 Samuel 21:20)";
    textos[1] = "\"...el cual tenía doce dedos\nen las manos, y otros doce en los\npies, veinticuatro por todos; y\ntambién era descendiente de los\ngigantes.\"\n(2 Samuel 21:20)";
    textos[2] = "\"¿Quién es este filisteo\nincircunciso, para que provoque\na los escuadrones del Dios\nviviente?\"\n(1 Samuel 17:26)";
    textos[3] = "\"Este desafió a Israel...\nJonatán, hijo de Simea\nhermano de David.\"\n(2 Samuel 21:21)\n\n¡Ayúdalo a vencer!";

    DrawText(textos[page], SCREEN_WIDTH / 2 - MeasureText(textos[page], 20) / 2, 140, 20, (Color){ 255, 247, 231, 220 });
    DrawText(TextFormat("Pagina %d/%d", page + 1, totalPages),
        SCREEN_WIDTH / 2 - 30, 540, 16, (Color){ 140, 130, 120, 255 });
    DrawText("Presiona ENTER para continuar",
        SCREEN_WIDTH / 2 - MeasureText("Presiona ENTER para continuar", 14) / 2,
        565, 14, COL_GOLD);
}

void UnloadMapTerrain() {
    if (mapTerrainLoaded && mapTerrainTex.id > 0) {
        UnloadTexture(mapTerrainTex);
        mapTerrainLoaded = false;
    }
}

void DrawNivel3Outro(int page, int totalPages) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 20, 22, 45, 255 });
    DrawText("Victoria en Gat", SCREEN_WIDTH / 2 - MeasureText("Victoria en Gat", 30) / 2, 50, 30, COL_GOLD);
    DrawLine(SCREEN_WIDTH / 2 - 120, 90, SCREEN_WIDTH / 2 + 120, 90, COL_GOLD);

    const char* textos[3];
    textos[0] = "\"Este desafió a Israel,\ny lo mató Jonatán,\nhijo de Simea hermano de David.\"\n(2 Samuel 21:21)";
    textos[1] = "\"Estos cuatro eran descendientes\nde los gigantes en Gat, los cuales\ncayeron por mano de David y por\nmano de sus siervos.\"\n(2 Samuel 21:22)";
    textos[2] = "El tamaño de tu enemigo no define\nel tamaño de tu victoria.\n\nAsí como David venció en el valle,\nJonatán venció en Gat. No por la\nfuerza de sus espadas, sino por\nla fe en el Dios de los ejércitos.\n\nLos gigantes caen cuando los hombres\nvalientes se atreven a creer.";

    DrawText(textos[page], SCREEN_WIDTH / 2 - MeasureText(textos[page], 20) / 2, 140, 20, (Color){ 255, 247, 231, 220 });
    DrawText(TextFormat("Pagina %d/%d", page + 1, totalPages),
        SCREEN_WIDTH / 2 - 30, 540, 16, (Color){ 140, 130, 120, 255 });
    DrawText("Presiona ENTER para continuar",
        SCREEN_WIDTH / 2 - MeasureText("Presiona ENTER para continuar", 14) / 2,
        565, 14, COL_GOLD);
}

void DrawNivel3Victory(int score) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 20, 22, 45, 230 });
    DrawRectangle(SCREEN_WIDTH / 2 - 200, 140, 400, 250, COL_OUTER);
    DrawRectangle(SCREEN_WIDTH / 2 - 198, 142, 396, 246, COL_GOLD);
    DrawRectangle(SCREEN_WIDTH / 2 - 196, 144, 392, 242, COL_BG);
    DrawText("NIVEL 3 COMPLETO!", SCREEN_WIDTH / 2 - MeasureText("NIVEL 3 COMPLETO!", 28) / 2, 170, 28, COL_GOLD);
    DrawText("Jonatán derrotó al gigante de Gat", SCREEN_WIDTH / 2 - MeasureText("Jonatán derrotó al gigante de Gat", 12) / 2, 230, 12, COL_TEXT);
    DrawText(TextFormat("Puntaje: %d", score), SCREEN_WIDTH / 2 - MeasureText(TextFormat("Puntaje: %d", score), 14) / 2, 290, 14, COL_GOLD);
    DrawText("Presiona ENTER para ver la historia final",
        SCREEN_WIDTH / 2 - MeasureText("Presiona ENTER para ver la historia final", 12) / 2, 340, 12, (Color){ 140, 130, 120, 255 });
}

void DrawHUDLevel3(int hp, int maxHp) {
    DrawRectangle(0, 0, SCREEN_WIDTH, 30, (Color){ 20, 22, 35, 220 });
    DrawRectangle(0, 28, SCREEN_WIDTH, 2, COL_GOLD);
    DrawText("Nivel 3 - La Fortaleza de Gat", SCREEN_WIDTH / 2 - 120, 7, 12, COL_GOLD);
    DrawHealthBar(10, 7, hp, maxHp);
}

// ---- Terrain texture for world map ----
static void CreateMapTerrainTexture() {
    if (mapTerrainLoaded) return;

    Image img = LoadImage("assets/mapa.png");
    if (img.data != NULL) {
        if (img.width != SCREEN_WIDTH || img.height != SCREEN_HEIGHT)
            ImageResize(&img, SCREEN_WIDTH, SCREEN_HEIGHT);
        mapTerrainTex = LoadTextureFromImage(img);
        UnloadImage(img);
        mapTerrainLoaded = true;
        return;
    }

    // ---- Fallback: terrain procedural si no existe mapa2.png ----
    const int w = SCREEN_WIDTH, h = SCREEN_HEIGHT;
    img = GenImageColor(w, h, (Color){ 228, 210, 178, 255 });

    auto px = [&](int x, int y, Color c) {
        if (x >= 0 && x < w && y >= 0 && y < h) ImageDrawPixel(&img, x, y, c);
    };

    // ---- 1. Mediterranean Sea ----
    ImageDrawRectangle(&img, 0, 0, 82, h, (Color){ 60, 115, 175, 255 });
    ImageDrawRectangle(&img, 0, 0, 82, h, (Color){ 50, 100, 160, 200 });
    // Deep water
    ImageDrawRectangle(&img, 0, 0, 50, h, (Color){ 40, 85, 145, 255 });
    // Waves
    for (int y = 0; y < h; y += 12) {
        for (int x = 0; x < 75; x += 8) {
            int ox = (y / 12 % 2) * 4;
            px(x + ox + (int)(sinf(y * 0.3f) * 3), y + 3, (Color){ 90, 150, 195, 100 });
            px(x + ox + 2 + (int)(sinf(y * 0.3f + 1) * 3), y + 7, (Color){ 90, 150, 195, 70 });
        }
    }
    // Foam line on shore
    for (int y = 0; y < h; y++) {
        int sx = 78 + (int)(sinf(y * 0.08f) * 4);
        px(sx, y, (Color){ 200, 215, 230, 120 });
        px(sx + 1, y, (Color){ 200, 215, 230, 60 });
    }

    // ---- 2. Coastal plain (Philistia) ----
    for (int y = 0; y < h; y++) {
        int plainW = 140 + (int)(sinf(y * 0.015f + 1.2f) * 25);
        int startX = 82;
        for (int x = startX; x < startX + plainW && x < 260; x++) {
            int n = (x * 13 + y * 7) % 20;
            Color c = { (unsigned char)(188 + n), (unsigned char)(178 + n), (unsigned char)(148 + n), 255 };
            px(x, y, c);
        }
    }
    // Sandy beach strip
    for (int y = 0; y < h; y++) {
        for (int x = 82; x < 92; x++)
            px(x, y, (Color){ 215, 198, 158, 255 });
    }

    // ---- 3. Shephelah foothills (rolling hills) ----
    struct { int x, y, r; Color c; } hills[] = {
        // Back row (darker)
        {240, 150, 55, {110, 145, 85, 255}}, {275, 135, 60, {115, 150, 90, 255}},
        {310, 145, 55, {110, 145, 85, 255}}, {345, 130, 58, {118, 152, 92, 255}},
        {380, 140, 52, {112, 146, 86, 255}}, {415, 135, 56, {116, 150, 90, 255}},
        {450, 148, 50, {110, 145, 85, 255}},
        // Middle row (medium)
        {255, 195, 60, {135, 168, 105, 255}}, {290, 180, 65, {142, 175, 112, 255}},
        {325, 190, 58, {138, 170, 108, 255}}, {360, 175, 62, {145, 178, 115, 255}},
        {395, 185, 55, {140, 172, 110, 255}}, {430, 178, 60, {143, 176, 113, 255}},
        {465, 190, 52, {136, 168, 106, 255}},
        // Front row (lighter)
        {265, 245, 65, {155, 185, 128, 255}}, {300, 230, 70, {160, 190, 135, 255}},
        {335, 240, 62, {155, 185, 128, 255}}, {370, 225, 68, {162, 192, 138, 255}},
        {405, 235, 60, {157, 187, 130, 255}}, {440, 228, 64, {160, 190, 135, 255}},
        {475, 240, 55, {153, 183, 126, 255}},
        // Bottom row
        {280, 300, 55, {145, 178, 115, 255}}, {315, 290, 60, {150, 183, 120, 255}},
        {350, 295, 55, {147, 180, 117, 255}}, {385, 285, 58, {152, 185, 122, 255}},
        {420, 295, 52, {148, 180, 118, 255}}, {455, 290, 55, {150, 183, 120, 255}},
        // Southern hills
        {260, 360, 50, {138, 170, 108, 255}}, {295, 350, 55, {142, 175, 112, 255}},
        {330, 355, 50, {140, 172, 110, 255}}, {365, 345, 52, {144, 177, 114, 255}},
        {400, 350, 48, {141, 173, 111, 255}}, {435, 348, 50, {143, 176, 113, 255}},
        // Far south
        {290, 415, 48, {130, 162, 100, 255}}, {325, 405, 50, {135, 168, 105, 255}},
        {360, 410, 45, {132, 164, 102, 255}}, {395, 405, 48, {136, 168, 106, 255}},
    };
    for (auto& h : hills)
        ImageDrawCircle(&img, h.x, h.y, h.r, h.c);

    // ---- 4. Valley of Elah (lighter wedge cut through hills) ----
    // The valley opens west toward Gath and narrows east toward Bethlehem
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            // Valley shape: triangular wedge from (290,205) to (240,270) at west end
            // narrowing to (450,232) at east end
            float xNorm = (x - 240.0f) / 210.0f; // 0 at west, 1 at east (x=450)
            if (x >= 240 && x <= 450 && xNorm >= 0 && xNorm <= 1) {
                float valleyTop = 205 + xNorm * 30;
                float valleyBot = 270 - xNorm * 20;
                if (y >= valleyTop && y <= valleyBot) {
                    int n = (x * 17 + y * 11) % 15;
                    Color vc = { (unsigned char)(175 + n), (unsigned char)(170 + n), (unsigned char)(145 + n), 255 };
                    px(x, y, vc);
                }
            }
        }
    }
    // Stream through the valley
    for (int i = 0; i < 120; i++) {
        float t = i / 120.0f;
        float sx = 240 + t * 220;
        float sy = 240 + t * (-10) + sinf(t * 12.56f + 0.5f) * 12;
        px((int)sx, (int)sy, (Color){ 130, 180, 175, 180 });
        px((int)sx, (int)sy + 1, (Color){ 130, 180, 175, 140 });
        px((int)sx + 1, (int)sy, (Color){ 130, 180, 175, 120 });
    }

    // ---- 5. Judean mountains ----
    // Back ridge (taller, further east)
    struct { int bx, by, bw, bh; Color c; } mtns[] = {
        // Back ridge
        {475, 380, 90, 170, {90, 130, 72, 255}}, {525, 375, 100, 190, {95, 135, 76, 255}},
        {575, 370, 110, 200, {92, 132, 73, 255}}, {625, 375, 95, 175, {97, 137, 78, 255}},
        {670, 380, 85, 150, {90, 130, 72, 255}},
        // Front ridge
        {490, 410, 85, 140, {108, 148, 85, 255}}, {540, 400, 95, 160, {112, 152, 88, 255}},
        {590, 395, 100, 170, {110, 150, 86, 255}}, {640, 405, 90, 145, {114, 154, 90, 255}},
        {680, 415, 80, 120, {108, 148, 85, 255}},
        // Third ridge
        {500, 435, 80, 110, {125, 162, 98, 255}}, {550, 430, 90, 130, {128, 165, 102, 255}},
        {600, 425, 95, 140, {125, 162, 98, 255}}, {650, 435, 85, 115, {130, 168, 105, 255}},
    };
    for (auto& m : mtns) {
        Vector2 v1 = { (float)(m.bx - m.bw / 2), (float)m.by };
        Vector2 v2 = { (float)(m.bx + m.bw / 2), (float)m.by };
        Vector2 v3 = { (float)m.bx, (float)(m.by - m.bh) };
        ImageDrawTriangle(&img, v1, v2, v3, m.c);
        // Shadow on east side
        Vector2 s1 = { (float)(m.bx + m.bw / 4), (float)(m.by - m.bh * 0.3f) };
        Vector2 s2 = { (float)(m.bx + m.bw / 3), (float)(m.by - m.bh * 0.7f) };
        Vector2 s3 = { (float)(m.bx + m.bw / 2), (float)m.by };
        ImageDrawTriangle(&img, s1, s2, s3, Fade(BLACK, 0.08f));
        // Snow cap on tallest peaks
        if (m.bh > 160) {
            Vector2 w1 = { (float)(m.bx - 6), (float)(m.by - m.bh + 15) };
            Vector2 w2 = { (float)(m.bx + 6), (float)(m.by - m.bh + 15) };
            Vector2 w3 = { (float)m.bx, (float)(m.by - m.bh) };
            ImageDrawTriangle(&img, w1, w2, w3, (Color){ 240, 242, 247, 180 });
        }
    }
    // Mountain ridge line (connects peaks)
    for (int i = 0; i < 40; i++) {
        float t = i / 40.0f;
        float rx = 440 + t * 260;
        float ry = 250 + sinf(t * 3.14f) * 30 + sinf(t * 8.0f) * 8;
        px((int)rx, (int)ry, (Color){ 85, 120, 65, 80 });
    }

    // ---- 6. Judean wilderness (eastern slope) ----
    for (int y = 0; y < h; y++) {
        for (int x = 650; x < 730; x++) {
            float slope = (x - 650.0f) / 80.0f;
            int n = (x * 7 + y * 13) % 25;
            if (y > 250 + slope * 80 && y < 550) {
                unsigned char br = (unsigned char)(170 - slope * 40 + n * 0.3f);
                unsigned char bg = (unsigned char)(150 - slope * 50 + n * 0.3f);
                unsigned char bb = (unsigned char)(120 - slope * 30 + n * 0.3f);
                px(x, y, (Color){ br, bg, bb, 255 });
            }
        }
    }

    // ---- 7. Dead Sea ----
    for (int y = 285; y < 520; y++) {
        float yNorm = (y - 285.0f) / 235.0f;
        int ovalW = (int)(30 + sinf(yNorm * 3.14f) * 20);
        int cx = 737;
        for (int x = cx - ovalW; x <= cx + ovalW; x++) {
            float dx = (x - cx) / (float)ovalW;
            if (dx * dx + (yNorm - 0.5f) * (yNorm - 0.5f) * 0.7f <= 1.0f) {
                int n2 = (x * 3 + y * 7) % 10;
                unsigned char db = (unsigned char)(42 + n2);
                unsigned char dg = (unsigned char)(65 + n2);
                unsigned char dr = (unsigned char)(135 + n2);
                px(x, y, (Color){ dr, dg, db, 255 });
            }
        }
    }
    // Dead Sea shore
    for (int y = 282; y < 522; y++) {
        float yNorm = (y - 285.0f) / 235.0f;
        int ow = (int)(33 + sinf(yNorm * 3.14f) * 22);
        int cx = 737;
        for (int side = -1; side <= 1; side += 2) {
            int sx = cx + side * ow;
            px(sx, y, (Color){ 180, 170, 140, 180 });
            px(sx + side * 1, y, (Color){ 180, 170, 140, 120 });
        }
    }

    // ---- 8. Jordan River ----
    for (int i = 0; i < 80; i++) {
        float t = i / 80.0f;
        float jx = 690 + t * 50 + sinf(t * 11.0f) * 8;
        float jy = 0 + t * 295;
        px((int)jx, (int)jy, (Color){ 70, 140, 175, 150 });
        px((int)jx + 1, (int)jy, (Color){ 70, 140, 175, 100 });
    }

    // ---- 9. Terrain detail (trees, scrub) ----
    // Small tree clusters in the hills
    for (int i = 0; i < 200; i++) {
        int tx = 80 + (i * 37 + 11) % 600;
        int ty = (i * 23 + 7) % 520 + 40;
        // Only place trees in hill/mountain regions
        int rVal = ((tx + ty) * 7 + i * 13) % 20;
        if (tx > 250 && tx < 700 && ty > 100 && ty < 500 && rVal < 3) {
            px(tx, ty, (Color){ 65, 115, 50, 180 });
            px(tx - 1, ty + 1, (Color){ 65, 115, 50, 140 });
            px(tx + 1, ty + 1, (Color){ 65, 115, 50, 140 });
        }
    }

    // ---- 10. Roads ----
    // Via Maris (coastal road)
    for (int i = 0; i < 200; i++) {
        float t = i / 200.0f;
        float rx = 95 + sinf(t * 1.5f + 0.3f) * 10;
        float ry = 10 + t * 550;
        if ((int)(t * 120) % 3 == 0) {
            px((int)rx, (int)ry, (Color){ 130, 100, 70, 150 });
            px((int)rx + 1, (int)ry, (Color){ 130, 100, 70, 100 });
        }
    }
    // Road to Bethlehem (from coast up to mountains)
    for (int i = 0; i < 150; i++) {
        float t = i / 150.0f;
        float rx = 210 + t * 375;
        float ry = 260 + t * (-20) + sinf(t * 3.0f) * 5;
        if ((int)(t * 80) % 3 == 0)
            px((int)rx, (int)ry, (Color){ 140, 110, 75, 150 });
    }

    // ---- 11. Decorative border ----
    // Outer frame
    for (int x = 0; x < w; x++) {
        px(x, 0, (Color){ 80, 60, 40, 200 });
        px(x, 1, (Color){ 80, 60, 40, 100 });
        px(x, h - 1, (Color){ 80, 60, 40, 200 });
        px(x, h - 2, (Color){ 80, 60, 40, 100 });
    }
    for (int y = 0; y < h; y++) {
        px(0, y, (Color){ 80, 60, 40, 200 });
        px(1, y, (Color){ 80, 60, 40, 100 });
        px(w - 1, y, (Color){ 80, 60, 40, 200 });
        px(w - 2, y, (Color){ 80, 60, 40, 100 });
    }
    // Inner gold line
    for (int x = 4; x < w - 4; x++) {
        px(x, 3, (Color){ 200, 168, 82, 120 });
        px(x, h - 4, (Color){ 200, 168, 82, 120 });
    }
    for (int y = 4; y < h - 4; y++) {
        px(3, y, (Color){ 200, 168, 82, 120 });
        px(w - 4, y, (Color){ 200, 168, 82, 120 });
    }
    // Corner ornaments
    for (int x = 5; x <= 15; x++) { px(x, 5, COL_GOLD); px(x, h - 6, COL_GOLD); }
    for (int x = w - 16; x <= w - 6; x++) { px(x, 5, COL_GOLD); px(x, h - 6, COL_GOLD); }
    for (int y = 5; y <= 15; y++) { px(5, y, COL_GOLD); px(w - 6, y, COL_GOLD); }
    for (int y = h - 16; y <= h - 6; y++) { px(5, y, COL_GOLD); px(w - 6, y, COL_GOLD); }

    // ---- 12. Compass rose ----
    int cx = 40, cy = 520;
    ImageDrawCircle(&img, cx, cy, 20, (Color){ 80, 60, 40, 60 });
    ImageDrawCircleLines(&img, cx, cy, 20, (Color){ 120, 100, 70, 80 });
    // N, S, E, W points
    for (int d = 0; d < 4; d++) {
        float a = d * 3.14159f / 2.0f;
        int ex = cx + (int)(cosf(a) * 18);
        int ey = cy + (int)(sinf(a) * 18);
        for (int l = 0; l < 6; l++) {
            float la = a + (l - 3) * 0.08f;
            int lx = cx + (int)(cosf(la) * (12 + l * 1.5f));
            int ly = cy + (int)(sinf(la) * (12 + l * 1.5f));
            ImageDrawLine(&img, cx, cy, lx, ly, (Color){ 150, 120, 80, 150 });
        }
    }

    mapTerrainTex = LoadTextureFromImage(img);
    UnloadImage(img);
    mapTerrainLoaded = true;
}

void DrawMapaMundi(int pathIndex, float progress, bool arrived) {
    CreateMapTerrainTexture();
    DrawTexture(mapTerrainTex, 0, 0, WHITE);

    // ---- Location names on map ----
    // Jerusalem (reference city, not a destination)
    DrawCircle(545, 118, 5, (Color){ 200, 180, 120, 200 });
    DrawCircleLines(545, 118, 5, (Color){ 150, 130, 80, 255 });
    DrawText("JERUSALEN", 515, 126, 12, (Color){ 80, 55, 30, 200 });

    // Bethlehem
    DrawCircle(576, 178, 6, (Color){ 255, 220, 100, 255 });
    DrawCircleLines(576, 178, 6, (Color){ 180, 140, 60, 255 });
    DrawText("BELEN", 552, 186, 13, (Color){ 80, 55, 30, 255 });

    // Valley of Elah
    DrawCircle(362, 234, 6, (Color){ 255, 220, 100, 255 });
    DrawCircleLines(362, 234, 6, (Color){ 180, 140, 60, 255 });
    DrawText("VALLE DE ELEH", 312, 242, 12, (Color){ 80, 55, 30, 255 });

    // Gath
    DrawCircle(206, 252, 6, (Color){ 255, 220, 100, 255 });
    DrawCircleLines(206, 252, 6, (Color){ 180, 140, 60, 255 });
    DrawText("GAT", 186, 260, 13, (Color){ 80, 55, 30, 255 });

    // Additional reference cities (smaller) 
    DrawCircle(338, 200, 3, (Color){ 200, 180, 120, 180 });
    DrawText("Azeca", 318, 206, 9, (Color){ 100, 70, 40, 160 });
    DrawCircle(374, 268, 3, (Color){ 200, 180, 120, 180 });
    DrawText("Socoh", 354, 274, 9, (Color){ 100, 70, 40, 160 });
    DrawCircle(112, 280, 3, (Color){ 200, 180, 120, 180 });
    DrawText("Asdod", 98, 286, 9, (Color){ 100, 70, 40, 160 });
    DrawCircle(160, 196, 3, (Color){ 200, 180, 120, 180 });
    DrawText("Ecrón", 146, 202, 9, (Color){ 100, 70, 40, 160 });
    DrawCircle(515, 350, 3, (Color){ 200, 180, 120, 180 });
    DrawText("Hebrón", 498, 356, 9, (Color){ 100, 70, 40, 160 });
    DrawCircle(85, 300, 3, (Color){ 200, 180, 120, 180 });
    DrawText("Ascalón", 70, 306, 9, (Color){ 100, 70, 40, 160 });
    DrawCircle(68, 370, 3, (Color){ 200, 180, 120, 180 });
    DrawText("Gaza", 54, 376, 9, (Color){ 100, 70, 40, 160 });

    // Region labels
    DrawText("MAR MEDITERRANEO", 4, 50, 10, (Color){ 180, 210, 235, 120 });
    DrawText("FILISTIA", 115, 30, 10, (Color){ 130, 100, 60, 100 });
    DrawText("SEFELA", 270, 90, 10, (Color){ 110, 140, 80, 100 });
    DrawText("MONTES DE JUDA", 520, 30, 10, (Color){ 90, 120, 65, 100 });
    DrawText("DESIERTO DE JUDA", 660, 50, 10, (Color){ 140, 120, 85, 100});
    DrawText("MAR MUERTO", 700, 540, 10, (Color){ 100, 140, 180, 120 });

    // ---- Route path (dotted line following waypoints) ----
    int segCount = 8;
    Vector2 waypoints[2][9] = {
        { {567,275}, {543,283}, {519,293}, {493,301}, {467,305}, {441,312}, {413,319}, {383,326}, {353,331} },
        { {353,331}, {335,339}, {316,345}, {296,350}, {276,352}, {256,351}, {236,350}, {216,349}, {197,349} }
    };
    for (int s = 0; s < segCount; s++) {
        Vector2 a = waypoints[pathIndex][s];
        Vector2 b = waypoints[pathIndex][s + 1];
        float segLen = sqrtf((b.x-a.x)*(b.x-a.x) + (b.y-a.y)*(b.y-a.y));
        int dots = (int)(segLen / 4.0f);
        for (int d = 0; d < dots; d++) {
            float t = (float)d / dots;
            float px = a.x + (b.x - a.x) * t;
            float py = a.y + (b.y - a.y) * t;
            if ((s * 100 + d) % 3 == 0)
                DrawPixel((int)px, (int)py, (Color){ 220, 180, 80, 200 });
        }
    }
    // DEBUG: red dots on each waypoint
    for (int i = 0; i <= segCount; i++) {
        DrawCircle((int)waypoints[pathIndex][i].x, (int)waypoints[pathIndex][i].y, 5, RED);
    }

    // ---- Destination glow ----
    if (arrived) {
        Vector2 dst = (pathIndex == 0) ? (Vector2){ 353, 331 } : (Vector2){ 197, 349 };
        float pulse = 0.15f + sinf(GetTime() * 4.5f) * 0.12f;
        DrawCircle((int)dst.x, (int)dst.y, 18, (Color){ 255, 220, 100, (unsigned char)(pulse * 255) });
        DrawCircleLines((int)dst.x, (int)dst.y, 18, (Color){ 255, 200, 60, (unsigned char)(pulse * 200) });
    }

    // ---- Player character icon ----
    float seg = progress * segCount;
    int idx = (int)seg;
    float frac = seg - idx;
    if (idx >= segCount) { idx = segCount; frac = 0; }
    float ppx = waypoints[pathIndex][idx].x + (waypoints[pathIndex][idx + 1].x - waypoints[pathIndex][idx].x) * frac;
    float ppy = waypoints[pathIndex][idx].y + (waypoints[pathIndex][idx + 1].y - waypoints[pathIndex][idx].y) * frac;

    DrawCircle((int)ppx, (int)ppy, 8, (Color){ 45, 130, 210, 255 });
    DrawCircleLines((int)ppx, (int)ppy, 8, (Color){ 25, 85, 175, 255 });
    DrawCircle((int)ppx, (int)ppy, 4, (Color){ 180, 210, 250, 200 });
    // Tiny "head" on character
    DrawCircle((int)ppx, (int)ppy - 4, 3, (Color){ 220, 195, 160, 255 });

    // ---- Bottom bar ----
    DrawRectangle(0, 550, SCREEN_WIDTH, 50, (Color){ 0, 0, 0, 140 });
    if (arrived) {
        DrawText("Presiona ENTER para continuar",
            SCREEN_WIDTH / 2 - MeasureText("Presiona ENTER para continuar", 16) / 2,
            562, 16, COL_GOLD);
    } else {
        DrawText("Viajando...",
            SCREEN_WIDTH / 2 - MeasureText("Viajando...", 16) / 2,
            562, 16, (Color){ 255, 247, 231, 200 });
    }

    // ---- Title ----
    DrawText("Tierra de Israel", SCREEN_WIDTH / 2 - MeasureText("Tierra de Israel", 18) / 2, 4, 18, (Color){ 220, 200, 170, 230 });
    DrawText("Tierra de Israel", SCREEN_WIDTH / 2 - MeasureText("Tierra de Israel", 18) / 2 + 1, 5, 18, (Color){ 60, 45, 25, 100 });
}

void DrawMenu(int sel) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 40, 60, 120, 255 });
    DrawText("DAVID Y GOLIAT", SCREEN_WIDTH / 2 - MeasureText("DAVID Y GOLIAT", 36) / 2, 100, 36, (Color){ 210, 170, 40, 255 });
    DrawText("DAVID Y GOLIAT", SCREEN_WIDTH / 2 - MeasureText("DAVID Y GOLIAT", 36) / 2 + 2, 102, 36, (Color){ 255, 247, 231, 80 });

    const int firstY = 220;
    const int spacing = 45;
    const char* items[3] = { "Iniciar", "Instrucciones", "Salir" };
    for (int i = 0; i < 3; i++) {
        int x = SCREEN_WIDTH / 2 - MeasureText(items[i], 22) / 2;
        int y = firstY + i * spacing;
        if (i == sel) {
            DrawText(items[i], x, y, 22, (Color){ 210, 170, 40, 255 });
            DrawText("►", x - 28, y, 18, (Color){ 210, 170, 40, 255 });
        } else {
            DrawText(items[i], x, y, 22, (Color){ 200, 195, 185, 255 });
        }
    }
    DrawText("Usa ▲/▼ para navegar, ENTER para seleccionar",
        SCREEN_WIDTH / 2 - MeasureText("Usa ▲/▼ para navegar, ENTER para seleccionar", 12) / 2,
        420, 12, (Color){ 140, 130, 120, 200 });
}

void DrawInstrucciones() {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 40, 60, 120, 255 });
    DrawText("INSTRUCCIONES", SCREEN_WIDTH / 2 - MeasureText("INSTRUCCIONES", 28) / 2, 80, 28, (Color){ 210, 170, 40, 255 });
    DrawLine(SCREEN_WIDTH / 2 - 150, 120, SCREEN_WIDTH / 2 + 150, 120, (Color){ 210, 170, 40, 150 });
    const char* lines[7];
    lines[0] = "Nivel 1: Junta 5 piedras en el arroyo";
    lines[1] = "Nivel 2: Derrota a Goliat con una piedra";
    lines[2] = "";
    lines[3] = "WASD / Flechas  -  Mover a David";
    lines[4] = "ESPACIO  -  Disparar piedra (Nivel 2)";
    lines[5] = "Mouse  -  Apuntar la mira";
    lines[6] = "";
    for (int i = 0; i < 7; i++) {
        if (lines[i][0] == '\0') continue;
        DrawText(lines[i], SCREEN_WIDTH / 2 - MeasureText(lines[i], 16) / 2, 160 + i * 30, 16, (Color){ 255, 247, 231, 220 });
    }
    DrawText("Presiona ENTER o ESC para volver",
        SCREEN_WIDTH / 2 - MeasureText("Presiona ENTER o ESC para volver", 14) / 2,
        480, 14, (Color){ 210, 170, 40, 200 });
}

void DrawGameOver(int score) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 20, 22, 45, 230 });
    DrawRectangle(SCREEN_WIDTH / 2 - 160, 150, 320, 200, COL_OUTER);
    DrawRectangle(SCREEN_WIDTH / 2 - 158, 152, 316, 196, COL_GOLD);
    DrawRectangle(SCREEN_WIDTH / 2 - 156, 154, 312, 192, COL_BG);
    DrawText("DERROTA", SCREEN_WIDTH / 2 - MeasureText("DERROTA", 32) / 2, 180, 32, (Color){ 200, 50, 50, 255 });
    DrawText(TextFormat("Puntaje final: %d", score),
        SCREEN_WIDTH / 2 - MeasureText(TextFormat("Puntaje final: %d", score), 14) / 2, 240, 14, COL_TEXT);
    DrawText("Presiona ENTER para volver al menu",
        SCREEN_WIDTH / 2 - MeasureText("Presiona ENTER para volver al menu", 12) / 2, 280, 12, (Color){ 140, 130, 120, 255 });
}

void DrawVictory(int score) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 20, 22, 45, 230 });
    DrawRectangle(SCREEN_WIDTH / 2 - 180, 120, 360, 250, COL_OUTER);
    DrawRectangle(SCREEN_WIDTH / 2 - 178, 122, 356, 246, COL_GOLD);
    DrawRectangle(SCREEN_WIDTH / 2 - 176, 124, 352, 242, COL_BG);
    DrawText("VICTORIA!", SCREEN_WIDTH / 2 - MeasureText("VICTORIA!", 36) / 2, 150, 36, COL_GOLD);
    DrawText("David derroto a Goliat con una piedra!",
        SCREEN_WIDTH / 2 - MeasureText("David derroto a Goliat con una piedra!", 14) / 2, 210, 14, COL_TEXT);
    DrawText(TextFormat("Puntaje final: %d", score),
        SCREEN_WIDTH / 2 - MeasureText(TextFormat("Puntaje final: %d", score), 14) / 2, 250, 14, COL_GOLD);
    DrawText("Presiona ENTER para volver al menu",
        SCREEN_WIDTH / 2 - MeasureText("Presiona ENTER para volver al menu", 12) / 2, 300, 12, (Color){ 140, 130, 120, 255 });
}

Texture2D CreateDavidPortrait() {
    Image img = GenImageColor(PORTRAIT_W, PORTRAIT_H, BLANK);
    auto px = [&](int x, int y, Color c) {
        if (x >= 0 && x < PORTRAIT_W && y >= 0 && y < PORTRAIT_H)
            ImageDrawPixel(&img, x, y, c);
    };

    // ---- HAIR (back layer) ----
    // Spiky hair mass
    for (int y = 0; y < 28; y++)
        for (int x = 24 - y/3; x < 72 + y/3; x++)
            px(x, y + 4, (Color){ 74, 40, 48, 255 });
    // Hair spikes (top)
    for (int x = 28; x < 36; x++) for (int y = 0; y < 8; y++) px(x, y, (Color){ 74, 40, 48, 255 });
    for (int x = 40; x < 48; x++) for (int y = 0; y < 10; y++) px(x, y, (Color){ 74, 40, 48, 255 });
    for (int x = 52; x < 60; x++) for (int y = 0; y < 8; y++) px(x, y, (Color){ 74, 40, 48, 255 });
    for (int x = 62; x < 68; x++) for (int y = 0; y < 6; y++) px(x, y, (Color){ 74, 40, 48, 255 });
    // Hair highlight
    for (int x = 34; x < 40; x++) px(x, 6, (Color){ 107, 58, 80, 255 });
    for (int x = 42; x < 48; x++) px(x, 4, (Color){ 107, 58, 80, 255 });
    for (int x = 50; x < 56; x++) px(x, 6, (Color){ 107, 58, 80, 255 });

    // ---- FACE & NECK ----
    // Neck
    for (int y = 52; y < 62; y++)
        for (int x = 40; x < 56; x++)
            px(x, y, (Color){ 215, 185, 150, 255 });
    // Neck shadow
    for (int x = 40; x < 44; x++)
        for (int y = 52; y < 62; y++)
            px(x, y, (Color){ 185, 160, 130, 255 });

    // Face oval (center 48, base at y=52)
    ImageDrawCircle(&img, 48, 34, 22, (Color){ 230, 195, 158, 255 });
    ImageDrawCircle(&img, 48, 34, 21, (Color){ 230, 195, 158, 255 });
    // Face outline
    ImageDrawCircle(&img, 48, 34, 22, (Color){ 50, 38, 32, 255 });
    // Chin detail
    for (int x = 42; x < 54; x++) px(x, 50, (Color){ 210, 178, 145, 255 });
    for (int x = 44; x < 52; x++) px(x, 51, (Color){ 210, 178, 145, 255 });

    // ---- HAIR (front / fringe) ----
    for (int y = 14; y < 24; y++)
        for (int x = 30 - (y-14)/3; x < 66 + (y-14)/3; x++)
            px(x, y, (Color){ 74, 40, 48, 255 });
    // Fringe strands pointing down
    for (int x = 32; x < 38; x++) for (int y = 22; y < 28; y++) px(x, y, (Color){ 74, 40, 48, 255 });
    for (int x = 40; x < 46; x++) for (int y = 22; y < 27; y++) px(x, y, (Color){ 74, 40, 48, 255 });
    for (int x = 50; x < 54; x++) for (int y = 22; y < 26; y++) px(x, y, (Color){ 74, 40, 48, 255 });
    for (int x = 56; x < 62; x++) for (int y = 22; y < 28; y++) px(x, y, (Color){ 74, 40, 48, 255 });
    // Fringe highlight
    for (int x = 34; x < 38; x++) px(x, 20, (Color){ 107, 58, 80, 255 });
    for (int x = 42; x < 46; x++) px(x, 20, (Color){ 107, 58, 80, 255 });
    for (int x = 56; x < 60; x++) px(x, 20, (Color){ 107, 58, 80, 255 });
    // Side hair
    for (int x = 20; x < 28; x++) for (int y = 24; y < 40; y++) px(x, y, (Color){ 74, 40, 48, 255 });
    for (int x = 68; x < 76; x++) for (int y = 24; y < 38; y++) px(x, y, (Color){ 74, 40, 48, 255 });

    // ---- EYEBROWS ----
    for (int x = 34; x <= 40; x++) px(x, 26, (Color){ 50, 38, 32, 255 });
    for (int x = 55; x <= 61; x++) px(x, 26, (Color){ 50, 38, 32, 255 });

    // ---- GLASSES ----
    // Left lens frame
    for (int y = 28; y <= 36; y++) { px(32, y, (Color){ 20, 20, 20, 255 }); px(44, y, (Color){ 20, 20, 20, 255 }); }
    for (int x = 32; x <= 44; x++) { px(x, 28, (Color){ 20, 20, 20, 255 }); px(x, 36, (Color){ 20, 20, 20, 255 }); }
    // Right lens frame
    for (int y = 28; y <= 36; y++) { px(51, y, (Color){ 20, 20, 20, 255 }); px(63, y, (Color){ 20, 20, 20, 255 }); }
    for (int x = 51; x <= 63; x++) { px(x, 28, (Color){ 20, 20, 20, 255 }); px(x, 36, (Color){ 20, 20, 20, 255 }); }
    // Bridge
    for (int x = 45; x <= 50; x++) px(x, 30, (Color){ 20, 20, 20, 255 });
    // Lens tint (light blue, subtle)
    for (int y = 29; y <= 35; y++)
        for (int x = 33; x <= 43; x++)
            px(x, y, (Color){ 160, 200, 232, 60 });
    for (int y = 29; y <= 35; y++)
        for (int x = 52; x <= 62; x++)
            px(x, y, (Color){ 160, 200, 232, 60 });
    // Lens shine (white reflection)
    px(34, 30, (Color){ 220, 240, 255, 180 });
    px(35, 30, (Color){ 220, 240, 255, 180 });
    px(34, 31, (Color){ 220, 240, 255, 120 });
    px(53, 30, (Color){ 220, 240, 255, 180 });
    px(54, 30, (Color){ 220, 240, 255, 180 });

    // ---- EYES (behind lenses) ----
    // Left eye
    px(36, 30, (Color){ 40, 32, 24, 255 });
    px(37, 30, (Color){ 40, 32, 24, 255 });
    px(36, 31, (Color){ 40, 32, 24, 255 });
    px(37, 31, (Color){ 40, 32, 24, 255 });
    px(36, 30, (Color){ 255, 255, 255, 180 }); // eye sparkle
    // Right eye
    px(56, 30, (Color){ 40, 32, 24, 255 });
    px(57, 30, (Color){ 40, 32, 24, 255 });
    px(56, 31, (Color){ 40, 32, 24, 255 });
    px(57, 31, (Color){ 40, 32, 24, 255 });
    px(56, 30, (Color){ 255, 255, 255, 180 });

    // ---- MOUTH (confident smile) ----
    px(45, 42, (Color){ 170, 92, 72, 255 });
    px(46, 42, (Color){ 170, 92, 72, 255 });
    px(47, 42, (Color){ 170, 92, 72, 255 });
    px(48, 42, (Color){ 170, 92, 72, 255 });
    px(49, 42, (Color){ 170, 92, 72, 255 });
    px(50, 42, (Color){ 170, 92, 72, 255 });
    px(51, 42, (Color){ 170, 92, 72, 255 });
    // Lower lip
    px(46, 43, (Color){ 200, 110, 90, 255 });
    px(47, 43, (Color){ 200, 110, 90, 255 });
    px(48, 43, (Color){ 200, 110, 90, 255 });
    px(49, 43, (Color){ 200, 110, 90, 255 });
    px(50, 43, (Color){ 200, 110, 90, 255 });
    // Teeth highlight
    px(47, 41, WHITE);
    px(48, 41, WHITE);
    px(49, 41, WHITE);

    // ---- BODY / GI ----
    // Black high collar
    for (int y = 52; y < 60; y++)
        for (int x = 34; x < 62; x++)
            px(x, y, (Color){ 26, 26, 26, 255 });
    // Collar V-neck shape
    for (int x = 40; x < 56; x++) px(x, 58, (Color){ 80, 45, 120, 255 });
    for (int x = 42; x < 54; x++) px(x, 59, (Color){ 80, 45, 120, 255 });

    // Gi top (purple)
    for (int y = 58; y < 90; y++)
        for (int x = 22; x < 74; x++)
            px(x, y, (Color){ 90, 46, 142, 255 });
    // Gi dark shading (left)
    for (int y = 60; y < 90; y++)
        for (int x = 22; x < 30; x++)
            px(x, y, (Color){ 65, 32, 104, 255 });
    // Gi dark shading (right)
    for (int y = 60; y < 90; y++)
        for (int x = 66; x < 74; x++)
            px(x, y, (Color){ 75, 38, 118, 255 });
    // Gi highlight (top center)
    for (int x = 40; x < 56; x++) px(x, 60, (Color){ 110, 60, 165, 255 });
    // Gi folds
    for (int y = 66; y < 88; y++) {
        px(36, y, (Color){ 75, 38, 118, 255 });
        px(58, y, (Color){ 75, 38, 118, 255 });
    }
    // Gi V-neck opening (shows black collar underneath)
    for (int x = 42; x < 54; x++) { px(x, 58, (Color){ 26, 26, 26, 255 }); px(x, 59, (Color){ 26, 26, 26, 255 }); }
    // Gi belt
    for (int y = 78; y < 82; y++)
        for (int x = 30; x < 66; x++)
            px(x, y, (Color){ 50, 35, 40, 255 });
    for (int x = 44; x < 52; x++) for (int y = 77; y < 83; y++) px(x, y, (Color){ 180, 155, 80, 255 });

    // ---- RIGHT HAND (thumb up) ----
    // Arm
    for (int y = 56; y < 70; y++)
        for (int x = 70; x < 80; x++)
            px(x, y, (Color){ 90, 46, 142, 255 });
    for (int y = 56; y < 70; y++)
        for (int x = 70; x < 73; x++)
            px(x, y, (Color){ 65, 32, 104, 255 });
    // Hand (palm up)
    for (int y = 66; y < 74; y++)
        for (int x = 76; x < 86; x++)
            px(x, y, (Color){ 230, 195, 158, 255 });
    // Hand shading
    for (int x = 76; x < 79; x++)
        for (int y = 66; y < 74; y++)
            px(x, y, (Color){ 195, 165, 135, 255 });
    // Thumb
    for (int y = 62; y < 68; y++)
        for (int x = 82; x < 88; x++)
            px(x, y, (Color){ 230, 195, 158, 255 });
    for (int y = 62; y < 68; y++)
        for (int x = 82; x < 84; x++)
            px(x, y, (Color){ 195, 165, 135, 255 });
    // Thumb nail
    px(86, 63, (Color){ 240, 215, 185, 255 });
    px(86, 64, (Color){ 240, 215, 185, 255 });
    // Fingers (curled under)
    for (int y = 72; y < 76; y++)
        for (int x = 78; x < 82; x++)
            px(x, y, (Color){ 215, 185, 150, 255 });
    for (int x = 82; x < 86; x++) px(x, 72, (Color){ 215, 185, 150, 255 });
    // Finger outline
    for (int x = 78; x < 82; x++) px(x, 75, (Color){ 50, 38, 32, 255 });
    px(85, 72, (Color){ 50, 38, 32, 255 });

    // ---- OUTLINE (face & body) ----
    // Face outline
    ImageDrawCircle(&img, 48, 34, 22, (Color){ 50, 38, 32, 255 });
    // Body outline
    for (int y = 58; y < 90; y++) { px(22, y, (Color){ 50, 38, 32, 255 }); px(73, y, (Color){ 50, 38, 32, 255 }); }
    for (int x = 22; x < 74; x++) { px(x, 88, (Color){ 50, 38, 32, 255 }); }
    // Arm outline
    for (int y = 56; y < 70; y++) { px(70, y, (Color){ 50, 38, 32, 255 }); px(79, y, (Color){ 50, 38, 32, 255 }); }

    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}
