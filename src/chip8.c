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
    // Extraemos los componentes comunes de las instrucciones para no repetir esto en cada case.

    // NNN: Dirección de 12 bits (Mascara 0x0FFF). Ej: 1NNN, ANNN.
    uint16_t nnn = opcode & 0x0FFF;

    // NN: Byte bajo de 8 bits (Mascara 0x00FF). Ej: 6XNN, 7XNN.
    uint8_t nn = opcode & 0x00FF;

    // N: Nibble bajo de 4 bits (Mascara 0x000F). Ej: DXYN.
    uint8_t n = opcode & 0x000F;

    // X: Segundo niblle. Hay que desplazarlo 8 bits a la derecha pra que sea un índice (0-15).
    // EJ: En 0x6A00, queremos que X sea 0xA (10), no 0xA00.
    uint8_t x = (opcode & 0x0F00) >> 8;

    // Y: Tercer nibble. Desplazamos 4 bits a la derecha para obtener un índice (0-15).
    // Ej: En 0x8X70, queremos  que Y sea 0x7 (7), no 0x70.
    uint8_t y = (opcode & 0x00F0) >> 4;

    // Usamos el primer nibble (4 bits más altos) para categorizar la instrucción.
    // Aplicamos una máscara AND con 0xF000.

    switch (opcode & 0xF000) {

        case 0x0000:
            switch (opcode) {
                case 0x00E0:
                    // 00E0 - CLS (Clear Screen)
                    memset(chip8->display, 0, sizeof(chip8->display));
                    chip8->draw_flag = true;    // Avisar a Raylib que redibuje
                    break;

                case 0x00EE:
                    // 00EE - RET (Return from subroutine)
                    if (chip8->sp > 0) { // Protección básica contra underflow
                        chip8->sp--;
                        chip8->pc = chip8->stack[chip8->sp];
                    }
                    break;

                default:
                    // 0NNN - Sys addr (Ignorado en emuladores modernos)
                    break;
            }
            break;
        
        case 0x1000:
            // 1NNN - JP addr (Salto a dirección NNN)
            chip8->pc = nnn;
            break;

        case 0x2000:
            // 2NNN - CALL addr
            if (chip8->sp < STACK_SIZE) { // Protección básica contra overflow
                chip8->stack[chip8->sp] = chip8->pc;
                chip8->sp++;
                chip8->pc = nnn;
            }
            break;

        case 0x6000:
            // 6XNN - LD Vx, byte (Carga el valor NN en el registro Vx)
            chip8->V[x] = nn;
            break;

        case 0x07000:
            // 7XNN - ADD Vx, byte (Suma NN a Vx sin acarreo)
            chip8->V[x] += nn;
            break;

        case 0x0800:
            // 8xyN - Operaciones entre Vx y Vy. El último dígito ( N ) determina la operación
            switch (opcode & 0x000F) {
                // 8xy0 - LD Vx, Vy
                // Asignación simple: Vx toma el valor de Vy
                case 0x0:
                    chip8->V[x] = chip8->V[y];
                    break;
                
                // 0xy1 - OR Vx, Vy
                // Bitwise OR: Enciende bits si alguno de los dos está encendido.
                case 0x1:
                    chip8->V[x] |= chip8->V[y];
                    break;

                // 8xy2 - AND Vx, Vy
                // Bitwise AND: Mantiene bits solo si ambos están encendidos.
                case 0x2:
                    chip8->V[x] &= chip8->V[y];
                    break;

                // 8xy3 - XOR Vx, Vy
                // Bitwise Exclusive OR: Enciende si son diferentes.
                case 0x3:
                    chip8->V[x] ^= chip8->V[y];
                    break;

                // 8xy4 - ADD Vx, Vy
                // Suma Vy a Vx. VF se pone a 1 si hay acarreo (resultado > 255).
                case 0x4: {
                    // Usamos uint16_t para detectar el resultado real antes del corte
                    uint16_t sum = chip8->V[x] + chip8->V[y];

                    if (sum > 255) {
                        chip8->V[0xF] = 1;
                    } else {
                        chip8->V[0xF] = 0;
                    }

                    // Guardamos solo los 8 bits bajos
                    chip8->V[x] = sum & 0xFF;
                    break;
                }

                // 8xy5 - SUB Vx, Vy
                // Vx = Vx - Vy
                case 0x5:
                    if (chip8->V[x] > chip8->V[y]) {
                        chip8->V[0xF] = 1;  // No hubo 'borrow' (resultado positivo)
                    } else {
                        chip8->V[0xF] = 0;  // Sí hubo 'borrow' (resultado negativo)
                    }
                    chip8->V[x] -= chip8->V[y];
                    break;
                
                // 8xy7 - SUBN Vx, Vy
                // Vx = Vy - Vx (Nota el orden invertido de la resta)
                case 0x7:
                    if (chip8->V[y] > chip8->V[x]) {
                        chip8->V[0xF] = 1;
                    } else {
                        chip8->V[0xF] = 0;
                    }
                    chip8->V[x] = chip8->V[y] - chip8->V[x];
                    break;

                // 8xy6 - SHR Vx {, Vy}
                // Desplazamiento a la derecha (División por 2)
                case 0x6:
                    // Guardamos el bit menos significativo (LSB) en VF antes de desplazar
                    chip8->V[0xF] = (chip8->V[x] & 0x1);
                    chip8->V[x] >>= 1;
                    break;

                // 8xyE - SHL Vx, {, Vy}
                // Desplazamiento a la izquierda (Multiplicación por 2)
                case 0xE:
                    // Guardamos el bit más significativo (MSB) en VF
                    chip8->V[0xF] = (chip8->V[x] & 0x80) >> 7;
                    chip8->V[x] <<= 1;
                    break;
            }
            break;

        case 0xA000:
            // ANNN - LD I, addr (Establece el registro índice I en NNN)
            chip8->I = nnn;
            break;

        case 0xC000:
            // CxNN - RND Vx, NN
            chip8->V[x] = (rand() % 256) & nn;
            break;
        
        // DXYN - DRW Vx, Vy, nibble (Dibuja sprite)
        // Dibuja un sprite en las coordenadas (Vx, Vy) con una altura de N píxeles.
        case 0xD000: {
            // 1. Obtenemos las coordenadas inciciales de los registros.
            //    Aplicamos módulo (%) para que si se pasan de 64/32, den a vuelta.
            uint8_t x_coord = chip8->V[x] % SCREEN_WIDTH;
            uint8_t y_coord = chip8->V[y] % SCREEN_HEIGHT;
            uint8_t height = n; // La altura es el último nibble del opcode

            // 2. Inicializamos el flag de colisión (VF) a 0.
            //    Solo se pondrá a 1 si detectamos que apagamos un píxel.
            chip8->V[0xF] = 0;

            // 3. Bucle para cada FILA del sprite (altura)
            for (int row = 0; row < height; row++) {

                // Obtenemos el byte de datos del sprite desde la memoria.
                // La dirección es I + la fila actual.
                uint8_t sprite_byte = chip8->memory[chip8->I + row];

                // 4. Bucle para cada PÍXEL (columna) de la fila (siempre 8 píxeles de ancho)
                for (int col = 0; col < 8; col++) {

                    // Comprobamos cada bit del sprite_byte, empezando por el más significativo (izquierda).
                    // Usamos una máscara (0x80 = 10000000) y la desplazamos a la derecha según la columna.
                    uint8_t sprite_pixel = sprite_byte & (0x80 >> col);

                    // Si el píxel del sprite es 0 (transparente), no hacemos nada.
                    // Solo dibujamos si el bit del sprite es 1.
                    if (sprite_pixel != 0) {
                        
                        // Calculamos la posición real en el buffer de pantalla 1D.
                        // Posición X actual = x_coord inicial + columna actual del bucle
                        // Posición Y actual = y_coord inicial + fila actual del bucle
                        int screen_x = x_coord + col;
                        int screen_y = y_coord + row;

                        // -- CLIPPING --
                        // Si el píxel se sale de la pantalla por la derecha o por abajo, lo ignoramos.
                        if (screen_x >= SCREEN_WIDTH || screen_y >= SCREEN_HEIGHT) {
                            continue;
                        }

                        // Índice en el array lineal display[]
                        int screen_index = screen_x + (screen_y * SCREEN_WIDTH);

                        // -- DETECCIÓN DE COLISIÓN --
                        // Si el píxel en la pantalla ya está encendido (1) y vamos a pintar (1),
                        // ocurrirá una colisión (1^1=0). Marcamos VF.
                        if (chip8->display[screen_index] == 1) {
                            chip8->V[0xF] = 1;
                        }

                        // -- DIBUJADO (XOR) --
                        // Aplicamos XOR al píxel de la pantalla.
                        chip8->display[screen_index] ^= 1;
                    }
                }
            }

            // Avisamos al sistema principal que la pantalla ha cambiado y necesita repintarse.
            chip8->draw_flag = true;
            break;
        }

        default:
            // Si llegamos aquí, encontramos un opcode desconocido.
            printf("Opcode desconocido: 0x%X\n", opcode);
            break;
    }
}

// Actualiza los temporizadores del sistema.
// Esta función debe llamarse a una frecuencia de 60Hz.
void chip8_update_timers(chip8_t *chip8) {
    if (chip8->delay_timer > 0) {
        chip8->delay_timer--;
    }

    if (chip8->sound_timer > 0) {
        chip8->sound_timer--;
        // Aquí podríamos poner un flag para que Raylib sepa que debe emitir sonido
        // o manejar el audio directamente en el main loop revisando esta variable.
    }
}

// Carga un archivo ROM en la memoria del CHIP-8
// Retorna true si tuvo éxito, false si falló.
bool chip8_load_rom(chip8_t *chip8, const char *filename) {
    // Abrimos el archivo en modo binario ("rb")
    // Es CRÍTICO usar "rb" en Windows/Linux para no alterar los bytes de nueva línea.
    FILE *rom = fopen(filename, "rb");
    if (!rom) {
        fprintf(stderr, "Error: No se pudo abrir la ROM %s\n", filename);
        return false;
    }

    // Buscamos el tamaño del archivo
    fseek(rom, 0, SEEK_END);    // Ir al final
    long rom_size = ftell(rom); // Lee posición actual (tamaño)
    rewind(rom);  // Volver al principio

    // Verificamos si la ROM cabe en la memoria
    // Espacio disponible = Total RAM (4096) - Inicio reservado (512)
    if (rom_size > RAM_SIZE - START_ADDRESS) {
        fprintf(stderr, "Error: La ROM es demasiado grande (%ld bytes)\n", rom_size);
        fclose(rom);
        return false;
    }

    // Leemos el archivo directamente hacia la memoria del emulador
    // &chip8->memory[START_ADDRESS] es el puntero a la dirección 0x200
    fread(&chip8->memory[START_ADDRESS], 1, rom_size, rom);

    fclose(rom);
    return true;
}
