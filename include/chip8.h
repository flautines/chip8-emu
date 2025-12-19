#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>     // Para uint8_t, uint16_t, etc.
#include <stdbool.h>    // Para tipo bool, true, false
#include <string.h>     // Para memset (usado en la inicialización)
#include <stdlib.h>     // Para rand()

// --- CONSTANTES DEL SISTEMA ---

// Ancho y alto de la pantalla original del CHIP-8 en píxeles.
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32 

// Tamaño total de la memoria RAM en bytes (4KB)
#define RAM_SIZE 4096

// Cantidad de registros de propósito general (V0 a VF).
#define NUM_REGISTERS 16

// Niveles de profundidad de la pila (Stack).
// Esto limita cuántas subrutinas anidadas podemos tener (llamadas dentro de llamadas).
#define STACK_SIZE 16

// Cantidad de teclas en el teclado hexadecimal (0-F).
#define NUM_KEYS 16

// Dirección de memoria donde comienzan casi todos los programas CHIP-8.
// Los primeros 512 bytes (0x000 a 0x1FF) estaban reservados originalmente
// para el intérprete en las máquinas COSMAC VIP.
#define START_ADDRESS 0x200

// Dirección donde cargaremos la fuente tipográfica (sprites de 0-F).
#define FONTSET_START_ADDRESS 0x50

// Estructura que representa el estado completo de la máquina CHIP-8
typedef struct {
    // -- MEMORIA --
    // Array lineal que representa los 4KB de RAM.
    uint8_t memory[RAM_SIZE];

    // -- REGISTROS --
    // 16 registros de 8 bits (V0, V1... VF).
    // Se usan para manipulación de datos, contadores, etc.
    // VF es especial: se usa como bandera (flag) de acarreo o colisión.
    uint8_t V[NUM_REGISTERS];

    // Registro de índice (Index Register).
    // Tiene 16 bits porque almacena direcciones de memoria (0x000 a 0xFFF).
    uint16_t I;

    // Contador de Programa (Program Counter).
    // Indica la dirección de memoria de la *siguiente* instrucción a ejecutar.
    uint16_t pc;

    // -- PILA (STACK) --
    // Almacena direcciones de retorno cuando se llama a una subrutina.
    uint16_t stack[STACK_SIZE];

    // Puntero de Pila (Stack Pointer).
    // Indica en qué nivel de la pila estamos actualmente
    uint8_t sp;

    // -- PANTALLA --
    // Buffer de video monocromático.
    // Aunque la pantalla es 2D (64x32), la representamos como un array 1D
    // de tamaño 2048 (64*32).
    // 1 = Píxel encendido, 0 = Píxel apagado.
    uint8_t display[SCREEN_WIDTH * SCREEN_HEIGHT];
    
    // -- TECLADO --
    // Almacena el estado actual de las 16 teclas.
    // true = Presionada, false = Soltada.
    bool keypad[NUM_KEYS];

    // -- TEMPORIZADORES --
    // Se decrementan automáticamnete a una frecuencia de 60Hz.

    // Delay Timer: Se usa para cronometrar eventos del juego.
    uint8_t delay_timer;

    // Sound Timer: Mientras sea mayor que 0, el emulador debe emitir un tono.
    uint8_t sound_timer;

    // -- EXTRAS --
    // Bandera para indicar si hay que dibujar en este cicle (optimización).
    bool draw_flag;
    
} chip8_t;

// Inicializa o reinicia la máquina CHIP-8
void chip8_init(chip8_t *chip8);
void chip8_cycle(chip8_t *chip8);

#endif