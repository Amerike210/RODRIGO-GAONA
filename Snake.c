#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define SCREEN_W 800
#define SCREEN_H 600

#define CELL 20

#define COLS (SCREEN_W / CELL)
#define ROWS (SCREEN_H / CELL)

#define MAX_SNAKE 100

typedef enum { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT } Direction;

typedef struct {
    int x, y; 
} Cell;

static bool platformGrid[COLS][ROWS];
static Cell snake[MAX_SNAKE];
static int snakeLen;
static Direction snakeDir;
static float moveTimer = 0.0f;
static float moveDelay = 0.12f; 
static bool gameOver = false;
static bool gameWin = false;

static Cell apple1, apple2;
static int applesEaten = 0;
static bool apple1Alive = true, apple2Alive = true;

static Cell portal;
static bool portalActive = false;

static bool equalCell(Cell a, Cell b) { return a.x == b.x && a.y == b.y; }
static bool insideGrid(int x, int y) { return x >= 0 && x < COLS && y >= 0 && y < ROWS; }

void GeneratePlatforms(void) {
    for (int i = 0; i < COLS; i++){
        for (int j = 0; j < ROWS; j++){ 
            platformGrid[i][j] = false;
        }
    }

    for (int i = 0; i < COLS; i++) {
        platformGrid[i][ROWS - 1] = true;
        platformGrid[i][ROWS - 2] = true; 
    }

    int baseRow = ROWS - 6;
    for (int i = 5; i < 12; i++) platformGrid[i][baseRow] = true;
    for (int i = 18; i < 25; i++) platformGrid[i][baseRow - 2] = true;
    for (int i = 30; i < 36; i++) platformGrid[i][baseRow - 1] = true;
    for (int i = 12; i < 20; i++) platformGrid[i][baseRow + 2] = true;

    platformGrid[22][ROWS - 4] = true;
    platformGrid[23][ROWS - 4] = true; 
}

bool IsOnPlatform(Cell c) {
    if (!insideGrid(c.x, c.y)) return false;
    return platformGrid[c.x][c.y];
}

Cell PlaceApple(void) {
    Cell c;
    int attempts = 0;
    do {
        c.x = GetRandomValue(0, COLS - 1);
        c.y = GetRandomValue(0, ROWS - 1);
        attempts++;
        if (attempts > 10000) break;
    } while (!IsOnPlatform(c) || (equalCell(c, snake[0]))); 
    return c;
}

void InitGame(void) {
    gameOver = false;
    gameWin = false;
    applesEaten = 0;
    apple1Alive = apple2Alive = true;
    portalActive = false;

    GeneratePlatforms();

    snakeLen = 3;
    snakeDir = DIR_RIGHT;

    int startX = 2;
    int startY = ROWS - 3; 
    while (startY > 0 && !platformGrid[startX][startY]) startY--;
    for (int i = 0; i < snakeLen; i++) {
        snake[i].x = startX - i;
        snake[i].y = startY; }

    apple1 = PlaceApple();
    apple2 = PlaceApple();
    while (equalCell(apple2, apple1)) apple2 = PlaceApple();

    for (int y = 0; y < ROWS; y++) {
        for (int x = COLS - 1; x >= 0; x--) {
            if (platformGrid[x][y]) {
                portal.x = x;
                portal.y = y;
                goto foundPortal;
            }
        }
    }
foundPortal:;
    moveTimer = 0.0f;
}

void MoveSnake(void) {
    Cell head = snake[0];
    Cell newHead = head;
    if (snakeDir == DIR_RIGHT) newHead.x++;
    else if (snakeDir == DIR_LEFT) newHead.x--;
    else if (snakeDir == DIR_UP) newHead.y--;
    else if (snakeDir == DIR_DOWN) newHead.y++;

    if (!insideGrid(newHead.x, newHead.y)) {
        gameOver = true;
        return;
    }

    for (int i = snakeLen - 1; i > 0; i--) snake[i] = snake[i - 1];
    snake[0] = newHead;

    if (!IsOnPlatform(newHead)) {
        gameOver = true;
        return;
    }

    if (apple1Alive && equalCell(newHead, apple1)) {
        apple1Alive = false;
        applesEaten++;
        if (snakeLen <MAX_SNAKE) {
            snake[snakeLen] = snake[snakeLen - 1];
            snakeLen++;
        }
    } else if (apple2Alive && equalCell(newHead, apple2)) {
        apple2Alive = false;
 applesEaten++;
        if (snakeLen <MAX_SNAKE) {
            snake[snakeLen] = snake[snakeLen - 1];
            snakeLen++;
        }
    }

    if (applesEaten >= 2) portalActive = true;

    if (portalActive && equalCell(newHead, portal)) {
        gameWin = true;
    }
}

int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "SnakePajaro");
    SetTargetFPS(60);
    srand(time(NULL));

    InitGame();

    while (!WindowShouldClose()) {
        if (!gameOver && !gameWin) {
            if (IsKeyPressed(KEY_RIGHT) && snakeDir != DIR_LEFT) snakeDir = DIR_RIGHT;
            if (IsKeyPressed(KEY_LEFT) && snakeDir != DIR_RIGHT) snakeDir = DIR_LEFT;
            if (IsKeyPressed(KEY_UP) && snakeDir != DIR_DOWN) snakeDir = DIR_UP;
            if (IsKeyPressed(KEY_DOWN) && snakeDir != DIR_UP) snakeDir = DIR_DOWN;
        }

        if (IsKeyPressed(KEY_R)) {
            InitGame();
        }

        if (!gameOver && !gameWin) {
            moveTimer += GetFrameTime();
            if (moveTimer >= moveDelay) {
                MoveSnake();
                moveTimer = 0.0f;
            }
        }

        BeginDrawing();
        ClearBackground((Color){135, 206, 235, 255}); 

        DrawCircle(120, 80, 26, WHITE);
        DrawCircle(150, 80, 34, WHITE);
        DrawCircle(180, 80, 24, WHITE);
        DrawCircle(420, 60, 30, WHITE);
        DrawCircle(450, 60, 36, WHITE);
        DrawCircle(480, 60, 28, WHITE);
        DrawCircle(650, 100, 22, WHITE);
        DrawCircle(680, 100, 30, WHITE);

        for (int x = 0; x < COLS; x++) {
            for (int y = 0; y < ROWS; y++) {
                if (platformGrid[x][y]) {
                    int px = x * CELL;
                    int py = y * CELL;
                    DrawRectangle(px, py + CELL/2, CELL, CELL/2, (Color){139,69,19,255});
                    DrawRectangle(px, py, CELL, CELL/2, (Color){34,139,34,255});
                    DrawRectangleLines(px, py, CELL, CELL/2, (Color){0,100,0,255});
                }
            }
        }

        if (apple1Alive) {
            DrawRectangle(apple1.x * CELL + 4, apple1.y * CELL + 4, CELL - 8, CELL - 8, RED);

            DrawRectangle(apple1.x * CELL + CELL/2 - 1, apple1.y * CELL + 2, 2, 6, DARKGREEN);
        }
        if (apple2Alive) {
            DrawRectangle(apple2.x * CELL + 4, apple2.y * CELL + 4, CELL - 8, CELL - 8, RED);
            DrawRectangle(apple2.x * CELL + CELL/2 - 1, apple2.y * CELL + 2, 2, 6, DARKGREEN);
        }

        if (portalActive) {
            DrawRectangle(portal.x * CELL + 2, portal.y * CELL + 2, CELL - 4, CELL - 4, GREEN);

            DrawRectangle(portal.x * CELL + CELL/4, portal.y * CELL + CELL/4, CELL/2, CELL/2, (Color){0,200,0,255});
        } else {

            DrawRectangleLines(portal.x * CELL + 2, portal.y * CELL + 2, CELL - 4, CELL - 4, (Color){0,128,0,60});
        }

        for (int i = 0; i < snakeLen; i++) {
            int px = snake[i].x * CELL;
            int py = snake[i].y * CELL;
            if (i == 0) DrawRectangle(px + 1, py + 1, CELL - 2, CELL - 2, (Color){30,144,255,255}); 
            else DrawRectangle(px + 1, py + 1, CELL - 2, CELL - 2, (Color){0,0,205,255}); 
            DrawRectangleLines(px + 1, py + 1, CELL - 2, CELL - 2, BLACK);
        }

        DrawText("Flechas: mover. R: reiniciar.", 10, 10, 14, BLACK);
        DrawText(TextFormat("Manzanas comidas: %i / 2", applesEaten), 10, 30, 14, BLACK);

        if (gameOver) {
            DrawText("TE CAISTE! Presiona R para reiniciar.", 120, SCREEN_H/2 - 20, 20, RED);
        } else if (gameWin) {
            DrawText("GANASTE! Entraste al portal verde. Presiona R para jugar otra vez.", 60, SCREEN_H/2 - 20, 20, GREEN);
        } else {
            if (!portalActive) {
                DrawText("Come 2 manzanas para abrir el portal verde.", 10, 50, 14, DARKGRAY);
            } else {
                DrawText("Portal activo! Entra al portal verde para ganar.", 10, 50, 14, DARKGREEN); }
        }

        EndDrawing(); }

    CloseWindow();
    return 0;
}
