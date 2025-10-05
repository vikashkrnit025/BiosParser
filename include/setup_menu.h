#ifndef SETUP_MENU_H
#define SETUP_MENU_H

#include "bios_types.h"
#include "config_parser.h"
#include "validator.h"

// Menu system functions
void display_main_menu(void);
void display_category_menu(bios_category_t category);
int handle_main_menu_input(bios_config_t *config);
int handle_category_menu_input(bios_config_t *config, bios_category_t category);

// Setting modification functions
int modify_setting_interactive(bios_config_t *config, const char *setting_name);
int get_user_input_uint32(const char *prompt, uint32_t min_val, uint32_t max_val, uint32_t *result);
int get_user_input_string(const char *prompt, char *buffer, int buffer_size);
int get_user_input_bool(const char *prompt, int *result);

// Menu display helpers
void print_header(const char *title);
void print_separator(void);
void wait_for_keypress(void);
void clear_screen(void);

// Setup utility main function
int run_setup_utility(bios_config_t *config);

#endif // SETUP_MENU_H