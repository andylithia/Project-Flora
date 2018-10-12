#ifndef _GDEFINE_H
#define _GDEFINE_H

#include "BCG.h"
#include "stm32f0xx.h"
// #include "flora_BSP.h"

#define FLASH_VAR(x) const x __attribute__((aligned(FLASH_PAGE_SIZE)))

typedef struct {
	uint16_t R;
	uint16_t G;
	uint16_t B;
} RGB_t;

typedef struct {
	uint16_t	IV9_pValue[4];
	RGB_t		LED_pValue;
} pwm_scheme_t;

#endif /* _GDEFINE_H */
