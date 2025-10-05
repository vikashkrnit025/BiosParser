CC=gcc
CFLAGS=-Iinclude -Wall -Wextra -g -D_WIN32
SRCDIR=src
INCDIR=include
CONFIGDIR=configs
BUILDDIR=build
SOURCES=$(wildcard $(SRCDIR)/*.c)
OBJECTS=$(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
TARGET=$(BUILDDIR)/system_config.exe
GENERATOR=$(BUILDDIR)/config_gen.exe

.PHONY: all clean run setup help generator

# Default target
all: setup $(TARGET) $(GENERATOR)

# Create necessary directories
setup:
	@if not exist $(BUILDDIR) mkdir $(BUILDDIR)
	@if not exist $(CONFIGDIR) mkdir $(CONFIGDIR)

# Build main executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^
	@echo.
	@echo ✓ System Configuration Manager built successfully!
	@echo   Run with: $(TARGET)

# Build configuration generator
$(GENERATOR): tools/config_generator.c $(filter-out $(BUILDDIR)/main.o,$(OBJECTS))
	$(CC) $(CFLAGS) -o $@ $^
	@echo ✓ Configuration generator built!

# Build object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@if not exist $(BUILDDIR) mkdir $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Generate sample configurations
generator: $(GENERATOR)
	@echo.
	@echo Generating sample configuration files...
	$(GENERATOR)

# Clean build artifacts
clean:
	@if exist $(BUILDDIR) rd /s /q $(BUILDDIR)
	@if exist $(CONFIGDIR)\*.bin del /q $(CONFIGDIR)\*.bin
	@echo Clean completed

# Run the program
run: $(TARGET)
	@echo Running System Configuration Manager...
	$(TARGET)

# Show help
help:
	@echo.
	@echo Available targets:
	@echo   all        - Build the configuration manager
	@echo   run        - Build and run the program
	@echo   generator  - Build and run configuration generator
	@echo   clean      - Remove all build artifacts
	@echo   help       - Show this help message
	@echo.