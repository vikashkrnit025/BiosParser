#include "../include/config_parser.h"
#include "../include/setup_menu.h"
#include "../include/validator.h"

int main(void) {
    bios_config_t config;
    
    printf("=================================\n");
    printf("   BIOS Configuration Manager\n");
    printf("=================================\n\n");
    
    printf("Loading system configuration...\n");
    if (parse_bios_config("configs\\default.bin", &config) != BIOS_SUCCESS) {
        printf("Initializing default configuration...\n");
        load_default_config(&config);
        save_bios_config("configs\\default.bin", &config);
    }
    
    printf("\nSystem validation...\n");
    if (validate_cpu_settings(&config) == BIOS_SUCCESS &&
        validate_memory_settings(&config) == BIOS_SUCCESS &&
        validate_boot_settings(&config) == BIOS_SUCCESS) {
        printf("✓ Configuration validation passed\n");
    } else {
        printf("⚠ Configuration has validation warnings\n");
    }
    
    detect_setting_conflicts(&config);
    display_config_summary(&config);
    
    printf("\n=== System Options ===\n");
    printf("1. Enter Setup Utility\n");
    printf("2. View Current Configuration\n");
    printf("3. System Information\n");
    printf("Enter choice (1-3): ");
    
    char choice_str[10];
    if (fgets(choice_str, sizeof(choice_str), stdin) != NULL) {
        int choice = atoi(choice_str);
        
        switch (choice) {
            case 1:
                printf("\nEntering Setup Utility...\n");
                run_setup_utility(&config);
                break;
                
            case 2:
                printf("\nCurrent system configuration:\n");
                display_config_summary(&config);
                break;
                
            case 3:
                printf("\n=== SYSTEM INFORMATION ===\n\n");
                
                printf("Configuration File Format:\n");
                printf("   Signature: %c%c%c%c\n", 
                       config.header.signature[0], config.header.signature[1],
                       config.header.signature[2], config.header.signature[3]);
                printf("   Version: %d\n", config.header.version);
                printf("   Settings Count: %d\n", config.setting_count);
                printf("   Data Integrity: 0x%04X\n\n", config.header.checksum);
                
                printf("System Categories:\n");
                printf("   CPU: Processor configuration and performance\n");
                printf("   Memory: RAM settings and timing\n");
                printf("   Boot: System startup configuration\n");
                printf("   I/O: Input/Output device settings\n\n");
                
                printf("Safety Features:\n");
                printf("   ✓ Value range validation\n");
                printf("   ✓ Configuration conflict detection\n");
                printf("   ✓ Data integrity verification\n");
                
                break;
                
            default:
                printf("Invalid selection. Displaying current configuration:\n");
                display_config_summary(&config);
                break;
        }
    }
    
    printf("\nSystem configuration manager completed.\n");
    printf("Press Enter to exit...");
    getchar();
    
    return 0;
}