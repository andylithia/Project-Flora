
#ifdef	TOP
#define	GLOBAL
#else
#define GLOBAL extern
#endif

#include "gdefine.h"
#include "LSYS_logic.h"
#include "stm32f0xx.h"
#include "DS3231.h"
#include "flora_BSP.h"

GLOBAL uint8_t g_freshPON;

GLOBAL uint8_t g_textBuffer[4];
GLOBAL uint8_t g_dpBuffer;
GLOBAL uint32_t	g_decayReg[4];
GLOBAL menu_t* g_currentMenu;

// GLOBAL FLASH_VAR(uint8_t g_EEEMU[FLASH_PAGE_SIZE*2]);
// GLOBAL FLASH_VAR(flash_saving_union g_fsave);
// GLOBAL uint8_t g_job;

GLOBAL LSYS_sys_config_t g_sysCfg;
GLOBAL LSYS_issue_t g_sysIssue;

GLOBAL DS3231_time_t g_time_reg;
// GLOBAL DS3231_alarm_t g_alarm_reg;
GLOBAL DS3231_control_t g_control_reg;

GLOBAL DS3231_time_t g_time_backup;
GLOBAL DS3231_time_t g_time_reference;
GLOBAL uint8_t g_time_last_sec;

GLOBAL pwm_scheme_t g_scheme_reg[8];
GLOBAL uint8_t g_curScheme;

GLOBAL uint8_t g_dim;
GLOBAL uint8_t g_digitDim;
