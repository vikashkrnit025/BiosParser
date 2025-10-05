#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "bios_types.h"
#include "config_parser.h"

// Validation functions
int validate_setting(const bios_setting_t *setting);
int validate_setting_value(const bios_setting_t *setting, const void *value);
int validate_cpu_settings(const bios_config_t *config);
int validate_memory_settings(const bios_config_t *config);
int validate_boot_settings(const bios_config_t *config);

// Safety checks
int check_safe_cpu_frequency(uint32_t frequency);
int check_safe_memory_frequency(uint32_t frequency);
int check_valid_boot_order(const char *boot_order);

// Conflict detection
int detect_setting_conflicts(const bios_config_t *config);
int check_cpu_memory_compatibility(const bios_config_t *config);

// Error reporting
void report_validation_error(const char *setting_name, const char *error_message);
void report_validation_warning(const char *setting_name, const char *warning_message);

#endif // VALIDATOR_H