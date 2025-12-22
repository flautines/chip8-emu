#include "debug.h"
#include <stdio.h>

// Imprime el estado actual del PC, Opcode y algunos registros clave
void chip8_debug_print(chip8_t *chip8) {
    uint16_t opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc+1];

    printf("PC: 0x%04X | Opcode: 0x%04X | I: 0x%04X | SP: %d\n",
           chip8->pc, opcode, chip8->I, chip8->sp);

    // Imprime los primeros 4 registros V para no saturar
    printf("V0: %02X V1: %02X V2: %02X V3: %02X\n",
           chip8->V[0], chip8->V[1], chip8->V[2], chip8->V[3]);
}