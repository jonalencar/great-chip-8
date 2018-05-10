#ifndef CHIP8_H
#define CHIP8_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define CHIP8_MEM_SIZE 0xFFF
#define CHIP8_STACK_SIZE 24
#define CHIP8_DISP_RES_WIDTH 64
#define CHIP8_DISP_RES_HEIGHT 32
#define CHIP8_KEY_SIZE 0xF

typedef uint8_t chip8_byte;
typedef uint16_t chip8_word;

/*
 * @brief contains register bank indices for register array
 */
enum chip8_register {
	V0, V1, V2, V3,
	V4, V5, V6, V7,
	V8, V9, VA, VB,
	VC, VD, VE, VF,
	REG_BANK_SIZE
};

/* 
 * @brief chip8 virtual machine structure
 */
typedef struct chip8_virtual_machine {
	chip8_word pc; /* program counter */
	chip8_word idx; /* index register */
	chip8_word inst_word; /* current instruction */
	chip8_word stack[CHIP8_STACK_SIZE]; /* memory stack */

	chip8_byte regs[REG_BANK_SIZE]; /* register unit array */
	chip8_byte mem[CHIP8_MEM_SIZE];
	chip8_byte gfx[CHIP8_DISP_RES_WIDTH][CHIP8_DISP_RES_HEIGHT]; /* pixel array */
	chip8_byte keys[CHIP8_KEY_SIZE];

	chip8_byte dly_tmr; /* used for timing events */
	chip8_byte snd_tmr; /* used for sound effects */
} chip8_vm;

typedef enum chip8_return_code {
	CHIP8_SUCCESS,
	CHIP8_FAILURE
} chip8_rc; 

/*
 * @brief chip8_vm constructor
 */
inline chip8_vm* chip8_new(void)
{
	return calloc(1, sizeof(chip8_vm));
}

/*
 * @brief chip8_vm content destroyer for resetting chip8_vm structure
 */
inline void chip8_destroy(chip8_vm chip8[static 1])
{
	if (chip8) { *chip8 = (chip8_vm) { 0 }; }
}

/*
 * @brief read file contents into chip8_vm memory array
 */
inline int chip8_read(chip8_vm chip8[static 1], size_t const size,
		FILE* const chip8_f)
{
	return size == fread(&chip8->mem[0x200], size, 1, chip8_f);
}

#endif