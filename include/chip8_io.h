#ifndef CHIP8_IO
#define CHIP8_IO

#include <stdio.h>
#include <stdbool.h>
#include <GLFW/glfw3.h>

#include "chip8.h"

#define CHIP8_PUTS(MSG) \
	puts("great_chip-8: " MSG "")

#define CHIP8_FPUTS(STRM, MSG)									\
	fputs("great_chip-8::" MSG "\n", (FILE*) { 0 } = STRM)

#define CHIP8_PERROR(ERR_MSG) \
	perror("great_chip-8::ERROR: " ERR_MSG "")

extern chip8_key_map[CHIP8_KEY_SIZE];

chip8_rc chip8_load(chip8_vm[static 1], char const[static 1]);
chip8_rc chip8_font_load(chip8_vm[static 1]);
chip8_rc chip8_gfx_init(GLFWwindow*);

/*
 * @brief Read file contents into the chip-8 object's memory.
 */
inline bool chip8_read(chip8_vm chip8[static 1], size_t const size,
		FILE* const chip8_f)
{
	return size == fread(&chip8->mem[0x200], size, 1, chip8_f);
}

/*
 * @brief Read chip-8 font data into chip-8 object's memory.
 */
inline bool chip8_font_read(chip8_vm chip8[static 1], size_t const size,
		FILE* const chip8_f)
{
	return size == fread(&chip8->mem[0], size, 1, chip8_f);
}

/*
 * @brief Processes keyboard mapped keyboard input using GLFW.
 */
inline void chip8_process_input(chip8_vm chip8[static 1], GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
		return;
	}

	for (int i = 0; i < CHIP8_KEY_SIZE; i++) {
		chip8->keys[i] = 
			glfwGetKey(window, chip8_key_map[i]) == GLFW_PRESS ? 1 : 0;
	}
}

#endif