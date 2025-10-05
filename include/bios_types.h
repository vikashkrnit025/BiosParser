#ifndef BIOS_TYPES_H
#define BIOS_TYPES_H

// Standard C library includes
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Windows specific includes
#ifdef _WIN32
#include <windows.h>
#endif

// System Configuration File Format
#define BIOS_SIGNATURE "SYSC"
#define BIOS_VERSION 1
#define MAX_SETTING_NAME 32
#define MAX_SETTING_DATA 16
#define MAX_SETTINGS 64

// Setting Data Types
typedef enum {
    BIOS_TYPE_UINT8 = 1,
    BIOS_TYPE_UINT16 = 2,
    BIOS_TYPE_UINT32 = 3,
    BIOS_TYPE_STRING = 4,
    BIOS_TYPE_BOOL = 5
} bios_data_type_t;

// Setting Categories
typedef enum {
    CATEGORY_CPU = 1,
    CATEGORY_MEMORY = 2,
    CATEGORY_BOOT = 3,
    CATEGORY_IO = 4,
    CATEGORY_POWER = 5
} bios_category_t;

// System Configuration File Header
typedef struct __attribute__((packed)) {
    char signature[4];          // "SYSC"
    uint16_t version;           // Format version
    uint16_t num_settings;      // Number of settings
    uint32_t data_size;         // Total data size
    uint16_t checksum;          // Simple checksum
    uint16_t reserved;          // Padding
} bios_config_header_t;

// Individual System Setting
typedef struct __attribute__((packed)) {
    char name[MAX_SETTING_NAME];     // Setting name
    uint8_t category;                // Setting category
    uint8_t type;                    // Data type
    uint8_t size;                    // Data size in bytes
    uint8_t flags;                   // Setting flags (read-only, etc.)
    uint8_t data[MAX_SETTING_DATA];  // Setting value
    uint32_t min_value;              // Minimum allowed value
    uint32_t max_value;              // Maximum allowed value
} bios_setting_t;

// System Configuration Container
typedef struct {
    bios_config_header_t header;
    bios_setting_t settings[MAX_SETTINGS];
    int setting_count;
} bios_config_t;

// Return codes
#define BIOS_SUCCESS 0
#define BIOS_ERROR_FILE -1
#define BIOS_ERROR_FORMAT -2
#define BIOS_ERROR_CHECKSUM -3
#define BIOS_ERROR_INVALID_SETTING -4

#endif // BIOS_TYPES_H