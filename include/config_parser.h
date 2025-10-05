#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include "bios_types.h"

// Forward declarations
struct bios_config;
struct bios_setting;

// Core parsing functions
int parse_bios_config(const char *filename, bios_config_t *config);
int save_bios_config(const char *filename, const bios_config_t *config);

// Configuration management
int load_default_config(bios_config_t *config);
int validate_config(const bios_config_t *config);

// Setting management
int get_setting_by_name(const bios_config_t *config, const char *name, bios_setting_t *setting);
int set_setting_by_name(bios_config_t *config, const char *name, const void *value);

// Display functions
void display_config_summary(const bios_config_t *config);
void display_setting(const bios_setting_t *setting);
void display_category_settings(const bios_config_t *config, bios_category_t category);

// Utility functions
uint16_t calculate_checksum(const bios_config_t *config);
const char* get_category_name(bios_category_t category);
const char* get_type_name(bios_data_type_t type);

#endif // CONFIG_PARSER_H