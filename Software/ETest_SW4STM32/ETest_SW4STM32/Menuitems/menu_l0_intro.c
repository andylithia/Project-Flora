#include "gvar.h"
#include "BCG.h"
#include "menu_l0_intro.h"
#include "stm32f0xx.h"
#include "flora_BSP.h"

#define DECAY_PERIOD 16
#define STEP_PERIOD 12
#define INTER_WORD_PERIOD 30

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;


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
	// menu_l0_intro_system_fault,
	NULL,

	menu_l0_intro_loop
};

menu_t menu_l0_intro = {
		MENU_IDN(1),
		&mfa_l0_intro
};

extern uint8_t	IV9_dimming_LUT[4];
extern menu_t menu_l1_top;

uint8_t		c_pos;
uint8_t		d_pos;
uint8_t		decayCount;
uint8_t		stepCount;
uint8_t		f_wordEnd;
uint8_t		f_lineEnd;

enum {ST_DONE, ST_DISPLAY, ST_ROTATE, ST_SPRESS, ST_ERR} selfTest_state;
// const uint8_t intro_text[] =
//		"GREETINGS\n THIS IS A SPECIAL PRESENT FOR MY DEARIE\nHAPPY BIRTH DAY!\n";
const uint8_t selfTest_text[]="8888    V023    ABCD    \0";
const uint8_t intro_text[]=
		"    HI !    THISIS ASPECIAL GIFT    FOR MY  DEAR    HAPPY  BIRTHDAY!        ";
void menu_l0_intro_enter(void* arg){
	g_decayReg[0] = 0;
	g_decayReg[1] = 0;
	g_decayReg[2] = 0;
	g_decayReg[3] = 0;

	g_scheme_reg[g_curScheme].LED_pValue.R = 0;
	g_scheme_reg[g_curScheme].LED_pValue.G = 0;
	g_scheme_reg[g_curScheme].LED_pValue.B = 0;

	decayCount = 0;
	c_pos = 0;
	d_pos = 0;
	selfTest_state = ST_DISPLAY;
}

void menu_l0_intro_exit(void* arg){

}

void menu_l0_intro_quad_event(void* arg){
	// on fw: move to line end, or go to the next line
	// on rv: replay line, or go to previous line
	if(*((uint8_t*)arg)&BIT(QINF_BIT_LONG_PRESS)) {
		if(selfTest_state == ST_DONE)
			g_currentMenu = menu_enter(g_currentMenu, &menu_l1_top, NULL, NULL);
		else {
			menu_l0_intro_enter(NULL);
			selfTest_state = ST_DONE;
		}
	}
}

void menu_l0_intro_tint(void* arg){
	// memcpy(g_textBuffer, "FUCK",4);
	if(decayCount>=DECAY_PERIOD){
		for(int i=0;i<4;i++){
			IV9_QuadDecay(&(g_decayReg[i]));
			IV9_FastPWMSet(&htim2, BIT(IV9_dimming_LUT[i]),\
					IV9_MixDimming(g_decayReg, i));
		}
		RGB_FastPWMSet_D(&htim1, RGB_MixDimming_D((g_decayReg[0]+g_decayReg[1]+g_decayReg[2]+g_decayReg[3])>>2));
		stepCount++;
		decayCount=0;
	} else
		decayCount++;

	// RGB_FastPWMSet(&htim1, color);
}

void menu_l0_intro_loop(void* arg){
	//if(f_wordEnd){
		// f_wordEnd = 0;
	//} else {
		if(stepCount >= STEP_PERIOD){
			stepCount = 0;
			if(selfTest_state==ST_DONE){
				g_dpBuffer = 0;
				if(intro_text[d_pos+(c_pos<<2)]!=' '){
					g_decayReg[d_pos] = IV9_PULSE_MAX;
					g_textBuffer[d_pos] = intro_text[d_pos+(c_pos<<2)];
					if (intro_text[d_pos+(c_pos<<2)]=='\0')
						g_currentMenu = menu_enter(g_currentMenu, &menu_l1_top, NULL, NULL);
				}

			} else {
				g_dpBuffer = B8(00001111);
				if(selfTest_text[d_pos+(c_pos<<2)]!=' '){
					if (selfTest_text[d_pos+(c_pos<<2)]=='\0'){
						d_pos = 0;
						c_pos = 0;
					}
					g_decayReg[d_pos] = IV9_PULSE_MAX;
					g_textBuffer[d_pos] = selfTest_text[d_pos+(c_pos<<2)];
				}

				switch (c_pos){
				case 0:
					g_scheme_reg[g_curScheme].LED_pValue.G = 0;
					g_scheme_reg[g_curScheme].LED_pValue.B = 0;
					g_scheme_reg[g_curScheme].LED_pValue.R = RGB_PULSE_MAX;
					break;
				case 2:
					g_scheme_reg[g_curScheme].LED_pValue.R = 0;
					g_scheme_reg[g_curScheme].LED_pValue.B = 0;
					g_scheme_reg[g_curScheme].LED_pValue.G = RGB_PULSE_MAX;
					break;
				case 4:
					g_scheme_reg[g_curScheme].LED_pValue.R = 0;
					g_scheme_reg[g_curScheme].LED_pValue.G = 0;
					g_scheme_reg[g_curScheme].LED_pValue.B = RGB_PULSE_MAX;
					break;
				}
			}

			if(d_pos==3){
				c_pos++;
				d_pos=0;
			} else
				d_pos++;
		}
	//}
}

