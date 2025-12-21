#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "chip8.h"

// --- CONFIGURACIÓN DE PANTALLA ---

// Factor de escala. Cada píxel del CHIP-8 será un cuadrado de 10x10 en tu monitor.
// 64x10 = 640 ancho, 32x10 = 320 alto.
#define SCALE_FACTOR 15

// Calculamos el tamaño real de la ventana
#define WINDOW_WIDTH (SCREEN_WIDTH * SCALE_FACTOR)
#define WINDOW_HEIGHT (SCREEN_HEIGHT * SCALE_FACTOR)

// Velocidad de simulación: Cuántos ciclos de CPU corremos por cada cuadro de vídeo (60Hz).
// CHIP-8 corría aprox a 500Hz - 700Hz.
// 60 frames * 10 ciclos = 600 instrucciones por segundo.
#define CYCLES_PER_FRAME 10

int main(int argc, char **argv) {
    // 1. Verificación de argumentos
    if (argc != 2) {
        printf("Uso: %s <ruta_a_la_rom>\n", argv[0]);
        return 1;
    }

    // 2. Inicialización del Hardware Virtual
    chip8_t chip8;
    chip8_init(&chip8);

    // Intentamos cargar la ROM especificada
    if (!chip8_load_rom(&chip8, argv[1])) {
        // El mensaje de error ya se imprime dentro de chip8_load_rom
        return 1;
    }

    // 3. Inicialización de Raylib (La Ventana)
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Emulador CHIP-8 en C");

    // Fijamos los FPS a 60. Esto es CRÍTICO.
    // Raylib intentará dormir el proceso para mantener esta velocidad estable.
    // Esto nos servirá como reloj maestro para los Timers del CHIP-8.
    SetTargetFPS(60);

    // Bucle principal: Se ejecuta mientras no cerramos la ventana
    while (!WindowShouldClose()) {

        // --- A. SIMULACIÓN DE CPU ---
        // Ejecutamos varios ciclos de CPU por cada frame de vídeo.
        // Esto separa la velocidad de renderizado (60Hz) de la velocidad de procesamiento (~600Hz).
        for (int i = 0; i < CYCLES_PER_FRAME; i++) {
            chip8_cycle(&chip8);
        }

        // --- B. ACTUALIZACIÓN DE TEMPORIZADORES ---
        // Los timers de CHIP-8 funcionan a 60Hz, igual que nuestro refresco de pantalla.
        // Por lo tanto, los actualizamos una vez por vuelta del bucle principal.
        chip8_update_timers(&chip8);

        // --- C. RENDERIZADO (DIBUJO) ---
        BeginDrawing();

        ClearBackground(BLACK); // Limpiamos el fondo (color negro)

        // Solo redibujamos los rectángulos si es necesario,
        // aunque Raylib es tan rápido que podríamos hacerlo siempre sin perder rendimiento.
        // Recorremos el array display[] linealmente.
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {

                // Calculamos el índice en el array 1D
                int index = x + (y * SCREEN_WIDTH);

                // Si el píxel está encendido (1)
                if (chip8.display[index] != 0) {
                    // Dibujamos un rectángulo escalado
                    // Posición X: x original * escala
                    // Posición Y: y original * escala
                    // Tamaño: escala x escala
                    DrawRectangle(x * SCALE_FACTOR, y * SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR, WHITE);
                }
            }
        }

        // Debug info en pantalla (Opcional)
        // DrawText("Ejecutando...", 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    // 4. Limpieza
    CloseWindow();  // Cierra ventana y contexto OpenGL

    return 0;
}