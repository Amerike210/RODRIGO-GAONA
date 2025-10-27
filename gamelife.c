#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Juego de la Vida");
    SetTargetFPS(10); 

    const int cols = 80;
    const int rows = 45;
    const int cellSize = 10;

    bool grid[45][80] = { false };
    bool next[45][80] = { false };

    bool running = false;

    srand(time(NULL));

    while (!WindowShouldClose())
    {
        // --- INPUT / CONTROLES ---
        if (IsKeyPressed(KEY_SPACE)) running = !running; 
        if (IsKeyPressed(KEY_C)) {                       
            for (int y = 0; y < rows; y++)
                for (int x = 0; x < cols; x++)
                    grid[y][x] = false;
        }
        if (IsKeyPressed(KEY_R)) {                       
            for (int y = 0; y < rows; y++)
                for (int x = 0; x < cols; x++)
                    grid[y][x] = GetRandomValue(0, 4) == 0; 
        }

        if (!running && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mouse = GetMousePosition();
            int x = mouse.x / cellSize;
            int y = mouse.y / cellSize;
            if (x >= 0 && x < cols && y >= 0 && y < rows)
                grid[y][x] = true;
        }
        if (!running && IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            Vector2 mouse = GetMousePosition();
            int x = mouse.x / cellSize;
            int y = mouse.y / cellSize;
            if (x >= 0 && x < cols && y >= 0 && y < rows)
                grid[y][x] = false;
        }

        if (running)
        {
            for (int y = 0; y < rows; y++)
            {
                for (int x = 0; x < cols; x++)
                {
                    int alive = 0;

                    for (int dy = -1; dy <= 1; dy++)
                    {
                        for (int dx = -1; dx <= 1; dx++)
                        {
                            if (dx == 0 && dy == 0) continue;
                            int nx = x + dx;
                            int ny = y + dy;

                            if (nx >= 0 && nx < cols && ny >= 0 && ny < rows)
                                if (grid[ny][nx]) alive++;
                        }
                    }

                    if (grid[y][x] && (alive == 2 || alive == 3))
                        next[y][x] = true;
                    else if (!grid[y][x] && alive == 3)
                        next[y][x] = true;
                    else
                        next[y][x] = false;
                }
            }

            for (int y = 0; y < rows; y++)
                for (int x = 0; x < cols; x++)
                    grid[y][x] = next[y][x];
        }
        BeginDrawing();
            ClearBackground(DARKGRAY);

            for (int y = 0; y < rows; y++)
            {
                for (int x = 0; x < cols; x++)
                {
                    if (grid[y][x])
                        DrawRectangle(x * cellSize, y * cellSize, cellSize - 1, cellSize - 1, RED);
                    else
                        DrawRectangleLines(x * cellSize, y * cellSize, cellSize, cellSize, BLACK);
                }
            }

            DrawText("Juego de la Vida - Conway", 10, 10, 20, PURPLE);
            DrawText("ESPACIO: Iniciar/Pausar", 10, 35, 15, BLUE);
            DrawText("R: Aleatorizar | C: Limpiar", 10, 55, 15, ORANGE);
            DrawText("Click izq: viva | der: muerta (en pausa)", 10, 75, 15, PINK);

            DrawText(running ? "Simulando..." : "En pausa", 10, screenHeight - 25, 20, running ? BROWN : GREEN);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}