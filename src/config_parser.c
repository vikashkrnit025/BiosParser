#include "../include/config_parser.h"

// Calculate checksum for data integrity
uint16_t calculate_checksum(const bios_config_t *config) {
    uint16_t checksum = 0;
    const uint8_t *data = (const uint8_t*)config->settings;
    size_t size = sizeof(bios_setting_t) * config->setting_count;
    
    for (size_t i = 0; i < size; i++) {
        checksum += data[i];
    }
    return checksum;
}

// Parse configuration file
int parse_bios_config(const char *filename, bios_config_t *config) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Cannot open file %s\n", filename);
        return BIOS_ERROR_FILE;
    }
    
    // Read header
    if (fread(&config->header, sizeof(bios_config_header_t), 1, file) != 1) {
        fclose(file);
        printf("Error: Cannot read header from %s\n", filename);
        return BIOS_ERROR_FORMAT;
    }
    
    // Validate signature
    if (memcmp(config->header.signature, BIOS_SIGNATURE, 4) != 0) {
        fclose(file);
        printf("Error: Invalid BIOS signature\n");
        return BIOS_ERROR_FORMAT;
    }
    
    // Read settings
    config->setting_count = config->header.num_settings;
    if (config->setting_count > MAX_SETTINGS) {
        fclose(file);
        printf("Error: Too many settings (%d > %d)\n", config->setting_count, MAX_SETTINGS);
        return BIOS_ERROR_FORMAT;
    }
    
    if (fread(config->settings, sizeof(bios_setting_t), config->setting_count, file) != config->setting_count) {
        fclose(file);
        printf("Error: Cannot read settings from %s\n", filename);
        return BIOS_ERROR_FORMAT;
    }
    
    fclose(file);
    
    // Validate checksum
    uint16_t calculated_checksum = calculate_checksum(config);
    if (calculated_checksum != config->header.checksum) {
        printf("Warning: Checksum mismatch (calculated: %04X, expected: %04X)\n", 
               calculated_checksum, config->header.checksum);
        // Continue anyway for demo purposes
    }
    
    printf("Successfully loaded configuration: %d settings\n", config->setting_count);
    return BIOS_SUCCESS;
}

// Save configuration file
int save_bios_config(const char *filename, const bios_config_t *config) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Error: Cannot create file %s\n", filename);
        return BIOS_ERROR_FILE;
    }
    
    // Update header
    bios_config_header_t header = config->header;
    memcpy(header.signature, BIOS_SIGNATURE, 4);
    header.version = BIOS_VERSION;
    header.num_settings = config->setting_count;
    header.data_size = sizeof(bios_setting_t) * config->setting_count;
    header.checksum = calculate_checksum(config);
    
    // Write header
    if (fwrite(&header, sizeof(bios_config_header_t), 1, file) != 1) {
        fclose(file);
        printf("Error: Cannot write header to %s\n", filename);
        return BIOS_ERROR_FILE;
    }
    
    // Write settings
    if (fwrite(config->settings, sizeof(bios_setting_t), config->setting_count, file) != config->setting_count) {
        fclose(file);
        printf("Error: Cannot write settings to %s\n", filename);
        return BIOS_ERROR_FILE;
    }
    
    fclose(file);
    printf("Configuration saved to %s\n", filename);
    return BIOS_SUCCESS;
}

// Create a new setting with specified parameters
static void create_setting(bios_setting_t *setting, const char *name, bios_category_t category,
                          bios_data_type_t type, uint32_t value, uint32_t min_val, uint32_t max_val) {
    memset(setting, 0, sizeof(bios_setting_t));
    strncpy(setting->name, name, MAX_SETTING_NAME - 1);
    setting->category = category;
    setting->type = type;
    setting->min_value = min_val;
    setting->max_value = max_val;
    
    switch (type) {
        case BIOS_TYPE_UINT8:
            setting->size = 1;
            *(uint8_t*)setting->data = (uint8_t)value;
            break;
        case BIOS_TYPE_UINT16:
            setting->size = 2;
            *(uint16_t*)setting->data = (uint16_t)value;
            break;
        case BIOS_TYPE_UINT32:
            setting->size = 4;
            *(uint32_t*)setting->data = value;
            break;
        case BIOS_TYPE_BOOL:
            setting->size = 1;
            *(uint8_t*)setting->data = value ? 1 : 0;
            break;
        default:
            setting->size = 1;
            break;
    }
}

// Initialize default system configuration
int load_default_config(bios_config_t *config) {
    memset(config, 0, sizeof(bios_config_t));
    
    // Initialize header
    memcpy(config->header.signature, BIOS_SIGNATURE, 4);
    config->header.version = BIOS_VERSION;
    
    // Create default settings
    int idx = 0;
    
    // CPU Settings
    create_setting(&config->settings[idx++], "CPU_FREQ", CATEGORY_CPU, BIOS_TYPE_UINT32, 3200, 800, 5000);
    create_setting(&config->settings[idx++], "CPU_CACHE", CATEGORY_CPU, BIOS_TYPE_BOOL, 1, 0, 1);
    create_setting(&config->settings[idx++], "VIRTUALIZATION", CATEGORY_CPU, BIOS_TYPE_BOOL, 1, 0, 1);
    
    // Memory Settings
    create_setting(&config->settings[idx++], "MEMORY_FREQ", CATEGORY_MEMORY, BIOS_TYPE_UINT32, 2400, 1333, 4000);
    create_setting(&config->settings[idx++], "MEMORY_TEST", CATEGORY_MEMORY, BIOS_TYPE_BOOL, 0, 0, 1);
    create_setting(&config->settings[idx++], "ECC_ENABLE", CATEGORY_MEMORY, BIOS_TYPE_BOOL, 0, 0, 1);
    
    // Boot Settings
    create_setting(&config->settings[idx++], "FAST_BOOT", CATEGORY_BOOT, BIOS_TYPE_BOOL, 1, 0, 1);
    create_setting(&config->settings[idx++], "SECURE_BOOT", CATEGORY_BOOT, BIOS_TYPE_BOOL, 1, 0, 1);
    create_setting(&config->settings[idx++], "BOOT_TIMEOUT", CATEGORY_BOOT, BIOS_TYPE_UINT8, 5, 0, 30);
    
    // I/O Settings
    create_setting(&config->settings[idx++], "USB_ENABLE", CATEGORY_IO, BIOS_TYPE_BOOL, 1, 0, 1);
    create_setting(&config->settings[idx++], "SATA_MODE", CATEGORY_IO, BIOS_TYPE_UINT8, 1, 0, 2); // 0=IDE, 1=AHCI, 2=RAID
    create_setting(&config->settings[idx++], "NETWORK_BOOT", CATEGORY_IO, BIOS_TYPE_BOOL, 0, 0, 1);
    
    config->setting_count = idx;
    config->header.num_settings = idx;
    config->header.data_size = sizeof(bios_setting_t) * idx;
    config->header.checksum = calculate_checksum(config);
    
    printf("Default configuration initialized: %d settings\n", config->setting_count);
    return BIOS_SUCCESS;
}

// Get setting by name
int get_setting_by_name(const bios_config_t *config, const char *name, bios_setting_t *setting) {
    for (int i = 0; i < config->setting_count; i++) {
        if (strcmp(config->settings[i].name, name) == 0) {
            *setting = config->settings[i];
            return BIOS_SUCCESS;
        }
    }
    return BIOS_ERROR_INVALID_SETTING;
}

// Set setting by name
int set_setting_by_name(bios_config_t *config, const char *name, const void *value) {
    for (int i = 0; i < config->setting_count; i++) {
        if (strcmp(config->settings[i].name, name) == 0) {
            memcpy(config->settings[i].data, value, config->settings[i].size);
            config->header.checksum = calculate_checksum(config);
            // Recalculate checksum
            return BIOS_SUCCESS;
        }
    }
    return BIOS_ERROR_INVALID_SETTING;
}

// Utility functions
const char* get_category_name(bios_category_t category) {
    switch (category) {
        case CATEGORY_CPU: return "CPU";
        case CATEGORY_MEMORY: return "Memory";
        case CATEGORY_BOOT: return "Boot";
        case CATEGORY_IO: return "I/O";
        case CATEGORY_POWER: return "Power";
        default: return "Unknown";
    }
}

const char* get_type_name(bios_data_type_t type) {
    switch (type) {
        case BIOS_TYPE_UINT8: return "uint8";
        case BIOS_TYPE_UINT16: return "uint16";
        case BIOS_TYPE_UINT32: return "uint32";
        case BIOS_TYPE_STRING: return "string";
        case BIOS_TYPE_BOOL: return "bool";
        default: return "unknown";
    }
}

// Display functions
void display_setting(const bios_setting_t *setting) {
    printf("%-20s [%s] ", setting->name, get_category_name(setting->category));
    
    switch (setting->type) {
        case BIOS_TYPE_UINT8:
            printf("%u", *(uint8_t*)setting->data);
            break;
        case BIOS_TYPE_UINT16:
            printf("%u", *(uint16_t*)setting->data);
            break;
        case BIOS_TYPE_UINT32:
            printf("%u", *(uint32_t*)setting->data);
            break;
        case BIOS_TYPE_BOOL:
            printf("%s", *(uint8_t*)setting->data ? "Enabled" : "Disabled");
            break;
        default:
            printf("(unknown type)");
            break;
    }
    
    if (setting->type != BIOS_TYPE_BOOL) {
        printf(" (Range: %u-%u)", setting->min_value, setting->max_value);
    }
    printf("\n");
}

void display_config_summary(const bios_config_t *config) {
    printf("\n=== System Configuration ===\n");
    printf("Version: %d\n", config->header.version);
    printf("Settings: %d\n", config->setting_count);
    printf("Checksum: %04X\n", config->header.checksum);
    printf("\n");
    
    for (int i = 0; i < config->setting_count; i++) {
        display_setting(&config->settings[i]);
    }
}

void display_category_settings(const bios_config_t *config, bios_category_t category) {
    printf("\n=== %s Settings ===\n", get_category_name(category));
    
    int found = 0;
    for (int i = 0; i < config->setting_count; i++) {
        if (config->settings[i].category == category) {
            printf("%d. ", ++found);
            display_setting(&config->settings[i]);
        }
    }
    
    if (found == 0) {
        printf("No settings found for this category.\n");
    }
}