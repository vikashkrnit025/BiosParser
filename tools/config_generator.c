/*
 * System Configuration Generator
 * Creates preset configuration files for testing
 */
#include "../include/config_parser.h"

void create_performance_config(bios_config_t *config) {
    load_default_config(config);
    
    // Optimize for performance
    uint32_t cpu_freq = 4200;  // Higher processor frequency
    uint32_t mem_freq = 3200;  // Faster memory
    uint8_t fast_boot = 1;     // Enable fast boot
    
    set_setting_by_name(config, "CPU_FREQ", &cpu_freq);
    set_setting_by_name(config, "MEMORY_FREQ", &mem_freq);
    set_setting_by_name(config, "FAST_BOOT", &fast_boot);
    
    printf("Performance configuration created\n");
}

void create_stability_config(bios_config_t *config) {
    load_default_config(config);
    
    // Optimize for stability
    uint32_t cpu_freq = 3000;  // Conservative processor frequency
    uint32_t mem_freq = 2400;  // Stable memory frequency
    uint8_t ecc_enable = 1;    // Enable ECC
    uint8_t mem_test = 1;      // Enable memory testing
    
    set_setting_by_name(config, "CPU_FREQ", &cpu_freq);
    set_setting_by_name(config, "MEMORY_FREQ", &mem_freq);
    set_setting_by_name(config, "ECC_ENABLE", &ecc_enable);
    set_setting_by_name(config, "MEMORY_TEST", &mem_test);
    
    printf("Stability configuration created\n");
}

int main(void) {
    bios_config_t config;
    
    printf("System Configuration Generator\n");
    printf("==============================\n\n");
    
    // Create default configuration
    printf("Creating default.bin...\n");
    load_default_config(&config);
    save_bios_config("configs\\default.bin", &config);
    
    // Create performance configuration
    printf("Creating performance.bin...\n");
    create_performance_config(&config);
    save_bios_config("configs\\performance.bin", &config);
    
    // Create stability configuration
    printf("Creating stability.bin...\n");
    create_stability_config(&config);
    save_bios_config("configs\\stability.bin", &config);
    
    printf("\nConfiguration files created successfully!\n");
    printf("Use these files to test the configuration parser.\n");
    
    return 0;
}