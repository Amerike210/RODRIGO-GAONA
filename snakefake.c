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

typedef enum { NONE, DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT } Direction;

typedef struct {
    int x, y;
} Cell;

typedef struct {
    Cell cell;
    float velY;
    bool grounded;
} SnakeSegment;

static bool platformGrid[COLS][ROWS];
static SnakeSegment snake[MAX_SNAKE];
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

static float gravity = 0.25f;

static bool equalCell(Cell a, Cell b) { return a.x == b.x && a.y == b.y; }
static bool insideGrid(int x, int y) { return x >= 0 && x < COLS && y >= 0 && y < ROWS; }

void GeneratePlatforms(void) {
    for (int i = 0; i < COLS; i++)
        for (int j = 0; j < ROWS; j++)
            platformGrid[i][j] = false;

    // suelo
    for (int i = 0; i < COLS; i++)
        platformGrid[i][ROWS - 1] = true;

    int baseRow = ROWS - 6;
    for (int i = 5; i < 12; i++) platformGrid[i][baseRow] = true;
    for (int i = 18; i < 25; i++) platformGrid[i][baseRow - 2] = true;
    for (int i = 30; i < 36; i++) platformGrid[i][baseRow - 1] = true;
    for (int i = 12; i < 20; i++) platformGrid[i][baseRow + 2] = true;

    platformGrid[22][ROWS - 4] = true;
    platformGrid[23][ROWS - 4] = true;
}

bool IsOnPlatform(Cell c) {
    if (!insideGrid(c.x, c.y + 1)) return true;
    return platformGrid[c.x][c.y + 1];
}

bool IsBlocked(Cell c) {
    if (!insideGrid(c.x, c.y)) return true;
    return platformGrid[c.x][c.y];
}

void InitGame(void) {
    gameWin = false;
    applesEaten = 0;
    apple1Alive = apple2Alive = true;
    portalActive = false;

    GeneratePlatforms();

    snakeLen = 3;
    snakeDir = DIR_RIGHT;

    int startX = 7;
    int startY = 5;

    for (int i = 0; i < snakeLen; i++) {
        snake[i].cell.x = startX - i;
        snake[i].cell.y = startY;
        snake[i].velY = 0.0f;
        snake[i].grounded = false;
    }

    apple1 = (Cell){8, ROWS - 7};
    apple2 = (Cell){32, ROWS - 9};

    // bloque solido para el portal
    for (int y = ROWS - 2; y >= 0; y--) {
        for (int x = COLS - 2; x >= 0; x--) {
            if (platformGrid[x][y] && !platformGrid[x][y - 1]) {
                portal.x = x;
                portal.y = y - 1;
                goto foundPortal;
            }
        }
    }
foundPortal:;
    moveTimer = 0.0f;
}

// fisicas del cuerpo 
void UpdateSnakeBodyPhysics(void) {
    // gravedad
    for (int i = snakeLen - 1; i >= 0; i--) {
        SnakeSegment *seg = &snake[i];

        // si no se apoya
        bool onSnake = false;
        if (i < snakeLen - 1) {
            if (equalCell((Cell){seg->cell.x, seg->cell.y + 1}, snake[i + 1].cell)) {
                onSnake = true;
            }
        }

        if (!IsOnPlatform(seg->cell) && !onSnake) {
            seg->grounded = false;
            seg->velY += gravity;
            int fall = (int)seg->velY;

            for (int j = 0; j < fall; j++) {
                Cell next = seg->cell;
                next.y++;
                if (IsBlocked(next)) {
                    seg->grounded = true;
                    seg->velY = 0.0f;
                    break;
                }
                // si cae 
                for (int s = 0; s < snakeLen; s++) {
                    if (s != i && equalCell(next, snake[s].cell)) {
                        seg->grounded = true;
                        seg->velY = 0.0f;
                        goto grounded;
                    }
                }
                seg->cell.y++;
            }
        grounded:;
        } else {
            seg->grounded = true;
            seg->velY = 0.0f;
        }
    }
}

// movimiento
void MoveSnake(void) {
    UpdateSnakeBodyPhysics();

    // solo mover por cabeza apoyada 
    if (!snake[0].grounded) return;

    Cell head = snake[0].cell;
    Cell newHead = head;

    if (snakeDir == DIR_RIGHT) {
        newHead.x++;
        snakeDir = NONE;
    } else if (snakeDir == DIR_LEFT) {
        newHead.x--;
        snakeDir = NONE;
    } else if (snakeDir == DIR_UP) {
        newHead.y--;
        snakeDir = NONE;
    } else if (snakeDir == DIR_DOWN) {
        newHead.y++;
        snakeDir = NONE;
    }

    if (!insideGrid(newHead.x, newHead.y)) return;
    if (IsBlocked(newHead)) return;

    // colision 
    for (int i = 0; i < snakeLen; i++) {
        if (equalCell(snake[i].cell, newHead)) return;
    }

    // mover en cadena
    for (int i = snakeLen - 1; i > 0; i--) {
        snake[i].cell = snake[i - 1].cell;
    }
    snake[0].cell = newHead;

    // comer 
    if (apple1Alive && equalCell(newHead, apple1)) {
        apple1Alive = false;
        applesEaten++;
        if (snakeLen < MAX_SNAKE) {
            snake[snakeLen].cell = snake[snakeLen - 1].cell;
            snake[snakeLen].velY = 0.0f;
            snake[snakeLen].grounded = false;
            snakeLen++;
        }
    }
    if (apple2Alive && equalCell(newHead, apple2)) {
        apple2Alive = false;
        applesEaten++;
        if (snakeLen < MAX_SNAKE) {
            snake[snakeLen].cell = snake[snakeLen - 1].cell;
            snake[snakeLen].velY = 0.0f;
            snake[snakeLen].grounded = false;
            snakeLen++;
        }
    }

    if (applesEaten >= 2) portalActive = true;
    if (portalActive && equalCell(newHead, portal)) gameWin = true;
}

int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "Snakebird Raylib");
    SetTargetFPS(60);
    srand(time(NULL));
    InitGame();

    while (!WindowShouldClose()) {
        if (!gameOver && !gameWin) {
            if (IsKeyPressed(KEY_A)) snakeDir = DIR_LEFT;
            if (IsKeyPressed(KEY_D)) snakeDir = DIR_RIGHT;
            if (IsKeyPressed(KEY_W)) snakeDir = DIR_UP;
            if (IsKeyPressed(KEY_S)) snakeDir = DIR_DOWN;
        }

        if (IsKeyPressed(KEY_R)) InitGame();

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

        for (int x = 0; x < COLS; x++) {
            for (int y = 0; y < ROWS; y++) {
                if (platformGrid[x][y]) {
                    int px = x * CELL;
                    int py = y * CELL;
                    DrawRectangle(px, py + CELL/2, CELL, CELL/2, (Color){139,69,19,255});
                    DrawRectangle(px, py, CELL, CELL/2, (Color){34,139,34,255});
                }
            }
        }
        if (apple1Alive)
            DrawRectangle(apple1.x * CELL + 4, apple1.y * CELL + 4, CELL - 8, CELL - 8, RED);
        if (apple2Alive)
            DrawRectangle(apple2.x * CELL + 4, apple2.y * CELL + 4, CELL - 8, CELL - 8, RED);

        if (portalActive)
            DrawRectangle(portal.x * CELL + 2, portal.y * CELL + 2, CELL - 4, CELL - 4, GREEN);
        else
            DrawRectangleLines(portal.x * CELL + 2, portal.y * CELL + 2, CELL - 4, CELL - 4, (Color){0,128,0,60});
            
        for (int i = 0; i < snakeLen; i++) {
            int px = snake[i].cell.x * CELL;
            int py = snake[i].cell.y * CELL;
            Color color = (i == 0) ? (Color){30,144,255,255} : (Color){0,0,205,255};
            DrawRectangle(px + 1, py + 1, CELL - 2, CELL - 2, color);
            DrawRectangleLines(px + 1, py + 1, CELL - 2, CELL - 2, BLACK);
        }

        DrawText("WASD: mover, R: reiniciar", 10, 10, 14, BLACK);
        DrawText(TextFormat("Manzanas: %i/2", applesEaten), 10, 30, 14, BLACK);

        if (gameWin)
            DrawText("¡GANASTE! Portal verde desbloqueado.", 60, SCREEN_H/2 - 20, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
