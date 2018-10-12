#ifndef MENU_L1_TOP_H_
#define MENU_L1_TOP_H_

typedef struct {
	uint8_t* name_short;
	uint8_t* name_long;
	menu_p_t menu_instance_p;
	void*	disp_update;
} top_menu_entry_t;

void menu_l1_top_enter(void* arg);
void menu_l1_top_exit(void* arg);
void menu_l1_top_quad_event(void* arg);
void menu_l1_top_tint(void* arg);
void menu_l1_top_system_fault(void* arg);
void menu_l1_top_loop(void* arg);

#endif /* MENU_L1_TOP_H_ */
