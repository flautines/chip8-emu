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

    // -- PILA (STACK) --




    
} chip8_t;

#endif