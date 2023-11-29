/*
 * bl.c
 *
 *  Created on: Nov 29, 2023
 *      Author: ohuguenot
 */
#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx_hal.h"

#include "bootloader.h"


typedef void (*fptr_app_t)(void);

typedef struct {
    uint32_t	stack_addr;     // Stack Pointer
    fptr_app_t	app_ptr;        // Program Counter
} JumpStruct;


void bootloader_error(uint8_t num_blinks) {
	// blink a LED to signal error
//	while(1) {
//		HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
//		for(int i = 0; i < num_blinks * 2; i++) {
//			HAL_GPIO_TogglePin(GPIOx, GPIO_Pin);
//			for(int i = 0; i < 100000; i++) {
//			}
//		}
//		HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
//		for(int i = 0; i < 100000; i++) {
//		}
//	}
}


static void deinit_all() {
	//-- reset peripherals to guarantee flawless start of user application
	__HAL_RCC_GPIOC_CLK_DISABLE();
	__HAL_RCC_GPIOD_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOA_CLK_DISABLE();
	HAL_RCC_DeInit();
	HAL_DeInit();
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;
}


static void jump(const uint32_t address) {
	// cast address to jump struct:
	// 1st 32bits are stack pointer
	// 2nd 32bits are reset handler
	const JumpStruct* vector_p = (JumpStruct*)address;

	deinit_all();

    /* Jump, used asm to avoid stack optimization */
    asm("msr msp, %0; bx %1;" : : "r"(vector_p->stack_addr), "r"(vector_p->app_ptr));
}

/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */

#define MAX_CHECKED_BYTES	(32)

// address where the address to jump to is written
#define JUMP_ADDRESS_ADDR 	(ADDR_FLASH_SECTOR_7)

void bootloader_init() {

	bool valid_fw = true;

	// read address to jump to (32 bits)
	uint32_t jump_addr = 0x08004000; //*(uint32_t*)(JUMP_ADDRESS_ADDR);

	// check that a firmware is flashed at this address
	for (int idx = 0; idx < MAX_CHECKED_BYTES; idx++) {
		if (*(uint8_t*)(jump_addr + idx) == 0xff) {
			valid_fw = false;
			break;
		}
	}

	// jump to firmware
	if (valid_fw) {
		jump(jump_addr);
	}
	else {
		bootloader_error(4);
	}
}


