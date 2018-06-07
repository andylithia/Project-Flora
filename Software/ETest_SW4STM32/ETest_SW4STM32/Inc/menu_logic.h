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
#define EVENT_INIT		0
#define EVENT_ENTER		1
#define EVENT_EXIT		2
#define EVENT_QUAD		3
#define EVENT_SSEC		4
#define EVENT_ALARM		5
#define EVENT_FAULT		6
#define EVENT_LOOP		7

#define MFA_CALL(MENU_INSTANCE,ENTRY,ARG) (*(MENU_INSTANCE->mfa_ptr))[ENTRY](ARG);

typedef void(*mfa_t[12])(void*);

typedef struct menu* menu_p_t;

typedef struct menu{
	mfa_t* mfa_ptr;
} menu_t;

menu_t* menu_return(menu_t* current, int delta, void* arg_exit, void* arg_enter);
menu_t* menu_enter(menu_t* current, menu_t* target, void* arg_exit, void* arg_enter);

#endif /* _MENU_LOGIC_H */
