#ifndef _MENU_LOGIC_H
#define _MENU_LOGIC_H

#include "stm32f0xx.h"

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
#include "BCG.h"

#define EVENT_INIT		0
#define EVENT_ENTER		1
#define EVENT_EXIT		2
#define EVENT_QUAD		3
#define EVENT_SSEC		4
#define EVENT_ALARM		5
#define EVENT_FAULT		6
#define EVENT_LOOP		7

#define _MFA_CALL(MENU_INSTANCE,ENTRY,ARG) (*(MENU_INSTANCE->mfa_ptr))[ENTRY](ARG)

typedef enum {\
	ISSUE_DISP = BIT(0),		// Request immediate display update
	ISSUE_INPUT = BIT(1),		// Request immediate input check
	ISSUE_LONG = BIT(2),		// Request long processing time
								//  to return, clear this bit
	ISSUE_FEED_DOG = BIT(3),	// Feed the dog
	} LSYS_issue_t;

typedef enum {\
	CONF_TIME_FETCH = BIT(0),	// Fetch new time from DS3231 at T interrupt
	CONF_DISP_INHIBIT = BIT(1),	// Stop automatic display update
	CONF_DOG_UNTIE = BIT(2),	// WatchDog timer enabled
	CONF_DOG_FIERCE = BIT(3),	// Generate System Fault instead of setting a
								//   bit when the dog is not fed
	CONF_LPM_ENABLE = BIT(4),	// Enable Low Power Mode
	} LSYS_sys_config_t;



typedef void(*mfa_t[8])(void*);

typedef struct menu* menu_p_t;

typedef struct menu{
	uint8_t	identifier;	// {4'b1010, D[3:0]}
	mfa_t* mfa_ptr;
} menu_t;

#define MENU_IDN(x) B8(10100000)|((x)&B8(00001111))
#define _VALIDATE_MENU_P(MENU_INSTANCE) ((MENU_INSTANCE->identifier&B8(11110000))==B8(10100000))

void* MFA_CALL(menu_t* menu_p, uint8_t event_type, void* arg);
menu_t* menu_return(menu_t* current, int delta, void* arg_exit, void* arg_enter);
menu_t* menu_enter(menu_t* current, menu_t* target, void* arg_exit, void* arg_enter);



#define FK_A_MFA_HEADER(FK) \
	void FK##_enter(void* arg);\
	void FK##_exit(void* arg);\
	void FK##_quad_event(void* arg);\
	void FK##_tint(void* arg);\
	void FK##_loop(void* arg);

#endif /* _MENU_LOGIC_H */
