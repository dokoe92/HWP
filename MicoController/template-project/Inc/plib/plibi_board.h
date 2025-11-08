#ifndef PLIBI_BOARD_H_
#define PLIBI_BOARD_H_

#include <stdint.h>
#include "plib_config.h"
#include "stm32h5xx.h"

#define STM32H553xx

#if !defined(UNUSED)
#define UNUSED(x) ((void)(x))
#endif

void pli_board_init(void);

void pl_board_led_set(uint8_t val);

int pl_board_button_get(void);

#endif
