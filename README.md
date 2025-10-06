<<<<<<< HEAD
# System Configuration Manager

A comprehensive system configuration parser and management utility for Windows systems. This application provides a robust interface for managing system settings through binary configuration files.

## Overview

This project implements a complete configuration management system that:

✓ **Parses binary configuration files** - Structured format with integrity verification
✓ **Validates system settings** - Prevents dangerous configurations
✓ **Provides setup interface** - Interactive configuration management
✓ **Manages multiple profiles** - Save/load different configurations
✓ **Detects conflicts** - Identifies incompatible settings

## Project Structure

```
BiosParser/
├── include/
│   ├── bios_types.h       # Core data structures and definitions
│   ├── config_parser.h    # Configuration file handling
│   ├── setup_menu.h       # User interface components
│   └── validator.h        # Configuration validation
├── src/
│   ├── main.c            # Application entry point
│   ├── config_parser.c   # File I/O and parsing logic
│   ├── setup_menu.c      # Interactive menu system
│   └── validator.c       # Validation and safety checks
├── tools/
│   └── config_generator.c # Configuration file generator
└── configs/
    └── *.bin             # Binary configuration files
```

## Building and Installation

### Prerequisites
- GCC compiler
- Windows 10/11
- Make utility

### Quick Build
```cmd
make all
```

### Running the Application
```cmd
make run
```

## Features

### Binary File Format
- Custom structured binary format
- Header with signature verification
- Checksum for data integrity
- Version compatibility checking

### Configuration Categories
- **CPU**: Processor frequency, cache settings, virtualization
- **Memory**: RAM frequency, testing options, ECC settings
- **Boot**: Startup configuration and timeouts
- **I/O**: USB, SATA, and network settings

### Safety Features
- Range validation for all numeric values
- Compatibility checking between related settings
- Warning system for potentially unsafe configurations
- Automatic conflict detection

## Usage

### Interactive Mode
Run the application and navigate through the menu system:

1. **Setup Utility** - Modify system settings interactively
2. **View Configuration** - Display current settings
3. **System Information** - Show technical details

### Configuration Management
- Save custom configurations
- Load preset configurations
- Reset to factory defaults
- Export/import settings

## Technical Implementation

### Data Structures
```c
typedef struct {
    char signature[4];      // Format identifier
    uint16_t version;       // Version number
    uint16_t num_settings;  // Setting count
    uint32_t data_size;     // Data size
    uint16_t checksum;      // Integrity check
} bios_config_header_t;
```

### Setting Management
```c
typedef struct {
    char name[32];          // Setting identifier
    uint8_t category;       // Setting category
    uint8_t type;           // Data type
    uint8_t data[16];       // Setting value
    uint32_t min_value;     // Minimum allowed
    uint32_t max_value;     // Maximum allowed
} bios_setting_t;
```

### Validation System
- Type-safe value checking
- Range boundary enforcement
- Cross-setting compatibility
- Hardware safety limits

## Advanced Features

### Configuration Generator
Generate preset configurations:
```cmd
make generator
```

Creates:
- `default.bin` - Standard settings
- `performance.bin` - High-performance configuration
- `stability.bin` - Conservative, stable settings

### Error Handling
- Comprehensive file I/O error handling
- Graceful recovery from corrupted files
- User-friendly error messages
- Automatic fallback to defaults

## Code Architecture

### Modular Design
- **Parser Module**: File I/O and data parsing
- **Validator Module**: Safety and compatibility checking
- **UI Module**: Interactive menu system
- **Core Types**: Shared data structures

### Memory Management
- Static allocation for predictable behavior
- Bounds checking on all array access
- No dynamic memory allocation

### Windows Optimization
- Windows-specific file paths (backslashes)
- Console clearing using `cls` command
- Native Windows console features

## Testing

The application includes built-in validation testing:
- Setting range verification
- File format validation
- Checksum integrity testing
- Configuration conflict detection

## Development

### Adding New Settings
1. Define setting in `load_default_config()`
2. Add validation rules in `validator.c`
3. Update category displays as needed

### Extending Categories
1. Add new category to `bios_category_t` enum
2. Update `get_category_name()` function
3. Implement category-specific validation

## License

This project is developed as a system configuration management tool for Windows environments.
=======
# BiosParser
>>>>>>> 2709c664232a16d4c0947651e1d697981e29e075
