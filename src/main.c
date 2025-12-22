#include <stdio.h>
#include <math.h>
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

// Mapa de teclas: Índice del array = Valor Hexadecimal CHIP8-8
// Valor del array = Código de tecla de Raylib
const int KEYMAP[16] = {
    KEY_X,          // 0
    KEY_ONE,        // 1
    KEY_TWO,        // 2
    KEY_THREE,      // 3
    KEY_Q,          // 4
    KEY_W,          // 5
    KEY_E,          // 6
    KEY_A,          // 7
    KEY_S,          // 8
    KEY_D,          // 9
    KEY_Z,          // A
    KEY_C,          // B
    KEY_FOUR,       // C
    KEY_R,          // D
    KEY_F,          // E
    KEY_V,          // F
};

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

    // 1. Inicializar Audio
    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        printf("Error: No se pudo inicializar el dispositivo de audio.\n");
        return 1;
    }

    // Configuración del stream de audio (44100Hz, 16bit, Mono)
    SetAudioStreamBufferSizeDefault(4096);
    AudioStream stream = LoadAudioStream(44100, 16, 1);
    PlayAudioStream(stream);    // Empezamos el stream (aunque le enviaremos silencio por ahora)

    // Variables para la generación de onda
    float frequency = 440.0f;   // 440Hz (Nota La)
    float sineIdx = 0.0f;

    // Control de modo pausa y debug
    bool debug_mode = false;    // Alternar con F1
    bool paused = false;        // Alternar con P

    // Fijamos los FPS a 60. Esto es CRÍTICO.
    // Raylib intentará dormir el proceso para mantener esta velocidad estable.
    // Esto nos servirá como reloj maestro para los Timers del CHIP-8.
    SetTargetFPS(60);

    // Bucle principal: Se ejecuta mientras no cerramos la ventana
    while (!WindowShouldClose()) {

        // --- GESTIÓN DE ENTRADA ---
        // Actualizamos el estado del teclado virtual antes de que la CPU corra.
        for (int i = 0; i < 16; i++) {
            if (IsKeyDown(KEYMAP[i])) {
                chip8.keypad[i] = true;     // Tecla presionada
            } else {
                chip8.keypad[i] = false;    // Tecla suelta
            }
        }

        // Alterna entre modo Debug
        if (IsKeyPressed(KEY_F1)) {
            debug_mode = !debug_mode;
        }

        // Alterna entre modo Pause
        if (IsKeyPressed(KEY_P)) {
            paused = !paused;
        }

        // --- A. SIMULACIÓN DE CPU ---
        // Ejecutamos varios ciclos de CPU por cada frame de vídeo.
        // Esto separa la velocidad de renderizado (60Hz) de la velocidad de procesamiento (~600Hz).
        if (!paused) {
            // Ejecución normal a 600Hz (10 ciclos por frame)
            for (int i = 0; i < CYCLES_PER_FRAME; i++) {
                chip8_cycle(&chip8);
            }
        } else {
            // Estamos en PAUSA.
            // Solo avanzamos si el usuario presiona 'S' (Step)
            if (IsKeyPressed(KEY_S)) {
                chip8_cycle(&chip8);
                // Opcional: Imprimir en consola también para tener historial
                chip8_debug_print(&chip8);
            }
        }
        

        // --- B. ACTUALIZACIÓN DE TEMPORIZADORES ---
        // Los timers de CHIP-8 funcionan a 60Hz, igual que nuestro refresco de pantalla.
        // Por lo tanto, los actualizamos una vez por vuelta del bucle principal.
        chip8_update_timers(&chip8);

        // --- GESTIÓN DE SONIDO ---

        if (chip8.sound_timer > 0) {
            // Si el timer está activo, generamos datos de onda senoidal
            if (IsAudioStreamProcessed(stream)) {
                // Buffer temporal para enviar al stream
                short *data = (short *)malloc(sizeof(short) * 4096);

                // Rellenamos el buffer con la onda matemática
                for (int k = 0; k < 4096; k++) {
                    data[k] = (short)(32000.0f * sinf(2 * PI * frequency * sineIdx / 44100));
                    sineIdx += 1.0f;
                }

                // Enviamos los datos a la tarjeta de sonido
                UpdateAudioStream(stream, data, 4096);
                free(data);
            }
        } else {
            // Si el timer es 0, aseguramos silencio.
            // Simplemente no actualizamos el stream con datos nuevos,
            // o podríamos enviar ceros si quisiéramos limpiar el buffer explícitamente.
            // Para simplificar, reiniciamos el índice de la onda para que no "cruja" al volver a empezar.
            sineIdx = 0.0f;
        }
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

        // --- DIBUJADO DE DEBUG UI ---
        if (debug_mode) {
            // Fondo semitransparente para que el texto se lea bien
            DrawRectangle(0, 0, 200, WINDOW_HEIGHT, Fade(BLUE, 0.9f));

            // Dibujamos los registros
            DrawText("REGISTROS", 10, 10, 10, WHITE);
            for (int i = 0; i < 16; i++) {
                char buffer[32];
                sprintf(buffer, "V%X: 0x%02X", i, chip8.V[i]);
                DrawText(buffer, 10, 30 + (i * 15), 10, WHITE);
            }

            char buffer[64];
            sprintf(buffer, "PC: 0x%04X", chip8.pc);
            DrawText(buffer, 10, 280, 10, YELLOW);

            sprintf(buffer, "I:  0x%04X", chip8.I);
            DrawText(buffer, 10, 300, 10, YELLOW);

            sprintf(buffer, "SP: 0x%02X", chip8.sp);
            DrawText(buffer, 10, 320, 10, YELLOW);

            if (paused) {
                DrawText("- PAUSADO -", 10, 350, 10, RED);
                DrawText("Presiona 'S' para Step", 10, 370, 10, GRAY);
            }
        }
        

        EndDrawing();
    }

    // 4. Limpieza
    UnloadAudioStream(stream);
    CloseAudioDevice();
    CloseWindow();  // Cierra ventana y contexto OpenGL

    return 0;
}