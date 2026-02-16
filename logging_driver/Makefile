#==============================================================================
# Logging Library Makefile
#==============================================================================

# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
INCLUDES = -I. -Iimplementations

# Build Directory
BUILD_DIR = build

# Detect OS
ifeq ($(OS),Windows_NT)
    # Windows-specific settings
    RM = del /Q
    MKDIR = if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
    EXE = .exe
    TARGET_OS = Windows
else
    # Linux/macOS settings
    RM = rm -rf
    MKDIR = mkdir -p $(BUILD_DIR)
    EXE =
    TARGET_OS = Linux
endif

# Targets
TARGET = example
APP_NAME = $(BUILD_DIR)/$(TARGET)$(EXE)

# Source Files
SRCS = logLib.c \
       implementations/logPlatform_console.c \
       implementations/logPlatform_file.c \
       implementations/logPlatform_http.c \
       examples/example_main.c

# Object Files
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)

# Default Rule
all: $(BUILD_DIR) $(APP_NAME)

# Create Build Directory
$(BUILD_DIR):
	@echo Creating build directory...
	@$(MKDIR)
	@if not exist $(BUILD_DIR)\implementations mkdir $(BUILD_DIR)\implementations
	@if not exist $(BUILD_DIR)\examples mkdir $(BUILD_DIR)\examples

# Compile Source Files
$(BUILD_DIR)/%.o: %.c
	@echo Compiling $<...
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Link the Application
$(APP_NAME): $(OBJS)
	@echo Linking $@...
	@$(CC) $(OBJS) -o $@ $(CFLAGS)
	@echo.
	@echo ========================================
	@echo Build successful! Target: $(TARGET_OS)
	@echo ========================================

# Clean Rule
clean:
	@echo Cleaning up...
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)

.PHONY: all clean
