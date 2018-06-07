#ifndef _MENU_L0_INTRO_H
#define _MENU_L0_INTRO_H

#include "menu_logic.h"

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
static void menu_l0_intro_enter(void* arg);
static void menu_l0_intro_exit(void* arg);
static void menu_l0_intro_quad_event(void* arg);
static void menu_l0_intro_tint(void* arg);
static void menu_l0_intro_system_fault(void* arg);
static void menu_l0_intro_loop(void* arg);


#endif /* _MENU_L0_INTRO_H */
