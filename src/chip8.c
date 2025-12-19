#include "chip8.h"

// Los sprites de los caracteres hexadecimales (0-F).
// Cada byte representa una fila de 8 píxeles.
// Ejemplo del '0':
// 0xF0 -> 11110000 ####
// 0x90 -> 10010000 #  #
// 0x90 -> 10010000 #  #
// 0x90 -> 10010000 #  #
// 0xF0 -> 11110000 ####

const uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0
    0x20, 0x60, 0x20, 0x20, 0x70,   // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,   // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,   // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,   // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,   // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,   // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,   // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,   // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,   // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,   // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,   // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,   // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,   // E
    0xF0, 0x80, 0xF0, 0x80, 0x80    // F
};

// Inicializa o reinicia la máquina CHIP-8
void chip8_init(chip8_t *chip8) {
    // 1. Limpiamos toda la memoria y registros
    //    Es importante poner el PC en 0x200, no en 0.
    chip8->pc = START_ADDRESS;
    chip8->I = 0;
    chip8->sp = 0;
    chip8->delay_timer = 0;
    chip8->sound_timer = 0;

    // Limpiamos (ponemos a 0) arrays completos
    memset(chip8->memory, 0, sizeof(chip8->memory));
    memset(chip8->V, 0, sizeof(chip8->V));
    memset(chip8->stack, 0, sizeof(chip8->stack));
    memset(chip8->display, 0, sizeof(chip8->display));

    // 2. Cargamos el fontset en la memoria
    //    Lo copiamos desde nuestro array 'const' hacia la RAM de la máquina
    //    empezando en la dirección 0x50 (80 decimal).
    for (int i = 0; i < 80; i++) {
        chip8->memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }

    // Inicializamos la semilla aleatoria (necesario para la instrucción RND)
    // Nota: srand se suele llamar una sola vez en el main, pero lo mencionamos aquí.
}