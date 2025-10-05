#include "../include/validator.h"

// Core validation functions
int validate_setting(const bios_setting_t *setting) {
    if (!setting) {
        return BIOS_ERROR_INVALID_SETTING;
    }
    
    // Check name is not empty
    if (strlen(setting->name) == 0) {
        report_validation_error(setting->name, "Setting name is empty");
        return BIOS_ERROR_INVALID_SETTING;
    }
    
    // Check data type is valid
    if (setting->type < BIOS_TYPE_UINT8 || setting->type > BIOS_TYPE_BOOL) {
        report_validation_error(setting->name, "Invalid data type");
        return BIOS_ERROR_INVALID_SETTING;
    }
    
    // Validate value ranges based on type
    switch (setting->type) {
        case BIOS_TYPE_UINT8: {
            uint8_t value = *(uint8_t*)setting->data;
            if (value < setting->min_value || value > setting->max_value) {
                report_validation_error(setting->name, "Value out of range");
                return BIOS_ERROR_INVALID_SETTING;
            }
            break;
        }
        case BIOS_TYPE_UINT16: {
            uint16_t value = *(uint16_t*)setting->data;
            if (value < setting->min_value || value > setting->max_value) {
                report_validation_error(setting->name, "Value out of range");
                return BIOS_ERROR_INVALID_SETTING;
            }
            break;
        }
        case BIOS_TYPE_UINT32: {
            uint32_t value = *(uint32_t*)setting->data;
            if (value < setting->min_value || value > setting->max_value) {
                report_validation_error(setting->name, "Value out of range");
                return BIOS_ERROR_INVALID_SETTING;
            }
            break;
        }
        case BIOS_TYPE_BOOL: {
            uint8_t value = *(uint8_t*)setting->data;
            if (value != 0 && value != 1) {
                report_validation_error(setting->name, "Boolean value must be 0 or 1");
                return BIOS_ERROR_INVALID_SETTING;
            }
            break;
        }
    }
    
    return BIOS_SUCCESS;
}

int validate_setting_value(const bios_setting_t *setting, const void *value) {
    if (!setting || !value) {
        return BIOS_ERROR_INVALID_SETTING;
    }
    
    switch (setting->type) {
        case BIOS_TYPE_UINT8: {
            uint8_t val = *(uint8_t*)value;
            return (val >= setting->min_value && val <= setting->max_value) ? BIOS_SUCCESS : BIOS_ERROR_INVALID_SETTING;
        }
        case BIOS_TYPE_UINT16: {
            uint16_t val = *(uint16_t*)value;
            return (val >= setting->min_value && val <= setting->max_value) ? BIOS_SUCCESS : BIOS_ERROR_INVALID_SETTING;
        }
        case BIOS_TYPE_UINT32: {
            uint32_t val = *(uint32_t*)value;
            return (val >= setting->min_value && val <= setting->max_value) ? BIOS_SUCCESS : BIOS_ERROR_INVALID_SETTING;
        }
        case BIOS_TYPE_BOOL: {
            uint8_t val = *(uint8_t*)value;
            return (val == 0 || val == 1) ? BIOS_SUCCESS : BIOS_ERROR_INVALID_SETTING;
        }
        default:
            return BIOS_ERROR_INVALID_SETTING;
    }
}

// Processor frequency safety check
int check_safe_cpu_frequency(uint32_t frequency) {
    // Basic safety checks for processor frequency
    if (frequency < 800) {
        return BIOS_ERROR_INVALID_SETTING; // Too low, system may not boot
    }
    if (frequency > 5000) {
        return BIOS_ERROR_INVALID_SETTING; // Too high, may damage hardware
    }
    if (frequency > 4000) {
        return 1; // Warning: High frequency
    }
    return BIOS_SUCCESS;
}

int validate_cpu_settings(const bios_config_t *config) {
    bios_setting_t setting;
    
    // Check CPU frequency
    if (get_setting_by_name(config, "CPU_FREQ", &setting) == BIOS_SUCCESS) {
        uint32_t freq = *(uint32_t*)setting.data;
        int result = check_safe_cpu_frequency(freq);
        if (result == BIOS_ERROR_INVALID_SETTING) {
            report_validation_error("CPU_FREQ", "Unsafe processor frequency");
            return BIOS_ERROR_INVALID_SETTING;
        } else if (result == 1) {
            report_validation_warning("CPU_FREQ", "High processor frequency - ensure adequate cooling");
        }
    }
    
    return BIOS_SUCCESS;
}

// Memory frequency safety check
int check_safe_memory_frequency(uint32_t frequency) {
    if (frequency < 1333) {
        return BIOS_ERROR_INVALID_SETTING; // Too low for modern systems
    }
    if (frequency > 4000) {
        return BIOS_ERROR_INVALID_SETTING; // Too high for most memory
    }
    return BIOS_SUCCESS;
}

int validate_memory_settings(const bios_config_t *config) {
    bios_setting_t setting;
    
    // Check memory frequency
    if (get_setting_by_name(config, "MEMORY_FREQ", &setting) == BIOS_SUCCESS) {
        uint32_t freq = *(uint32_t*)setting.data;
        if (check_safe_memory_frequency(freq) != BIOS_SUCCESS) {
            report_validation_error("MEMORY_FREQ", "Invalid memory frequency");
            return BIOS_ERROR_INVALID_SETTING;
        }
    }
    
    return BIOS_SUCCESS;
}

// Boot configuration validation
int check_valid_boot_order(const char *boot_order) {
    // Simple validation - just check it's not empty
    return (boot_order && strlen(boot_order) > 0) ? BIOS_SUCCESS : BIOS_ERROR_INVALID_SETTING;
}

int validate_boot_settings(const bios_config_t *config) {
    bios_setting_t setting;
    
    // Check boot timeout
    if (get_setting_by_name(config, "BOOT_TIMEOUT", &setting) == BIOS_SUCCESS) {
        uint8_t timeout = *(uint8_t*)setting.data;
        if (timeout > 30) {
            report_validation_warning("BOOT_TIMEOUT", "Long boot timeout may delay system startup");
        }
    }
    
    return BIOS_SUCCESS;
}

// System configuration validation
int detect_setting_conflicts(const bios_config_t *config) {
    // Check processor and memory compatibility
    return check_cpu_memory_compatibility(config);
}

int check_cpu_memory_compatibility(const bios_config_t *config) {
    bios_setting_t cpu_setting, mem_setting;
    
    if (get_setting_by_name(config, "CPU_FREQ", &cpu_setting) == BIOS_SUCCESS &&
        get_setting_by_name(config, "MEMORY_FREQ", &mem_setting) == BIOS_SUCCESS) {
        
        uint32_t cpu_freq = *(uint32_t*)cpu_setting.data;
        uint32_t mem_freq = *(uint32_t*)mem_setting.data;
        
        // Compatibility check for optimal performance
        if (cpu_freq > 4000 && mem_freq < 2400) {
            report_validation_warning("CPU_FREQ/MEMORY_FREQ", 
                "High processor frequency with low memory frequency may reduce performance");
        }
    }
    
    return BIOS_SUCCESS;
}

// Error reporting functions
void report_validation_error(const char *setting_name, const char *error_message) {
    printf("ERROR [%s]: %s\n", setting_name ? setting_name : "Unknown", error_message);
}

void report_validation_warning(const char *setting_name, const char *warning_message) {
    printf("WARNING [%s]: %s\n", setting_name ? setting_name : "Unknown", warning_message);
}