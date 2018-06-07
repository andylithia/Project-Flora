#ifdef	TOP
#define	GLOBAL
#else
#define GLOBAL extern
#endif

#include "menu_logic.h"
#include "stm32f0xx.h"

GLOBAL uint8_t obuffer[4];
