# ==============================================================================
# GPIO Driver - PC Example Makefile
# Use: make (defaults to HTTP) or make PLATFORM=WINDOWS
# ==============================================================================

# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2

# Paths (Relative to these specific examples)
ROOT_DIR = ../..
LOGGING_DIR = $(ROOT_DIR)/../logging_driver

INCLUDES = -I$(ROOT_DIR) \
           -I$(ROOT_DIR)/config \
           -I$(LOGGING_DIR) \
           -I../common \
           -I$(ROOT_DIR)/../helper_utils

# Platform Selection
PLATFORM ?= HTTP

ifeq ($(PLATFORM), HTTP)
    CFLAGS += -DPLATFORM_HTTP
    SRC_IMPL = $(ROOT_DIR)/implementations/pc/gpioLib_http.c
    TARGET_NAME = example_http
else ifeq ($(PLATFORM), WINDOWS)
    CFLAGS += -DPLATFORM_WINDOWS
    SRC_IMPL = $(ROOT_DIR)/implementations/pc/gpioLib_windows.c
    TARGET_NAME = example_windows
endif

# Build Directory
BUILD_DIR = build

# Source Files
SRCS = $(BUILD_DIR)/gpio_config_gen.c \
       $(ROOT_DIR)/gpioLib.c \
       $(SRC_IMPL) \
       $(LOGGING_DIR)/logLib.c \
       $(LOGGING_DIR)/implementations/logPlatform_console.c \
       ../common/app_main.c \
       ../../implementations/pc/platform_adapter.c \
       $(ROOT_DIR)/helpers/gpio_helper.c

# Object Files
OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(SRCS)))

# Targets
TARGET = $(BUILD_DIR)/$(TARGET_NAME).exe

# OS detection for linking
ifeq ($(OS), Windows_NT)
    LDFLAGS = -lws2_32 -lwinhttp
else
    LDFLAGS = 
endif

.PHONY: all clean prepare

all: prepare $(TARGET)

prepare:
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)

# Generation Rule
$(BUILD_DIR)/gpio_config_gen.c: config.json
	@echo [CFG] Generating config from JSON...
	@python $(ROOT_DIR)/scripts/gen_config.py config.json $@

# Compilation Rules
# Note: We use VPATH or explicit rules because sources are scattered

$(BUILD_DIR)/gpio_config_gen.o: $(BUILD_DIR)/gpio_config_gen.c | prepare
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/gpioLib.o: $(ROOT_DIR)/gpioLib.c | prepare
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/gpioLib_http.o: $(ROOT_DIR)/implementations/pc/gpioLib_http.c | prepare
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/gpioLib_windows.o: $(ROOT_DIR)/implementations/pc/gpioLib_windows.c | prepare
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/logLib.o: $(LOGGING_DIR)/logLib.c | prepare
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/logPlatform_console.o: $(LOGGING_DIR)/implementations/logPlatform_console.c | prepare
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/app_main.o: ../common/app_main.c | prepare
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/platform_adapter.o: ../../implementations/pc/platform_adapter.c | prepare
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/gpio_helper.o: $(ROOT_DIR)/helpers/gpio_helper.c | prepare
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Linking
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)
	@echo.
	@echo ========================================
	@echo Build successful: $(TARGET)
	@echo Platform: $(PLATFORM)
	@echo ========================================

clean:
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
