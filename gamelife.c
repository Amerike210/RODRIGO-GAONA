#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define WIDTH 800
#define HEIGHT 450
#define CELL_SIZE 10
#define COLS (WIDTH / CELL_SIZE)
#define ROWS (HEIGHT / CELL_SIZE)

void ClearGrid(bool board[ROWS][COLS]) {
    for (int y = 0; y < ROWS; y++)
        for (int x = 0; x < COLS; x++)
            board[y][x] = false;
}

void RandomizeGrid(bool board[ROWS][COLS]) {
    for (int y = 0; y < ROWS; y++)
        for (int x = 0; x < COLS; x++)
            board[y][x] = (GetRandomValue(0, 4) == 0);
}

int CountNeighbors(bool board[ROWS][COLS], int x, int y) {
    int alive = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;
            int nx = x + dx, ny = y + dy;
            if (nx >= 0 && nx < COLS && ny >= 0 && ny < ROWS)
                alive += board[ny][nx];
        }
    }
    return alive;
}

void UpdateGrid(bool src[ROWS][COLS], bool dest[ROWS][COLS]) {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            int neighbors = CountNeighbors(src, x, y);
            if (src[y][x])
                dest[y][x] = (neighbors == 2 || neighbors == 3);
            else
                dest[y][x] = (neighbors == 3);
        }
    }
}

int main(void) {
    InitWindow(WIDTH, HEIGHT, "Conway - Juego de la Vida");
    SetTargetFPS(10);
    srand(time(NULL));

    bool board[ROWS][COLS] = { false };
    bool buffer[ROWS][COLS] = { false };
    bool simulate = false;

    const int infoFontSize = 10;
    const int infoPadding = 10;

    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_SPACE)) simulate = !simulate;
        if (IsKeyPressed(KEY_C)) ClearGrid(board);
        if (IsKeyPressed(KEY_R)) RandomizeGrid(board);

        if (!simulate) {
            Vector2 mouse = GetMousePosition();
            int cx = mouse.x / CELL_SIZE;
            int cy = mouse.y / CELL_SIZE;
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && cx < COLS && cy < ROWS)
                board[cy][cx] = true;
            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && cx < COLS && cy < ROWS)
                board[cy][cx] = false;
        }

        if (simulate) {
            UpdateGrid(board, buffer);
            for (int y = 0; y < ROWS; y++)
                for (int x = 0; x < COLS; x++)
                    board[y][x] = buffer[y][x];
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int y = 0; y < ROWS; y++) {
            for (int x = 0; x < COLS; x++) {
                Rectangle cell = { x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1 };
                if (board[y][x]) DrawRectangleRec(cell, RED);
                else DrawRectangleLines(cell.x, cell.y, CELL_SIZE, CELL_SIZE, LIGHTGRAY);
            }
        }


        DrawText("Juego de la Vida", 10, 10, 20, MAROON);
        DrawText(simulate ? "Ejecutando..." : "Pausado", 10, HEIGHT - 30, 20, simulate ? RED : GREEN);

        const char *info1 = "ESPACIO: Pausar/Reanudar";
        const char *info2 = "R: Aleatorio | C: Limpiar";
        const char *info3 = "Click: activar/desactivar celula";

        int text1Width = MeasureText(info1, infoFontSize);
        int text2Width = MeasureText(info2, infoFontSize);
        int text3Width = MeasureText(info3, infoFontSize);
        int maxWidth = text1Width;
        if (text2Width > maxWidth) maxWidth = text2Width;
        if (text3Width > maxWidth) maxWidth = text3Width;

        int baseY = HEIGHT - (infoFontSize * 3 + infoPadding * 2);

        DrawText(info1, WIDTH - maxWidth - infoPadding, baseY, infoFontSize, BLUE);
        DrawText(info2, WIDTH - maxWidth - infoPadding, baseY + infoFontSize + 5, infoFontSize, DARKGREEN);
        DrawText(info3, WIDTH - maxWidth - infoPadding, baseY + (infoFontSize + 5) * 2, infoFontSize, DARKPURPLE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
