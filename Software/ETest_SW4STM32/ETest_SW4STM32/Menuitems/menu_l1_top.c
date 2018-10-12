#include "gvar.h"
#include "BCG.h"
#include "menu_l1_top.h"
#include "stm32f0xx.h"
#include "flora_BSP.h"
#include "string.h"
#include "BCDUtil.h"

#define FADE_STEP	1
#define PCHG_DELAY	200

const uint8_t nameList[] = "TIMDATCSTSET";
const uint8_t wkDay[]= "!ERR MON TUE WED THU FRI SAT SUN!ERR";
const uint8_t monEditLimit[] = {0x99,0x31,0x29,0x31,0x30,0x31,0x30,0x31,0x31,0x30,0x31,0x30,0x31};
const uint8_t WeekTab[] = {\
		(3 << 5) + 31, (6 << 5) + 29, (0 << 5) + 31, (3 << 5) + 30,\
		(5 << 5) + 31, (1 << 5) + 30, (3 << 5) + 31, (6 << 5) + 31,\
		(1 << 5) + 30, (4 << 5) + 31, (0 << 5) + 30, (2 << 5) + 31
};
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern I2C_HandleTypeDef hi2c1;

mfa_t mfa_l1_top = {
		NULL,
		menu_l1_top_enter,
		menu_l1_top_exit,
		menu_l1_top_quad_event,
		menu_l1_top_tint,
		NULL,
		NULL,
		menu_l1_top_loop
};

menu_t menu_l1_top = {
		MENU_IDN(2),
		&mfa_l1_top
};

uint8_t f_entering;

enum pageSwAnim_fsm_e {PS_STABLE, PS_PCHG_DELAY, PS_FADE_OUT, PS_FADE_IN}\
	pageSwAnim_fsm;
uint16_t pchgDelay;

enum curPage_e {PAGE_TIM, PAGE_DAT, PAGE_CST, PAGE_SET} curPage;
enum dateDisp_e {DDISP_MON_DATE, DDISP_YEAR, DDISP_WDAY} dateDisp;
uint8_t	curLevel;

uint8_t digitEditPtr; // Left most digit :0
extern uint8_t IV9_dimming_LUT[4];

/** -private function prototypes- **/

void qproc_kdn();
void qproc_rf();
void qproc_rr();
void qproc_lkdn();

void tproc_tim();
void tproc_dat();

/** ---------- **/

void menu_l1_top_enter(void* crap){
	f_entering = 1;
	g_sysCfg|=CONF_TIME_FETCH;

}

void menu_l1_top_exit(void* arg){
	g_sysCfg&=~CONF_TIME_FETCH;
}

void menu_l1_top_quad_event(void* arg){
	uint8_t karg = *((uint8_t*)arg);
	// Break the ongoing animation
	if(pageSwAnim_fsm!=PS_STABLE){
		pageSwAnim_fsm = PS_STABLE;
		for(int i=0;i<4;i++){
			g_decayReg[i] = IV9_PULSE_MAX;
			IV9_FastPWMSet(&htim2, BIT(IV9_dimming_LUT[i]),\
				IV9_MixDimming(g_decayReg, i));
		}
		pchgDelay = 0;
		g_dpBuffer &= ~BIT(3);
	}

	// Dispatch to key processing functions
	if(karg&BIT(QINF_BIT_KDN)){
		if(karg&BIT(QINF_BIT_LONG_PRESS))
			qproc_lkdn();
		else
			qproc_kdn();
	}
	else if(karg&BIT(QINF_BIT_ROT)){
		if(karg&BIT(QINF_BIT_FW_RVN))
			qproc_rf();
		else
			qproc_rr();
	}
	*(uint8_t*)arg = 0;
}

void qproc_kdn(){
	uint8_t mon_t;
	uint8_t date_t;
	uint8_t year_t;
	if(curPage==PAGE_TIM){
		if(curLevel == 0){
			// Toggle DAY/NIGHT Lighting Scheme
			if(g_dim){
				for(int i=0; i<4; i++)
					g_scheme_reg[g_curScheme].IV9_pValue[i] = IV9_PULSE_MAX/4;
				g_scheme_reg[g_curScheme].LED_pValue.R = 0;
				g_scheme_reg[g_curScheme].LED_pValue.G = RGB_PULSE_MAX/32;
				g_scheme_reg[g_curScheme].LED_pValue.B = RGB_PULSE_MAX/16;
			}else{
				for(int i=0; i<4; i++)
					g_scheme_reg[g_curScheme].IV9_pValue[i] = IV9_PULSE_MAX;
				g_scheme_reg[g_curScheme].LED_pValue.R = RGB_PULSE_MAX;
				g_scheme_reg[g_curScheme].LED_pValue.G = RGB_PULSE_MAX/4;
				g_scheme_reg[g_curScheme].LED_pValue.B = 0;
			}
			g_dim = !g_dim;
		} else if (curLevel == 1){
			if(digitEditPtr == 1){
				// Done, save the current time and return
				g_time_reg.SEC = 0;
				DS3231_SetTime(&hi2c1, &g_time_reg, 0xFF);
				curLevel = 0;
				g_sysCfg|=CONF_TIME_FETCH;					// Restore RTC Sync
			} else {
				digitEditPtr++;
			}
		}

	} else if (curPage == PAGE_DAT){
		if(curLevel == 0){
			// Toggle MM.DD / YEAR / WKD
			dateDisp = dateDisp==DDISP_WDAY?DDISP_MON_DATE:dateDisp+1;
		} else if (curLevel == 1){
			if(digitEditPtr == 1){
				/*
				switch(dateDisp){
				case DDISP_MON_DATE:
					mon_t = g_time_reg.MONTH;
					date_t = g_time_reg.DATE;
					DS3231_GetTime(&hi2c1, &g_time_reg, 0xFF);
					g_time_reg.MONTH = mon_t;
					g_time_reg.DATE = date_t;
					break;
				case DDISP_YEAR:
					year_t = g_time_reg.YEAR;
					DS3231_GetTime(&hi2c1, &g_time_reg, 0xFF);
					g_time_reg.YEAR = year_t;
					break;
				default:
					__NOP();
				}
				*/
				g_time_reg.DAY =
					GetWeekDay(BCD2DEC(g_time_reg.YEAR),\
							BCD2DEC(g_time_reg.MONTH),\
							BCD2DEC(g_time_reg.DATE))+0x01;

				DS3231_SetTime(&hi2c1, &g_time_reg, 0xFF);
				curLevel = 0;
				g_sysCfg|=CONF_TIME_FETCH;
			} else {
				digitEditPtr++;
			}
		}
	}
} /* qproc_kdn */

void qproc_lkdn(){
	if(curPage == PAGE_TIM){
		if(curLevel == 0){
			// Enter Time Editing Mode
			g_sysCfg&=~CONF_TIME_FETCH; // Stop time updating
			digitEditPtr = 0;
			curLevel = 1;
		} else if (curLevel == 1){
			// Give Up current Edit
			DS3231_GetTime(&hi2c1, &g_time_reg, 0xFF);
			digitEditPtr = 0;
		}

	} else if (curPage == PAGE_DAT) {
		if(curLevel == 0){
			// Enter Date Edit Mode
			if(dateDisp!=DDISP_WDAY){
				g_sysCfg&=~CONF_TIME_FETCH;
				if(dateDisp==DDISP_MON_DATE)
					digitEditPtr = 0;
				else if (dateDisp == DDISP_YEAR)
					digitEditPtr = 1;
				curLevel = 1;
			}
		} else if (curLevel == 1){
			// Give Up current Edit
			DS3231_GetTime(&hi2c1, &g_time_reg, 0xFF);
			if(dateDisp==DDISP_MON_DATE)
					digitEditPtr = 0;
			else if (dateDisp == DDISP_YEAR)
					digitEditPtr = 1;
		}
	}
} /* qproc_lkdn */

void qproc_rf(){
	// CHANGING PAGE
	if(curLevel==0){
		pageSwAnim_fsm = PS_PCHG_DELAY;
		curPage = curPage==PAGE_DAT?PAGE_TIM:curPage+1;
	} else if(curPage == PAGE_TIM){
		// incr the current digit
		if(!digitEditPtr)
				g_time_reg.HOUR.HOUR.HOUR_24 = \
					PBCDADD(g_time_reg.HOUR.HOUR.HOUR_24, 0x01);
		else 	g_time_reg.MIN = PBCDADD(g_time_reg.MIN, 0x01);

		if(g_time_reg.HOUR.HOUR.HOUR_24>=0x24)
			g_time_reg.HOUR.HOUR.HOUR_24 = 0;
		if(g_time_reg.MIN >= 0x60)
			g_time_reg.MIN = 0;

	} else if(curPage == PAGE_DAT){
		// incr the current digit
		if(dateDisp==DDISP_MON_DATE){
			if(!digitEditPtr)
				g_time_reg.MONTH = (PBCDADD(g_time_reg.MONTH, 0x01))&0x1F;
			else
				g_time_reg.DATE = (PBCDADD(g_time_reg.DATE, 0x01))&0x3F;

			if(g_time_reg.MONTH == 0x13)
				g_time_reg.MONTH = 0x01;

			if(g_time_reg.DATE > monEditLimit[BCD2DEC(g_time_reg.MONTH)])
				g_time_reg.DATE = 0x01;

			if(g_time_reg.MONTH == 0x02)
				if((!LYEAR_CHECK(BCD2DEC(g_time_reg.YEAR))) && g_time_reg.DATE > 0x28)
					g_time_reg.DATE = 0x01;

		} else if (dateDisp == DDISP_YEAR){
			g_time_reg.YEAR = PBCDADD(g_time_reg.YEAR, 0x01);

			if(g_time_reg.MONTH == 0x02)
				if((!LYEAR_CHECK(BCD2DEC(g_time_reg.YEAR))) && g_time_reg.DATE > 0x28)
					g_time_reg.DATE = 0x28;
		}
	}
} /* qproc_rf */

void qproc_rr(){
	// CHANGING PAGE
	if(curLevel==0){
		pageSwAnim_fsm = PS_PCHG_DELAY;
		curPage = curPage==PAGE_TIM?PAGE_DAT:curPage-1;
	} else if(curPage == PAGE_TIM){
		// decr the current digit
		if(!digitEditPtr)
			g_time_reg.HOUR.HOUR.HOUR_24 = \
					PBCDSUB(g_time_reg.HOUR.HOUR.HOUR_24, 0x01);
		else 	g_time_reg.MIN = PBCDSUB(g_time_reg.MIN, 0x01);

		if(g_time_reg.HOUR.HOUR.HOUR_24 == 0x00)
			g_time_reg.HOUR.HOUR.HOUR_24 = 0x23;
		if(g_time_reg.MIN == 0x00)
			g_time_reg.MIN = 0x59;
	} else if(curPage == PAGE_DAT){
		// decr the current digit
		if(dateDisp==DDISP_MON_DATE){
			if(!digitEditPtr)
				g_time_reg.MONTH = (PBCDSUB(g_time_reg.MONTH, 0x01))&0x1F;
			else
				g_time_reg.DATE = (PBCDSUB(g_time_reg.DATE, 0x01))&0x3F;

			if(g_time_reg.MONTH == 0)
				g_time_reg.MONTH = 0x12;

			if(g_time_reg.DATE > monEditLimit[BCD2DEC(g_time_reg.MONTH)]\
					||g_time_reg.DATE == 0)
				g_time_reg.DATE = monEditLimit[BCD2DEC(g_time_reg.MONTH)];

			if(g_time_reg.MONTH == 0x02)
				if((!LYEAR_CHECK(BCD2DEC(g_time_reg.YEAR)))\
						&& g_time_reg.DATE > 0x28)
					g_time_reg.DATE = 0x28;
		} else if (dateDisp == DDISP_YEAR){
			g_time_reg.YEAR = PBCDSUB(g_time_reg.YEAR, 0x01);

			if(g_time_reg.MONTH == 0x02)
				if((!LYEAR_CHECK(BCD2DEC(g_time_reg.YEAR)))\
						&& g_time_reg.DATE > 0x28)
					g_time_reg.DATE = 0x28;
		}
	}
} /* qproc_rr */

/*** Timer Interrupt handler, executed at 2.5kHz ***/
void menu_l1_top_tint(void* arg){
	/*-----  ANIMATION CONTROL  -----*/
	if(f_entering){
		for(int i=0; i<4; i++){
			IV9_LinFadeOut(g_decayReg+i, FADE_STEP);
		}
		if(g_decayReg[0]==0&&g_decayReg[3]==0){
			f_entering = 0;
			pageSwAnim_fsm = PS_FADE_IN;
			memset(g_decayReg, 0, sizeof(g_decayReg));
			g_dim = 0;
			for(int i=0; i<4; i++)
				g_scheme_reg[g_curScheme].IV9_pValue[i] = IV9_PULSE_MAX;
			g_scheme_reg[g_curScheme].LED_pValue.R = RGB_PULSE_MAX;
			g_scheme_reg[g_curScheme].LED_pValue.G = RGB_PULSE_MAX/4;
			g_scheme_reg[g_curScheme].LED_pValue.B = 0;
		}
	} else {
		/*** State #1: Prechange Delay, Displaying the entry name ***/
		if (pageSwAnim_fsm==PS_PCHG_DELAY){
			if(pchgDelay==0){
				memcpy(g_textBuffer+1,nameList+3*curPage,3);
				g_textBuffer[0]='0'+curPage+1;
				g_dpBuffer = BIT(3);
			}
			if(pchgDelay<PCHG_DELAY)
				pchgDelay++;
			else {
				pchgDelay = 0;
				pageSwAnim_fsm = PS_FADE_OUT;
			}
		/*** State #2: Fading out ***/
		} else if (pageSwAnim_fsm == PS_FADE_OUT){
			for(int i=0;i<4;i++)
				IV9_LinFadeOut(g_decayReg+i, FADE_STEP<<1);
			if(g_decayReg[0]==0){
				pageSwAnim_fsm = PS_FADE_IN;
				g_dpBuffer &= ~BIT(3);
			}
		} else {
			/*** State #0: Stabled, execute code & generate display data ***/
			switch(curPage){
			case PAGE_TIM: tproc_tim(); break;
			case PAGE_DAT: tproc_dat(); break;
			default:
				curPage = PAGE_TIM;
			}
			/*** State #3: Fading in ***/
			if(pageSwAnim_fsm==PS_FADE_IN){
				for(int i=0;i<4;i++)
					IV9_LinFadeIn(g_decayReg+i, FADE_STEP);
				if(g_decayReg[0]==IV9_PULSE_MAX)
					pageSwAnim_fsm = PS_STABLE;
			}
		}
	}


	/*-----  DIGIT DIMMING CONTROL  -----*/
	g_digitDim=0;
	if(curLevel!=0){
		if(digitEditPtr)	g_digitDim |= BIT(2)|BIT(3);
		else				g_digitDim |= BIT(0)|BIT(1);
		g_digitDim=~g_digitDim;
	}


	/*-----  PWM PVALUE MODIFYING  -----*/
	for(int i=0;i<4;i++){
		if(g_digitDim&BIT(i))
			IV9_FastPWMSet(&htim2, BIT(IV9_dimming_LUT[i]),\
				IV9_MixDimming_Div(1, g_decayReg, i));
		else
			IV9_FastPWMSet(&htim2, BIT(IV9_dimming_LUT[i]),\
				IV9_MixDimming_Div(0, g_decayReg, i));
	}
	RGB_FastPWMSet_D(&htim1, RGB_MixDimming_D(g_decayReg[0]));
} /* menu_l1_top_tint */


void menu_l1_top_loop(void* arg){
	__NOP();
}

void tproc_tim(){
	// Level 0, display the time
	DS3231_ExtractBCD(g_textBuffer, g_time_reg.HOUR.HOUR.HOUR_24, 0);
	DS3231_ExtractBCD(g_textBuffer+2, g_time_reg.MIN, 0);
	if(curLevel == 0){
		if(g_time_reg.SEC&1) g_dpBuffer = BIT(0);
		else g_dpBuffer = 0;
	} else {
		g_dpBuffer = 0;
		if(digitEditPtr)	g_dpBuffer = BIT(1)|BIT(0);
		else				g_dpBuffer = BIT(3)|BIT(2);
	}
	g_textBuffer[0]+='0';
	g_textBuffer[1]+='0';
	g_textBuffer[2]+='0';
	g_textBuffer[3]+='0';
}

void tproc_dat(){
		if(dateDisp==DDISP_MON_DATE){
			DS3231_ExtractBCD(g_textBuffer, g_time_reg.MONTH, 0);
			DS3231_ExtractBCD(g_textBuffer+2, g_time_reg.DATE, 0);
			g_textBuffer[0]+='0';
			g_textBuffer[1]+='0';
			g_textBuffer[2]+='0';
			g_textBuffer[3]+='0';
		} else if(dateDisp == DDISP_YEAR){
			DS3231_ExtractBCD(g_textBuffer+2, g_time_reg.YEAR, 0);
			g_textBuffer[0]='2';
			g_textBuffer[1]='0';
			g_textBuffer[2]+='0';
			g_textBuffer[3]+='0';
		} else
			memcpy(g_textBuffer, wkDay+4*g_time_reg.DAY, 4);

		g_dpBuffer = 0;
		if(!curLevel == 0){
			if(digitEditPtr)	g_dpBuffer = BIT(1)|BIT(0);
			else				g_dpBuffer = BIT(3)|BIT(2);
		} else if (dateDisp == DDISP_MON_DATE)
			g_dpBuffer |= BIT(2);
}
