#include <LSYS_logic.h>

menu_t* menu_enter(menu_t* current, menu_t* target, void* arg_exit, void* arg_enter){
	if((*(current->mfa_ptr))[EVENT_EXIT]!= NULL)
		(*(current->mfa_ptr))[EVENT_EXIT](arg_exit); 		// Call 'on exit' event
	if((*(target->mfa_ptr))[EVENT_ENTER]!= NULL)
		(*(target->mfa_ptr))[EVENT_ENTER](arg_enter);			// Call 'on enter' event
	return target;
}

void* MFA_CALL(menu_t* menu_p, uint8_t event_type, void* arg){
	if(_VALIDATE_MENU_P(menu_p) && menu_p->mfa_ptr[event_type]!=NULL) {
		_MFA_CALL(menu_p,event_type,arg);
		return (void*)(menu_p->mfa_ptr[event_type]);
	} else
		return NULL;
}
