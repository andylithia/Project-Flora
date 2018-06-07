#include "gvar.h"
#include "BCG.h"
#include "menu_l0_intro.h"
#include "stm32f0xx.h"

#define DECAY_STEP_PERIOD 10

/* Menu function pointer array type
*  Standard form of a menu function: void name(void*){}
*  Assignment:
*	#00. parent menu init
*	#01. enter
*	#02. exit
*	#03. quad_event
*	#04. const 100ms
*	#05. alarm
*	#06. system fault
*	#07. loop
*/
mfa_t mfa_l0_intro = {
	NULL,
	menu_l0_intro_enter,
	menu_l0_intro_exit,
	menu_l0_intro_quad_event,
	menu_l0_intro_tint,
	NULL,
	menu_l0_intro_system_fault,
	menu_l0_intro_loop
};
uint8_t		d_pos;
uint8_t		decay_counters[4];
const static uint8_t intro_text[] =
		"GREETINGS!\n THISIS ASPECIAL PRESENT FOR MY  DEARIE\n READTHE USERMANUAL  FOR MORE\n";
static void menu_l0_intro_enter(void* arg);

static void menu_l0_intro_exit(void* arg);

static void menu_l0_intro_quad_event(void* arg){
	// exit;
}

static void menu_l0_intro_tint(void* arg){

}

static void menu_l0_intro_system_fault(void* arg){
	// Force System Reset
	NVIC_SETFAULTMASK();
	GenerateSystemReset();
}

static void menu_l0_intro_loop(void* arg){

}

