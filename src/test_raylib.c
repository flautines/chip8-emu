#include <raylib.h>

int main(void) {
    // Configuración inicial
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "CHIP-8 Emulator - Raylib Test");
    SetTargetFPS(60);

    // Bucle pricipal
    while (!WindowShouldClose()) {
        // Actualización
        // (En nuestro emulador aquí iría la lógica CHIP-8)

        // Dibujado
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("¡raylib funciona correctamente!", 190, 200, 20, LIGHTGRAY);
        DrawFPS(10, 10);

        // Dibuja un cuadrado que representaría un pixel CHIP-8 escalado
        DrawRectangle(350, 250, 16, 16, BLACK); // 16x = 1 pixel CHIP-8

        EndDrawing();
    }

    CloseWindow();
    return 0;
}