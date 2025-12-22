# CHIP-8 Emulator en C99

![CHIP-8 Banner](https://img.shields.io/badge/Language-C99-blue) ![Library](https://img.shields.io/badge/Library-Raylib-red) ![License](https://img.shields.io/badge/License-MIT-green)

Un emulador completo y funcional de la máquina virtual **CHIP-8** escrito desde cero en C estándar (C99). Este proyecto utiliza la librería **Raylib** para el renderizado gráfico, la entrada de usuario y la síntesis de audio.

El objetivo de este proyecto es demostrar conceptos fundamentales de emulación de sistemas: ciclo Fetch-Decode-Execute, gestión de memoria, manipulación de bits, timers de 60Hz y depuración visual.

## 🚀 Características

* **Emulación Completa:** Soporte para los 35 opcodes originales del set de instrucciones CHIP-8.
* **Gráficos:** Renderizado escalado con detección de colisiones vía XOR.
* **Sonido:** Sintetizador de onda senoidal (Beeper) generado proceduralmente.
* **Debug Overlay:** Interfaz visual (activable con `F1`) para inspeccionar Registros, PC, I y Stack en tiempo real.
* **Modo Paso a Paso:** Capacidad de pausar la ejecución y avanzar instrucción por instrucción.
* **Compatibilidad:** Gestión de "Quirks" configurables (Bit Shifting y Load/Store behavior) para soportar ROMs antiguas y modernas.
* **Cross-Platform:** Código C99 compatible con Linux, Windows, macOS y WebAssembly.

## 🛠️ Requisitos

Para compilar este proyecto necesitas:
* **GCC** (Compilador de C)
* **Make** (Sistema de construcción)
* **Raylib** (Librería multimedia)

### Instalación en Linux (Debian/Ubuntu)

```bash
# 1. Instalar herramientas de compilación
sudo apt update
sudo apt install build-essential git

# 2. Instalar Raylib (desde repositorios o compilar manualmente)
sudo apt install libraylib-dev

```
## 📦 Compilación

Clona el repositorio y compila usando `make`:

```Plaintext

git clone https://github.com/flautines/chip8-emu.git
cd chip8-emu
make

```

Esto generará un ejecutable binario llamado chip8.

## 🎮 Uso

Ejecuta el emulador pasando la ruta de una ROM como argumento:

```sh

./chip8 roms/BRIX.ch8

```

**Controles**

El teclado original hexadecimal (0-F) está mapeado a la parte izquierda del teclado QWERTY:


```Plaintext
 Teclado CHIP-8	   Teclado PC 
|===============|==============|
|   1 2 3 C	    |   1 2 3 4    |
|   4 5 6 D	    |   Q W E R    |
|   7 8 9 E	    |   A S D F    |
|   A 0 B F	    |   Z X C V    |
```

**Teclas de Sistema / Debug**

|Tecla	| Acción |
|-------|--------|
|ESC	| Salir del emulador |
|F1	| Mostrar/Ocultar Interfaz de Debug (Registros) |
|P	| Pausar / Reanudar la CPU |
|S	| Avanzar un paso (solo si está pausado) |

## 📂 Estructura del Proyecto

```Plaintext

chip8-emu/
├── src/
│   ├── main.c       # Bucle principal, Raylib, Input, Audio
│   └── chip8.c      # Implementación de la CPU, Opcodes y Lógica
├── include/
│   └── chip8.h      # Definiciones, Constantes y Structs
├── roms/            # Carpeta para colocar tus juegos .ch8
└── Makefile         # Script de compilación automatizado
```

## ⚙️ Configuración de Compatibilidad (Quirks)

Algunos juegos antiguos dependen de comportamientos específicos del hardware original (COSMAC VIP) o de variantes posteriores (SuperChip).

Puedes configurar estos comportamientos editando src/chip8.c en la función chip8_init:

```C

// Ejemplo en src/chip8.c
chip8->quirk_shift = true;       // true: Moderno (x >> 1), false: Original (y >> 1)
chip8->quirk_load_store = false; // true: I cambia, false: I estático

```

## 📜 Créditos y Referencias

Desarrollado siguiendo las especificaciones técnicas de:

* Cowgod's Chip-8 Technical Reference
* Guide to making a CHIP-8 emulator (Tobias V. Langhoff)

## 📄 Licencia

Este proyecto está bajo la Licencia MIT. Siéntete libre de usarlo, modificarlo y aprender de él.

