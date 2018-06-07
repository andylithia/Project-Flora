#include "menu_logic.h"

menu_t* menu_enter(menu_t* current, menu_t* target, void* arg_exit, void* arg_enter){
	if((*(current->mfa_ptr))[EVENT_EXIT]!= NULL)
		(*(current->mfa_ptr))[EVENT_EXIT](arg_exit); 		// Call 'on exit' event
	if((*(target->mfa_ptr))[EVENT_ENTER]!= NULL)
		(*(target->mfa_ptr))[EVENT_ENTER](arg_enter);			// Call 'on enter' event
	return target;
}
