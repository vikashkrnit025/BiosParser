#include "../include/setup_menu.h"

// Console management functions
void clear_screen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void print_header(const char *title) {
    printf("\n");
    printf("=====================================\n");
    printf("         %s\n", title);
    printf("=====================================\n");
}

void print_separator(void) {
    printf("-------------------------------------\n");
}

void wait_for_keypress(void) {
    printf("\nPress Enter to continue...");
    getchar();
}

// Main menu display
void display_main_menu(void) {
    clear_screen();
    print_header("System Setup Utility");
    printf("1. CPU Configuration\n");
    printf("2. Memory Settings\n");
    printf("3. Boot Configuration\n");
    printf("4. I/O Settings\n");
    printf("5. View All Settings\n");
    printf("6. Reset to Defaults\n");
    printf("7. Save Configuration\n");
    printf("8. Load Configuration\n");
    printf("9. Exit\n");
    print_separator();
    printf("Select option (1-9): ");
}

// Category menu display
void display_category_menu(bios_category_t category) {
    clear_screen();
    print_header(get_category_name(category));
    printf("Category: %s Configuration\n", get_category_name(category));
    print_separator();
    printf("1. View %s Settings\n", get_category_name(category));
    printf("2. Modify %s Settings\n", get_category_name(category));
    printf("3. Back to Main Menu\n");
    print_separator();
    printf("Select option (1-3): ");
}

// Get user input functions
int get_user_input_uint32(const char *prompt, uint32_t min_val, uint32_t max_val, uint32_t *result) {
    char buffer[64];
    uint32_t value;
    
    printf("%s (Range: %u-%u): ", prompt, min_val, max_val);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return -1;
    }
    
    if (sscanf(buffer, "%u", &value) != 1) {
        printf("Error: Invalid number format\n");
        return -1;
    }
    
    if (value < min_val || value > max_val) {
        printf("Error: Value %u is out of range (%u-%u)\n", value, min_val, max_val);
        return -1;
    }
    
    *result = value;
    return 0;
}

int get_user_input_string(const char *prompt, char *buffer, int buffer_size) {
    printf("%s: ", prompt);
    if (fgets(buffer, buffer_size, stdin) == NULL) {
        return -1;
    }
    
    // Remove newline if present
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n') {
        buffer[len-1] = '\0';
    }
    
    return 0;
}

int get_user_input_bool(const char *prompt, int *result) {
    char buffer[64];
    printf("%s (y/n): ", prompt);
    
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return -1;
    }
    
    if (buffer[0] == 'y' || buffer[0] == 'Y') {
        *result = 1;
        return 0;
    } else if (buffer[0] == 'n' || buffer[0] == 'N') {
        *result = 0;
        return 0;
    } else {
        printf("Error: Please enter 'y' or 'n'\n");
        return -1;
    }
}

// Setting modification
int modify_setting_interactive(bios_config_t *config, const char *setting_name) {
    bios_setting_t setting;
    if (get_setting_by_name(config, setting_name, &setting) != BIOS_SUCCESS) {
        printf("Error: Setting '%s' not found\n", setting_name);
        return -1;
    }
    
    printf("\nCurrent setting:\n");
    display_setting(&setting);
    printf("\n");
    
    switch (setting.type) {
        case BIOS_TYPE_UINT32: {
            uint32_t new_value;
            if (get_user_input_uint32("Enter new value", setting.min_value, setting.max_value, &new_value) == 0) {
                if (set_setting_by_name(config, setting_name, &new_value) == BIOS_SUCCESS) {
                    printf("Setting updated successfully\n");
                    return 0;
                }
            }
            break;
        }
        case BIOS_TYPE_BOOL: {
            int new_value;
            if (get_user_input_bool("Enable setting", &new_value) == 0) {
                uint8_t bool_val = new_value ? 1 : 0;
                if (set_setting_by_name(config, setting_name, &bool_val) == BIOS_SUCCESS) {
                    printf("Setting updated successfully\n");
                    return 0;
                }
            }
            break;
        }
        default:
            printf("Error: Setting type not supported for modification\n");
            return -1;
    }
    
    printf("Failed to update setting\n");
    return -1;
}

// Menu handlers
int handle_category_menu_input(bios_config_t *config, bios_category_t category) {
    int choice;
    char buffer[64];
    
    while (1) {
        display_category_menu(category);
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            continue;
        }
        
        if (sscanf(buffer, "%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            wait_for_keypress();
            continue;
        }
        
        switch (choice) {
            case 1:
                clear_screen();
                display_category_settings(config, category);
                wait_for_keypress();
                break;
                
            case 2: {
                clear_screen();
                printf("Settings available for modification:\n");
                display_category_settings(config, category);
                
                char setting_name[MAX_SETTING_NAME];
                printf("\nEnter setting name to modify (or 'back' to return): ");
                if (fgets(setting_name, sizeof(setting_name), stdin) != NULL) {
                    // Remove newline
                    size_t len = strlen(setting_name);
                    if (len > 0 && setting_name[len-1] == '\n') {
                        setting_name[len-1] = '\0';
                    }
                    
                    if (strcmp(setting_name, "back") == 0) {
                        break;
                    }
                    
                    modify_setting_interactive(config, setting_name);
                    wait_for_keypress();
                }
                break;
            }
            
            case 3:
                return 0;  // Back to main menu
                
            default:
                printf("Invalid choice. Please select 1-3.\n");
                wait_for_keypress();
                break;
        }
    }
}

int handle_main_menu_input(bios_config_t *config) {
    int choice;
    char buffer[64];
    char filename[256];
    
    while (1) {
        display_main_menu();
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            continue;
        }
        
        if (sscanf(buffer, "%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            wait_for_keypress();
            continue;
        }
        
        switch (choice) {
            case 1:
                handle_category_menu_input(config, CATEGORY_CPU);
                break;
                
            case 2:
                handle_category_menu_input(config, CATEGORY_MEMORY);
                break;
                
            case 3:
                handle_category_menu_input(config, CATEGORY_BOOT);
                break;
                
            case 4:
                handle_category_menu_input(config, CATEGORY_IO);
                break;
                
            case 5:
                clear_screen();
                display_config_summary(config);
                wait_for_keypress();
                break;
                
            case 6:
                load_default_config(config);
                printf("Default configuration restored.\n");
                wait_for_keypress();
                break;
                
            case 7:
                printf("Enter filename to save (e.g., 'configs\\my_config.bin'): ");
                if (get_user_input_string("", filename, sizeof(filename)) == 0) {
                    if (save_bios_config(filename, config) == BIOS_SUCCESS) {
                        printf("Configuration saved successfully.\n");
                    } else {
                        printf("Failed to save configuration.\n");
                    }
                }
                wait_for_keypress();
                break;
                
            case 8:
                printf("Enter filename to load (e.g., 'configs\\default.bin'): ");
                if (get_user_input_string("", filename, sizeof(filename)) == 0) {
                    if (parse_bios_config(filename, config) == BIOS_SUCCESS) {
                        printf("Configuration loaded successfully.\n");
                    } else {
                        printf("Failed to load configuration.\n");
                    }
                }
                wait_for_keypress();
                break;
                
            case 9:
                printf("\nExiting Setup Utility...\n");
                return 0;
                
            default:
                printf("Invalid choice. Please select 1-9.\n");
                wait_for_keypress();
                break;
        }
    }
}

// Main setup utility function
int run_setup_utility(bios_config_t *config) {
    printf("Entering Setup Utility...\n");
    printf("Press Enter to continue...");
    getchar();
    
    return handle_main_menu_input(config);
}