#include "raylib.h"
#include "rlgl.h"
#include "game.h"
#include "player.h"
#include "level1.h"
#include "level2.h"
#include "level3.h"
#include "tiles.h"
#include "utils.h"
#include <cstdlib>
#include <cmath>
#include <ctime>

typedef struct {
    float x, y;
    float speedX, speedY;
    bool active;
} PlayerProj;

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "David: El Camino a la Victoria");
    SetTargetFPS(60);
    srand((unsigned int)time(0));

    SetTextureFilter(GetFontDefault().texture, TEXTURE_FILTER_POINT);
    CreateTileTextures();

    InitAudioDevice();
    Music menuMusic = LoadMusicStream("assets/sounds/06 - For Argon!The Kingdom of Agatha - Chivalry 2 (Original Game Soundtrack).mp3");
    Music historiaMusic = LoadMusicStream("assets/sounds/Memories of Mother (God of War 2018 Soundtrack).mp3");
    Music dialogMusic = LoadMusicStream("assets/sounds/19 - The Slaughter of Coxwell - Chivalry 2 (Original Game Soundtrack).mp3");
    Music bossMusic = LoadMusicStream("assets/sounds/07 - Come Hither - Chivalry 2 (Original Game Soundtrack).mp3");
    Music victoriaMusic = LoadMusicStream("assets/sounds/golden brown slowed.mp3");
    Music defeatMusic = LoadMusicStream("assets/sounds/age-of-empires-2-the-conquerors-track-2_cTT3vmx7.mp3");
    Music level3Music = LoadMusicStream("assets/sounds/boss theme.ogg");
    PlayMusicStream(menuMusic);

    GameState state = MENU;
    Player player = CreatePlayer();
    Bush bushes[BUSH_COUNT];
    Stone stones[STONES_TOTAL];
    Goliath goliath = CreateGoliath();
    PlayerProj playerProjs[AMMO_MAX];
    Player jonathan = CreatePlayer();
    UnloadTexture(jonathan.sheet);
    jonathan.sheet = CreateJonathanTexture();
    jonathan.maxHp = JONATHAN_MAX_HP;
    jonathan.hp = JONATHAN_MAX_HP;
    Giant2 giant2 = CreateGiant2();
    Texture2D davidPortrait = CreateDavidPortrait();
    Texture2D youtubeImg = LoadTexture("assets/imagen1.png");
    Texture2D gatImg = LoadTexture("assets/gat.png");
    Texture2D cuevaImg = LoadTexture("assets/cueva.png");

    int menuSelection = 0;
    int historiaPage = 0;
    int victoriaPage = 0;
    int stonesCollected = 0;
    int score = 0;
    bool lvl1Dialog = false;
    int lvl1DialogPage = 0;
    float slingAngle = 0.0f;
    int nivel3Page = 0;
    float ataqueCooldown = 0.0f;
    float ataqueFlashTimer = 0.0f;
    int mapaPathIndex = 0;
    float mapaProgress = 0.0f;
    bool mapaArrived = false;
    int nextAfterMap = 0;

    bool transActive = false;
    float transAlpha = 0.0f;
    int transDir = 1;
    bool transToHistoria = false;
    bool transToNivel1 = false;
    bool transToLevel2 = false;
    bool transToVicHist = false;
    bool transToNivel3Intro = false;
    bool transToLevel3 = false;
    bool transToNivel3Outro = false;
    bool transToMapa = false;
    bool historiaTabActive = false;
    bool cuevaTabActive = false;

    for (int i = 0; i < AMMO_MAX; i++) playerProjs[i].active = false;

    while (!WindowShouldClose()) {
        UpdateMusicStream(menuMusic);
        UpdateMusicStream(historiaMusic);
        UpdateMusicStream(dialogMusic);
        UpdateMusicStream(bossMusic);
        UpdateMusicStream(victoriaMusic);
        UpdateMusicStream(defeatMusic);
        UpdateMusicStream(level3Music);
        float dt = GetFrameTime();

        if (transActive) {
            transAlpha += dt * 2.5f * transDir;
            if (transDir == 1 && transAlpha >= 1.0f) {
                transAlpha = 1.0f;
                transDir = -1;
                if (transToHistoria) {
                    StopMusicStream(level3Music);
                    PlayMusicStream(historiaMusic);
                    player = CreatePlayer();
                    InitBushes(bushes, BUSH_COUNT);
                    InitStones(stones, STONES_TOTAL, bushes, BUSH_COUNT);
                    goliath = CreateGoliath();
                    stonesCollected = 0;
                    score = 0;
                    historiaPage = 0;
                    for (int i = 0; i < AMMO_MAX; i++) playerProjs[i].active = false;
                    state = HISTORIA;
                    transToHistoria = false;
                } else if (transToNivel1) {
                    StopMusicStream(level3Music);
                    PlayMusicStream(dialogMusic);
                    lvl1Dialog = true;
                    lvl1DialogPage = 0;
                    historiaPage = 0;
                    state = NIVEL_1;
                    transToNivel1 = false;
                } else if (transToLevel2) {
                    StopMusicStream(dialogMusic);
                    PlayMusicStream(bossMusic);
                    player.hp = player.maxHp;
                    player.ammo = AMMO_MAX;
                    goliath = CreateGoliath();
                    state = NIVEL_2;
                    transToLevel2 = false;
                } else if (transToVicHist) {
                    StopMusicStream(bossMusic);
                    PlayMusicStream(victoriaMusic);
                    victoriaPage = 0;
                    state = VICTORIA_HISTORIA;
                    transToVicHist = false;
                } else if (transToNivel3Intro) {
                    StopMusicStream(victoriaMusic);
                    PlayMusicStream(level3Music);
                    nivel3Page = 0;
                    state = NIVEL_3_INTRO;
                    transToNivel3Intro = false;
                } else if (transToLevel3) {
                    UnloadTexture(jonathan.sheet);
                    jonathan = CreatePlayer();
                    UnloadTexture(jonathan.sheet);
                    jonathan.sheet = CreateJonathanTexture();
                    jonathan.maxHp = JONATHAN_MAX_HP;
                    jonathan.hp = JONATHAN_MAX_HP;
                    giant2 = CreateGiant2();
                    ataqueCooldown = 0.0f;
                    ataqueFlashTimer = 0.0f;
                    state = NIVEL_3;
                    transToLevel3 = false;
                } else if (transToNivel3Outro) {
                    nivel3Page = 0;
                    state = NIVEL_3_OUTRO;
                    transToNivel3Outro = false;
                } else if (transToMapa) {
                    mapaProgress = 0.0f;
                    mapaArrived = false;
                    state = MAPA_MUNDI;
                    transToMapa = false;
                }
            }
            if (transDir == -1 && transAlpha <= 0.0f) {
                transAlpha = 0.0f;
                transActive = false;
            }
        }

        switch (state) {
            case MENU:
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                    menuSelection--;
                    if (menuSelection < 0) menuSelection = 2;
                }
                if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                    menuSelection++;
                    if (menuSelection > 2) menuSelection = 0;
                }
                if (IsKeyPressed(KEY_ENTER)) {
                    if (menuSelection == 0) {
                        StopMusicStream(menuMusic);
                        PlayMusicStream(level3Music);
                        mapaPathIndex = 0;
                        nextAfterMap = 0;
                        mapaProgress = 0.0f;
                        mapaArrived = false;
                        state = MAPA_MUNDI;
                    } else if (menuSelection == 1) {
                        state = INSTRUCCIONES;
                    } else if (menuSelection == 2) {
                        CloseWindow();
                    }
                }
                break;

            case HISTORIA:
                if (IsKeyPressed(KEY_ENTER)) {
                    historiaPage++;
                    if (historiaPage >= 5) {
                        transActive = true;
                        transDir = 1;
                        transAlpha = 0.0f;
                        transToNivel1 = true;
                    }
                }
                break;

            case INSTRUCCIONES:
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
                    state = MENU;
                }
                break;

            case NIVEL_1: {
                if (lvl1Dialog) {
                    if (IsKeyPressed(KEY_ENTER)) {
                        lvl1DialogPage++;
                        if (lvl1DialogPage >= 3) lvl1Dialog = false;
                    }
                    break;
                }
                float prevX = player.x, prevY = player.y;
                UpdatePlayer(player);

                for (int i = 0; i < BUSH_COUNT; i++) {
                    Rectangle pr = { player.x - PLAYER_COL_W/2, player.y - PLAYER_COL_H/2, PLAYER_COL_W, PLAYER_COL_H };
                    Rectangle br = { bushes[i].x - 15, bushes[i].y - 15, 30, 30 };
                    if (CheckCollisionRecs(pr, br)) {
                        player.x = prevX;
                        player.y = prevY;
                    }
                }

                for (int i = 0; i < STONES_TOTAL; i++) {
                    if (!stones[i].collected) {
                        Rectangle pr = { player.x - PLAYER_COL_W/2, player.y - PLAYER_COL_H/2, PLAYER_COL_W, PLAYER_COL_H };
                        Rectangle sr = { stones[i].x - 16, stones[i].y - 16, 32, 32 };
                        if (CheckCollisionRecs(pr, sr)) {
                            stones[i].collected = true;
                            stonesCollected++;
                            score += 100;
                        }
                    }
                }

                if (stonesCollected >= STONES_NEEDED) {
                    PlayMusicStream(dialogMusic);
                    state = NIVEL_1_COMPLETO;
                }
                break;
            }

            case NIVEL_2: {
                UpdatePlayer(player);
                UpdateGoliath(goliath, player.x, player.y, dt);
                slingAngle += dt * 360.0f;

                if (player.x > SCREEN_WIDTH - 200) player.x = (float)SCREEN_WIDTH - 200;

                Rectangle pr = { player.x - PLAYER_COL_W/2, player.y - PLAYER_COL_H/2, PLAYER_COL_W, PLAYER_COL_H };
                Rectangle gr = { goliath.x - 25, goliath.y - 30, 50, 60 };
                if (CheckCollisionRecs(pr, gr) && player.invulnTimer <= 0) {
                    player.hp--;
                    player.invulnTimer = 1.5f;
                    float pushDir = (player.x - goliath.x < 0) ? -40.0f : 40.0f;
                    player.x += pushDir;
                    if (player.hp <= 0) { StopMusicStream(bossMusic); PlayMusicStream(defeatMusic); state = DERROTA; break; }
                }

                if (IsKeyPressed(KEY_SPACE) && player.ammo > 0 && player.shootCooldown <= 0) {
                    player.ammo--;
                    player.shootCooldown = 0.4f;
                    for (int i = 0; i < AMMO_MAX; i++) {
                        if (!playerProjs[i].active) {
                            playerProjs[i].x = player.x;
                            playerProjs[i].y = player.y;
                            float mx = (float)GetMouseX();
                            float my = (float)GetMouseY();
                            float dx = mx - player.x;
                            float dy = my - player.y;
                            float dist = sqrtf(dx * dx + dy * dy);
                            if (dist > 0) {
                                playerProjs[i].speedX = (dx / dist) * PLAYER_PROJ_SPEED;
                                playerProjs[i].speedY = (dy / dist) * PLAYER_PROJ_SPEED;
                            }
                            playerProjs[i].active = true;
                            break;
                        }
                    }
                }

                for (int i = 0; i < AMMO_MAX; i++) {
                    if (playerProjs[i].active) {
                        playerProjs[i].x += playerProjs[i].speedX;
                        playerProjs[i].y += playerProjs[i].speedY;

                        if (playerProjs[i].x < 0 || playerProjs[i].x > SCREEN_WIDTH ||
                            playerProjs[i].y < 0 || playerProjs[i].y > SCREEN_HEIGHT) {
                            playerProjs[i].active = false;
                            continue;
                        }

                        Rectangle psr = { playerProjs[i].x - 6, playerProjs[i].y - 6, 12, 12 };
                        if (CheckCollisionRecs(psr, gr)) {
                            playerProjs[i].active = false;
                            goliath.hp--;
                            if (goliath.hp <= 0) {
                                score += 500;
                                state = VICTORIA;
                            }
                            break;
                        }
                    }
                }

                for (int i = 0; i < GOLIATH_PROJ_MAX; i++) {
                    if (goliath.projectiles[i].active) {
                        Rectangle gpr = { goliath.projectiles[i].x - 5, goliath.projectiles[i].y - 5, 10, 10 };
                        if (CheckCollisionRecs(gpr, pr) && player.invulnTimer <= 0) {
                            player.hp--;
                            player.invulnTimer = 1.5f;
                            goliath.projectiles[i].active = false;
                            if (player.hp <= 0) { StopMusicStream(bossMusic); PlayMusicStream(defeatMusic); state = DERROTA; break; }
                        }
                    }
                }

                if (state == NIVEL_2 && player.ammo <= 0) {
                    bool anyActive = false;
                    for (int i = 0; i < AMMO_MAX; i++)
                        if (playerProjs[i].active) { anyActive = true; break; }
                    if (!anyActive) { StopMusicStream(bossMusic); PlayMusicStream(defeatMusic); state = DERROTA; }
                }
                break;
            }

            case NIVEL_1_COMPLETO:
                if (IsKeyPressed(KEY_ENTER)) {
                    transActive = true;
                    transDir = 1;
                    transAlpha = 0.0f;
                    mapaPathIndex = 0;
                    nextAfterMap = 2;
                    transToMapa = true;
                }
                break;

            case VICTORIA:
                if (IsKeyPressed(KEY_ENTER)) {
                    transActive = true;
                    transDir = 1;
                    transAlpha = 0.0f;
                    transToVicHist = true;
                }
                break;

            case VICTORIA_HISTORIA:
                if (IsKeyPressed(KEY_ENTER)) {
                    victoriaPage++;
                    if (victoriaPage >= 4) {
                        transActive = true;
                        transDir = 1;
                        transAlpha = 0.0f;
                        mapaPathIndex = 1;
                        nextAfterMap = 3;
                        transToMapa = true;
                    }
                }
                break;

            case MAPA_MUNDI:
                if (historiaTabActive) {
                    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
                        historiaTabActive = false;
                    }
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        float mx = (float)GetMouseX(), my = (float)GetMouseY();
                        Rectangle linkRect = { SCREEN_WIDTH / 2 - 166, 480, 380, 16 };
                        if (CheckCollisionPointRec((Vector2){mx, my}, linkRect)) {
                            system("start \"\" \"https://www.youtube.com/watch?v=k-heTusuUx0\"");
                        } else {
                            historiaTabActive = false;
                        }
                    }
                    break;
                }
                if (cuevaTabActive) {
                    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
                        cuevaTabActive = false;
                    }
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        float mx = (float)GetMouseX(), my = (float)GetMouseY();
                        Rectangle cLinkRect = { SCREEN_WIDTH / 2 - 166, 480, 380, 16 };
                        if (CheckCollisionPointRec((Vector2){mx, my}, cLinkRect)) {
                            system("start \"\" \"https://www.youtube.com/watch?v=-lWtpuh4XWo\"");
                        } else {
                            cuevaTabActive = false;
                        }
                    }
                    break;
                }
                if (!mapaArrived) {
                    mapaProgress += dt * 0.35f;
                    if (mapaProgress >= 1.0f) {
                        mapaProgress = 1.0f;
                        mapaArrived = true;
                    }
                } else if (IsKeyPressed(KEY_ENTER)) {
                    if (nextAfterMap == 0) {
                        if (mapaPathIndex == 0) {
                            mapaPathIndex = 1;
                            mapaProgress = 0.0f;
                            mapaArrived = false;
                        } else {
                            transActive = true;
                            transDir = 1;
                            transAlpha = 0.0f;
                            transToHistoria = true;
                        }
                    } else {
                        transActive = true;
                        transDir = 1;
                        transAlpha = 0.0f;
                        if (nextAfterMap == 1) {
                            transToNivel1 = true;
                        } else if (nextAfterMap == 2) {
                            transToLevel2 = true;
                        } else {
                            transToNivel3Intro = true;
                        }
                    }
                }
                if (mapaArrived && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    float mx = (float)GetMouseX(), my = (float)GetMouseY();
                    Rectangle imgRect = { 150, 150, 150, 150 };
                    if (CheckCollisionPointRec((Vector2){mx, my}, imgRect)) {
                        historiaTabActive = true;
                    }
                    Rectangle cuevaRect = { 425, 350, 100, 86 };
                    if (CheckCollisionPointRec((Vector2){mx, my}, cuevaRect)) {
                        cuevaTabActive = true;
                    }
                }
                break;

            case NIVEL_3_INTRO:
                if (IsKeyPressed(KEY_ENTER)) {
                    nivel3Page++;
                    if (nivel3Page >= 4) {
                        transActive = true;
                        transDir = 1;
                        transAlpha = 0.0f;
                        transToLevel3 = true;
                    }
                }
                break;

            case NIVEL_3: {
                ataqueCooldown -= dt;
                if (ataqueFlashTimer > 0) ataqueFlashTimer -= dt;

                float prevX = jonathan.x, prevY = jonathan.y;
                UpdatePlayer(jonathan);

                jonathan.x = (jonathan.x < 20) ? 20 : (jonathan.x > SCREEN_WIDTH - 20) ? SCREEN_WIDTH - 20 : jonathan.x;
                jonathan.y = (jonathan.y < 40) ? 40 : (jonathan.y > SCREEN_HEIGHT - 20) ? SCREEN_HEIGHT - 20 : jonathan.y;
                if (jonathan.x > SCREEN_WIDTH - 200) jonathan.x = (float)SCREEN_WIDTH - 200;

                UpdateGiant2(giant2, jonathan.x, jonathan.y, dt);

                Rectangle jr = { jonathan.x - PLAYER_COL_W/2, jonathan.y - PLAYER_COL_H/2, PLAYER_COL_W, PLAYER_COL_H };
                Rectangle gr = { giant2.x - 28, giant2.y - 30, 56, 60 };

                if (CheckCollisionRecs(jr, gr) && jonathan.invulnTimer <= 0) {
                    jonathan.hp--;
                    jonathan.invulnTimer = 1.5f;
                    float pushDir = (jonathan.x - giant2.x < 0) ? -50.0f : 50.0f;
                    jonathan.x += pushDir;
                    if (jonathan.hp <= 0) {
                        StopMusicStream(level3Music);
                        PlayMusicStream(defeatMusic);
                        state = DERROTA;
                        break;
                    }
                }

                if (IsKeyPressed(KEY_SPACE) && ataqueCooldown <= 0) {
                    float dist = sqrtf((jonathan.x - giant2.x) * (jonathan.x - giant2.x) + (jonathan.y - giant2.y) * (jonathan.y - giant2.y));
                    if (dist < 55.0f) {
                        giant2.hp--;
                        ataqueCooldown = 0.5f;
                        ataqueFlashTimer = 0.15f;
                        score += 200;
                        if (giant2.hp <= 0) {
                            score += 500;
                            state = NIVEL_3_VICTORIA;
                            break;
                        }
                    }
                }

                for (int i = 0; i < GIANT2_PROJ_MAX; i++) {
                    if (giant2.projectiles[i].active) {
                        Rectangle gpr = { giant2.projectiles[i].x - 5, giant2.projectiles[i].y - 5, 10, 10 };
                        if (CheckCollisionRecs(gpr, jr) && jonathan.invulnTimer <= 0) {
                            jonathan.hp--;
                            jonathan.invulnTimer = 1.5f;
                            giant2.projectiles[i].active = false;
                            if (jonathan.hp <= 0) {
                                StopMusicStream(level3Music);
                                PlayMusicStream(defeatMusic);
                                state = DERROTA;
                                break;
                            }
                        }
                    }
                }
                break;
            }

            case NIVEL_3_VICTORIA:
                if (IsKeyPressed(KEY_ENTER)) {
                    transActive = true;
                    transDir = 1;
                    transAlpha = 0.0f;
                    transToNivel3Outro = true;
                }
                break;

            case NIVEL_3_OUTRO:
                if (IsKeyPressed(KEY_ENTER)) {
                    nivel3Page++;
                    if (nivel3Page >= 3) {
                        StopMusicStream(level3Music);
                        PlayMusicStream(menuMusic);
                        state = MENU;
                    }
                }
                break;

            case DERROTA:
                if (IsKeyPressed(KEY_ENTER)) {
                    StopMusicStream(historiaMusic);
                    StopMusicStream(bossMusic);
                    StopMusicStream(defeatMusic);
                    StopMusicStream(level3Music);
                    PlayMusicStream(menuMusic);
                    state = MENU;
                }
                break;
        }

        BeginDrawing();
        ClearBackground((Color){ 50, 60, 80, 255 });

        switch (state) {
            case MENU:
                DrawMenu(menuSelection);
                break;

            case HISTORIA:
                DrawHistoria(historiaPage, 6);
                break;

            case INSTRUCCIONES:
                DrawInstrucciones();
                break;

            case NIVEL_1_COMPLETO:
                DrawNivel1Completo(score);
                break;

            case NIVEL_1:
                DrawLevel1Background();
                DrawBushes(bushes, BUSH_COUNT);
                DrawStones(stones, STONES_TOTAL);
                DrawPlayer(player);
                DrawHUDLevel1(stonesCollected, player.hp, player.maxHp, score);
                DrawText("Junta las 5 piedras para enfrentar a Goliat", SCREEN_WIDTH / 2 - 170, SCREEN_HEIGHT - 22, 14, (Color){ 245, 240, 230, 200 });

                if (lvl1Dialog) {
                    DrawRectangle(2, DIALOG_Y, SCREEN_WIDTH - 4, 3, (Color){ 67, 54, 49, 255 });
                    DrawRectangle(2, DIALOG_Y + DIALOG_H - 3, SCREEN_WIDTH - 4, 3, (Color){ 67, 54, 49, 255 });
                    DrawRectangle(0, DIALOG_Y + 3, 3, DIALOG_H - 6, (Color){ 67, 54, 49, 255 });
                    DrawRectangle(SCREEN_WIDTH - 3, DIALOG_Y + 3, 3, DIALOG_H - 6, (Color){ 67, 54, 49, 255 });
                    DrawRectangle(3, DIALOG_Y + 3, SCREEN_WIDTH - 6, 1, (Color){ 214, 181, 95, 255 });
                    DrawRectangle(3, DIALOG_Y + DIALOG_H - 4, SCREEN_WIDTH - 6, 1, (Color){ 214, 181, 95, 255 });
                    DrawRectangle(3, DIALOG_Y + 4, 1, DIALOG_H - 8, (Color){ 214, 181, 95, 255 });
                    DrawRectangle(SCREEN_WIDTH - 4, DIALOG_Y + 4, 1, DIALOG_H - 8, (Color){ 214, 181, 95, 255 });
                    DrawRectangle(4, DIALOG_Y + 4, SCREEN_WIDTH - 8, DIALOG_H - 8, (Color){ 255, 247, 231, 255 });
                    const char* l1text[3];
                    l1text[0] = "David ha llegado al arroyo de Belen.\nDebe encontrar piedras lisas para su honda.";
                    l1text[1] = "Usa las teclas WASD o las flechas del\nteclado para moverte por el campo.";
                    l1text[2] = "Junta 5 piedras para poder enfrentar a Goliat.\nPresiona ENTER para comenzar.";
                    DrawText(l1text[lvl1DialogPage], 14, DIALOG_Y + 18, 26, (Color){ 50, 40, 36, 255 });
                    DrawText("Presiona ENTER para continuar", 14, DIALOG_Y + DIALOG_H - 28, 16, (Color){ 214, 181, 95, 255 });
                    if ((int)(GetTime() * 3) % 2 == 0)
                        DrawText("▼", SCREEN_WIDTH - 28, DIALOG_Y + DIALOG_H - 30, 20, (Color){ 50, 40, 36, 255 });
                }
                break;

            case NIVEL_2: {
                DrawLevel2Background();

                // Sling
                rlPushMatrix();
                rlTranslatef(player.x, player.y, 0);
                rlRotatef(slingAngle, 0, 0, 1);
                int sl = 14;
                utils::DrawLineDDA(0, 0, (float)sl, 0, (Color){ 150, 105, 50, 255 });
                utils::DrawLineDDA((float)sl, 0, (float)sl + 3, -3, (Color){ 150, 105, 50, 255 });
                utils::DrawLineDDA((float)sl, 0, (float)sl + 3, 3, (Color){ 150, 105, 50, 255 });
                rlPopMatrix();

                int mx = GetMouseX(), my = GetMouseY();
                utils::DrawLineDDA(player.x, player.y, (float)mx, (float)my, Fade(WHITE, 0.3f));
                utils::DrawCircleMidpoint(mx, my, 6, Fade(RED, 0.6f));
                utils::DrawLineDDA((float)mx - 8, (float)my, (float)mx + 8, (float)my, Fade(RED, 0.5f));
                utils::DrawLineDDA((float)mx, (float)my - 8, (float)mx, (float)my + 8, Fade(RED, 0.5f));

                DrawPlayer(player);
                DrawGoliath(goliath);

                for (int i = 0; i < AMMO_MAX; i++) {
                    if (playerProjs[i].active) {
                        utils::DrawCircleMidpointFill((int)playerProjs[i].x, (int)playerProjs[i].y, 5, (Color){ 100, 80, 160, 255 });
                        utils::DrawCircleMidpoint((int)playerProjs[i].x, (int)playerProjs[i].y, 5, (Color){ 40, 30, 20, 255 });
                    }
                }

                DrawHUDLevel2(player.ammo, player.hp, player.maxHp, score);
                DrawText("ESPACIO para disparar | Apunta con el mouse",
                    SCREEN_WIDTH / 2 - 170, SCREEN_HEIGHT - 22, 14, (Color){ 245, 240, 230, 200 });
                break;
            }

            case VICTORIA:
                DrawVictory(score);
                break;

            case VICTORIA_HISTORIA:
                DrawVictoriaHistoria(victoriaPage, 4);
                break;

            case MAPA_MUNDI:
                DrawMapaMundi(mapaPathIndex, mapaProgress, mapaArrived);
                if (youtubeImg.id > 0) {
                    Rectangle srcRect = { 0, 0, (float)youtubeImg.width, (float)youtubeImg.height };
                    Rectangle dstRect = { 150, 150, 150, 150 };
                    DrawTexturePro(youtubeImg, srcRect, dstRect, (Vector2){ 0, 0 }, 0, WHITE);
                }
                if (cuevaImg.id > 0) {
                    Rectangle cSrc = { 0, 0, (float)cuevaImg.width, (float)cuevaImg.height };
                    Rectangle cDst = { 425, 350, 100, 86 };
                    DrawTexturePro(cuevaImg, cSrc, cDst, (Vector2){ 0, 0 }, 0, WHITE);
                }
                if (historiaTabActive) {
                    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 200 });
                    DrawRectangle(85, 100, SCREEN_WIDTH - 170, SCREEN_HEIGHT - 200, (Color){ 255, 247, 231, 255 });
                    DrawRectangle(85, 100, SCREEN_WIDTH - 170, 3, (Color){ 214, 181, 95, 255 });
                    DrawText("GAT: LA TIERRA DE GIGANTES", SCREEN_WIDTH / 2 - MeasureText("GAT: LA TIERRA DE GIGANTES", 24) / 2, 120, 24, (Color){ 50, 40, 36, 255 });
                    DrawText("Según las recientes excavaciones arqueológicas, esta imponente", SCREEN_WIDTH / 2 - 290, 155, 18, (Color){ 50, 40, 36, 255 });
                    DrawText("fortaleza era cinco veces más grande que la Jerusalén de su época.", SCREEN_WIDTH / 2 - 290, 175, 18, (Color){ 50, 40, 36, 255 });
                    DrawText("Gat fue temida y conocida como la \"tierra de gigantes\", hogar de los", SCREEN_WIDTH / 2 - 290, 195, 18, (Color){ 50, 40, 36, 255 });
                    DrawText("descendientes de los Anaquim y Refaitas. Aquí es donde nació y creció", SCREEN_WIDTH / 2 - 290, 215, 18, (Color){ 50, 40, 36, 255 });
                    DrawText("Goliat, el mítico campeón cuya colosal armadura infundía terror,", SCREEN_WIDTH / 2 - 290, 235, 18, (Color){ 50, 40, 36, 255 });
                    DrawText("antes de enfrentar su destino ante David.", SCREEN_WIDTH / 2 - 290, 255, 18, (Color){ 50, 40, 36, 255 });
                    DrawText("EVIDENCIA ARQUEOLOGICA", SCREEN_WIDTH / 2 - MeasureText("EVIDENCIA ARQUEOLOGICA", 18) / 2, 300, 18, (Color){ 214, 181, 95, 255 });
                    if (gatImg.id > 0) {
                        Rectangle gSrc = { 0, 0, (float)gatImg.width, (float)gatImg.height };
                        Rectangle gDst = { SCREEN_WIDTH / 2 - 75, 325, 150, 150 };
                        DrawTexturePro(gatImg, gSrc, gDst, (Vector2){ 0, 0 }, 0, WHITE);
                    }
                    DrawText("https://www.youtube.com/watch?v=k-heTusuUx0", SCREEN_WIDTH / 2 - 166, 480, 14, (Color){ 60, 100, 200, 255 });
                    DrawText("Presiona ENTER o ESC para cerrar", SCREEN_WIDTH / 2 - MeasureText("Presiona ENTER o ESC para cerrar", 14) / 2, 520, 14, (Color){ 214, 181, 95, 255 });
                }
                if (cuevaTabActive) {
                    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 200 });
                    DrawRectangle(85, 100, SCREEN_WIDTH - 170, SCREEN_HEIGHT - 200, (Color){ 255, 247, 231, 255 });
                    DrawRectangle(85, 100, SCREEN_WIDTH - 170, 3, (Color){ 214, 181, 95, 255 });
                    DrawText("Cueva de Adulam: El Refugio de David", SCREEN_WIDTH / 2 - MeasureText("Cueva de Adulam: El Refugio de David", 24) / 2, 120, 24, (Color){ 50, 40, 36, 255 });
                    DrawText("\"Yéndose luego David de allí, huyó a la cueva", SCREEN_WIDTH / 2 - MeasureText("\"Yéndose luego David de allí, huyó a la cueva", 18) / 2, 180, 18, (Color){ 50, 40, 36, 255 });
                    DrawText("de Adulam; y cuando sus hermanos y toda la", SCREEN_WIDTH / 2 - MeasureText("de Adulam; y cuando sus hermanos y toda la", 18) / 2, 200, 18, (Color){ 50, 40, 36, 255 });
                    DrawText("casa de su padre lo supieron, vinieron allí a él.\"", SCREEN_WIDTH / 2 - MeasureText("casa de su padre lo supieron, vinieron allí a él.\"", 18) / 2, 220, 18, (Color){ 50, 40, 36, 255 });
                    DrawText("(1 Samuel 22:1, RVR1960)", SCREEN_WIDTH / 2 - MeasureText("(1 Samuel 22:1, RVR1960)", 16) / 2, 248, 16, (Color){ 100, 80, 70, 255 });
                    DrawText("EVIDENCIA ARQUEOLOGICA", SCREEN_WIDTH / 2 - MeasureText("EVIDENCIA ARQUEOLOGICA", 18) / 2, 300, 18, (Color){ 214, 181, 95, 255 });
                    if (cuevaImg.id > 0) {
                        Rectangle cvSrc = { 0, 0, (float)cuevaImg.width, (float)cuevaImg.height };
                        Rectangle cvDst = { SCREEN_WIDTH / 2 - 75, 325, 150, 150 };
                        DrawTexturePro(cuevaImg, cvSrc, cvDst, (Vector2){ 0, 0 }, 0, WHITE);
                    }
                    DrawText("https://www.youtube.com/watch?v=-lWtpuh4XWo", SCREEN_WIDTH / 2 - 166, 480, 14, (Color){ 60, 100, 200, 255 });
                    DrawText("Presiona ENTER o ESC para cerrar", SCREEN_WIDTH / 2 - MeasureText("Presiona ENTER o ESC para cerrar", 14) / 2, 520, 14, (Color){ 214, 181, 95, 255 });
                }
                break;

            case NIVEL_3_INTRO:
                DrawNivel3Intro(nivel3Page, 4);
                break;

            case NIVEL_3: {
                DrawLevel3Background();

                DrawPlayer(jonathan);

                if (ataqueFlashTimer > 0) {
                    float swordX = jonathan.x + ((jonathan.dir == DIR_RIGHT) ? 20 : (jonathan.dir == DIR_LEFT) ? -20 : 0);
                    float swordY = jonathan.y + ((jonathan.dir == DIR_DOWN) ? 10 : (jonathan.dir == DIR_UP) ? -10 : 0);
                    utils::DrawCircleMidpointFill((int)swordX, (int)swordY, 14, (Color){ 255, 255, 220, 120 });
                    utils::DrawCircleMidpoint((int)swordX, (int)swordY, 14, (Color){ 255, 210, 50, 200 });
                    DrawLine((int)jonathan.x, (int)jonathan.y, (int)swordX, (int)swordY, (Color){ 255, 255, 220, 100 });
                }

                DrawGiant2(giant2);

                DrawHUDLevel3(jonathan.hp, jonathan.maxHp);
                DrawText("Presiona ESPACIO para atacar con la espada",
                    SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT - 22, 14, (Color){ 245, 240, 230, 200 });
                break;
            }

            case NIVEL_3_VICTORIA:
                DrawNivel3Victory(score);
                break;

            case NIVEL_3_OUTRO:
                DrawNivel3Outro(nivel3Page, 3);
                break;

            case DERROTA:
                DrawGameOver(score);
                break;
        }

        if (transActive || transAlpha > 0.0f) {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, (unsigned char)(transAlpha * 255) });
        }

        EndDrawing();
    }

    UnloadMusicStream(menuMusic);
    UnloadMusicStream(historiaMusic);
    UnloadMusicStream(dialogMusic);
    UnloadMusicStream(bossMusic);
    UnloadMusicStream(victoriaMusic);
    UnloadMusicStream(defeatMusic);
    UnloadMusicStream(level3Music);
    CloseAudioDevice();
    UnloadPlayer(player);
    UnloadTexture(jonathan.sheet);
    UnloadTileTextures();
    UnloadMapTerrain();
    if (davidPortrait.id > 0) UnloadTexture(davidPortrait);
    if (youtubeImg.id > 0) UnloadTexture(youtubeImg);
    if (gatImg.id > 0) UnloadTexture(gatImg);
    if (cuevaImg.id > 0) UnloadTexture(cuevaImg);
    CloseWindow();
    return 0;
}
