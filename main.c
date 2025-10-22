#include "raylib.h"
//------------------------------------------------------------------------------------
// Programa principal
//------------------------------------------------------------------------------------
int main(void)
{
    // Inicialización
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "PONG - Rodrigo");

    // Pelota
    Vector2 ballPosition = { GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
    Vector2 ballSpeed = { 5.0f, 4.0f };
    int ballRadius = 10;

    // Jugadores
    const int paddleWidth = 20;
    const int paddleHeight = 100;
    Rectangle player1 = { 30, (float)screenHeight/2 - paddleHeight/2, paddleWidth, paddleHeight };
    Rectangle player2 = { (float)screenWidth - 50, (float)screenHeight/2 - paddleHeight/2, paddleWidth, paddleHeight };
    const float paddleSpeed = 6.0f;

    // Línea central con agujero
    const int midLineWidth = 4;
    const int midHoleHeight = 80;
    Rectangle midLineTop = { screenWidth/2 - midLineWidth/2, 0, midLineWidth, screenHeight/2 - midHoleHeight/2 };
    Rectangle midLineBottom = { screenWidth/2 - midLineWidth/2, screenHeight/2 + midHoleHeight/2, midLineWidth, screenHeight/2 - midHoleHeight/2 };

    // Contadores de puntos
    int scorePlayer1 = 0;
    int scorePlayer2 = 0;

    SetTargetFPS(60);
    //------------------------------------------------------------------------------------
    while (!WindowShouldClose())
    {
        // --- Actualización ---
        // Movimiento jugador 1
        if (IsKeyDown(KEY_W)) player1.y -= paddleSpeed;
        if (IsKeyDown(KEY_S)) player1.y += paddleSpeed;

        // Movimiento jugador 2
        if (IsKeyDown(KEY_UP)) player2.y -= paddleSpeed;
        if (IsKeyDown(KEY_DOWN)) player2.y += paddleSpeed;

        // Limitar movimiento dentro de la pantalla
        if (player1.y < 0) player1.y = 0;
        if (player1.y + player1.height > screenHeight) player1.y = screenHeight - player1.height;
        if (player2.y < 0) player2.y = 0;
        if (player2.y + player2.height > screenHeight) player2.y = screenHeight - player2.height;

        // Movimiento de la pelota
        ballPosition.x += ballSpeed.x;
        ballPosition.y += ballSpeed.y;

        // Rebote con límites superior e inferior
        if ((ballPosition.y <= ballRadius) || (ballPosition.y >= screenHeight - ballRadius))
        {
            ballSpeed.y *= -1;
        }

        // Colisiones con jugadores
        if (CheckCollisionCircleRec(ballPosition, ballRadius, player1))
        {
            ballSpeed.x *= -1;
            ballPosition.x = player1.x + player1.width + ballRadius;
        }

        if (CheckCollisionCircleRec(ballPosition, ballRadius, player2))
        {
            ballSpeed.x *= -1;
            ballPosition.x = player2.x - ballRadius;
        }

        // Puntaje y reinicio de pelota si sale de los lados
        if (ballPosition.x < 0)
        {
            scorePlayer2++; // Jugador 2 anota
            ballPosition = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
            ballSpeed = (Vector2){ 5.0f, 4.0f };
        }
        else if (ballPosition.x > screenWidth)
        {
            scorePlayer1++; // Jugador 1 anota
            ballPosition = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
            ballSpeed = (Vector2){ -5.0f, 4.0f }; // Invertir dirección para variar
        }

        // --- Dibujo ---
        BeginDrawing();
            ClearBackground(BLACK);

            // Líneas de límites
            DrawLine(0, 0, screenWidth, 0, GRAY);
            DrawLine(0, screenHeight - 1, screenWidth, screenHeight - 1, GRAY);

            // Línea central
            DrawRectangleRec(midLineTop, GRAY);
            DrawRectangleRec(midLineBottom, GRAY);

            // Jugadores
            DrawRectangleRec(player1, RAYWHITE);
            DrawRectangleRec(player2, RAYWHITE);

            // Pelota
            DrawCircleV(ballPosition, (float)ballRadius, MAROON);

            // Textos de ayuda
            DrawText("Jugador 1: W / S", 20, 20, 20, LIGHTGRAY);
            DrawText("Jugador 2: FLECHAS ↑ / ↓", screenWidth - 260, 20, 20, LIGHTGRAY);

            // Mostrar puntaje
            DrawText(TextFormat("%d", scorePlayer1), screenWidth/4, 40, 40, RAYWHITE);
            DrawText(TextFormat("%d", scorePlayer2), 3*screenWidth/4, 40, 40, RAYWHITE);

            DrawFPS(10, screenHeight - 30);
        EndDrawing();
    }
    //------------------------------------------------------------------------------------
    CloseWindow();
    return 0;
}
