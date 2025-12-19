#include "chip8.h"
#include <stdio.h> // Para printf (útil para debug si una instrucción falla)

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

// Ejecuta un cicle de CPU (una instrucción)
void chip8_cycle(chip8_t *chip8) {
    // -------------------------------
    // 1. FETCH (Captura)
    // -------------------------------
    // Recuperamos el opcode de 16 bits combinando dos bytes de memoria.
    // pc: byte alto (high byte). pc+1 byte bajo (low byte).
    uint16_t opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];

    // Avanzamos el Program Counter para la próxima instrucción.
    // Es vital hacer esto ANTES de ejecutar, o los saltos (Jumps) no funcionarán bien,
    // ya que un Jump sobreescribe el PC. Si sumamos 2 después del Jump, aterrizaremos mal.
    chip8->pc += 2;

    // -------------------------------
    // 2. DECODE & EXECUTE 
    // -------------------------------
    // Uasmos el primer nibble (4 bits más altos) para categorizar la instrucción.
    // Aplicamos una máscara AND con 0xF000.

    switch (opcode & 0xF000) {

        case 0x0000:
            switch (opcode) {
                case 0x00E0:
                    // 00E0 - CLS (Clear Screen)
                    // TODO: Implementar limpiar pantalla
                    break;
                case 0x00EE:
                    // 00EE - RET (Return from subroutine)
                    // TODO: Implementar retorno de subrutina
                    break;
                default:
                    // 0NNN - Sys addr (Ignorado en emuladores modernos)
                    break;
            }
            break;
        
        case 0x1000:
            // 1NNN - JP addr (Salto a dirección NNN)
            // TODO: Implementar salto
            break;

        case 0x6000:
            // 6XNN - LD Vx, byte (Carga el valor NN en el registro Vx)
            // TODO: Implementar carga de registro
            break;

        case 0x07000:
            // 7XNN - ADD Vx, byte (Suma NN a Vx sin acarreo)
            // TODO: Implementar suma simple
            break;

        case 0xA000:
            // ANNN - LD I, addr (Establece el registro índice I en NNN)
            // TODO: Implementar set index
            break;

        case 0xD000:
            // DXYN - DRW Vx, Vy, nibble (Dibuja sprite)
            // TODO: Implementar gráficos
            break;

        default:
            // Si llegamos aquí, encontramos un opcode desconocido.
            printf("Opcode desconocido: 0x%X\n", opcode);
            break;
    }
}