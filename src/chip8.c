#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <GLFW/glfw3.h>

#include "chip8.h"
#include "chip8_io.h"
#include "chip8_istr.h"
#include "chip8_gfx.h"
#include "chip8_dbg.h"

static chip8_vm* chip8_new_vm(void)
{
	chip8_vm* chip8 = calloc(1, sizeof(*chip8));

	if (!chip8) {
		CHIP8_ERR("ERROR::Memory allocation failed");
		return NULL;
	}
	chip8->pc = 0x200;
	chip8->sp = 0xEA0;
	chip8->idx = 0;
	return chip8;
}

static chip8_rc chip8_init_vm(chip8_vm** const chip8_ptr,
		const char* argv[static 2], const size_t rom_index)
{
	*chip8_ptr = chip8_new_vm();

	if (!*chip8_ptr) {
		CHIP8_PERROR("Virtual machine construction failed");
		return CHIP8_FAILURE;
	} else if (!chip8_load_data(&((*chip8_ptr)->mem), argv[rom_index], 0x200)) {
		CHIP8_PERROR("ROM load failed");
		return CHIP8_FAILURE;
	} else if (!chip8_load_data(&((*chip8_ptr)->mem), CHIP8_FONT_PATH, 0)) {
		CHIP8_PERROR("Font load failed");
		return CHIP8_FAILURE;
	}
	CHIP8_MEM_DUMP((*chip8_ptr)->mem);
	return CHIP8_SUCCESS;
}

/*
 * @brief Fetch next instruction from loaded chip-8 ROM.
 */
static inline chip8_word chip8_fetch(chip8_vm chip8[const static 1])
{
	chip8_word istr = chip8->mem[chip8->pc];
	istr <<= 8;
	istr |= chip8->mem[chip8->pc+1];
	return istr;
}

/*
 * @brief Disassemble next instruction and execute with corresponding function.
 */
static inline chip8_rc chip8_execute(chip8_vm chip8[const static 1])
{
	static time_t timer;
	static chip8_byte cycle;
	const chip8_opcode opcode = chip8_disassemble(chip8->istr);

	if (NOP == opcode) {
		return CHIP8_FAILURE;
	}

	if (cycle == 60) {
		if (chip8->dly_tmr) {
			chip8->dly_tmr--;
		}

		if (chip8->snd_tmr) {
			chip8->snd_tmr--;
		}
	} else if (time(NULL) == timer) {
		cycle++;
	} else {
		cycle = 0;
	}
	chip8_istr_set[opcode](chip8);
	time(&timer);
	return CHIP8_SUCCESS;
}

int main(int argc, char* argv[argc+1])
{
	int exit_state = EXIT_SUCCESS;
	chip8_vm* chip8;
	GLFWwindow* window;
	chip8_renderer* renderer;

	srand((unsigned) time(NULL));

	/* initialize Chip-8 virtual machine */
	if (!chip8_init_vm(&chip8, argv, 1)) {
		CHIP8_ERR("ERROR: Virtual machine initialization failed");
		exit_state = CHIP8_FAILURE;
		goto EXIT;
	}

	/* initialize graphics and create window */
	if (!chip8_init_gfx(&window, &renderer, CHIP8_DEFAULT_RES_SCALE)) {
		CHIP8_ERR("ERROR: OpenGL initialization failed");
		exit_state = CHIP8_FAILURE;
		goto EXIT;
	}

	/* fetch-execute cycle */
	while (!glfwWindowShouldClose(window)) {
		chip8->istr = chip8_fetch(chip8);

		if (!chip8_execute(chip8)) {
			CHIP8_ERR("ERROR: chip-8 execution failed, this shouldn't happen");
			exit_state = CHIP8_FAILURE;
			goto EXIT;
		}

		if (draw_flag) {
			chip8_render(chip8, renderer);
			glfwSwapBuffers(window);
			draw_flag = false;
		}
	}

EXIT:
	free(chip8);
	free(renderer);
	return exit_state;
}
