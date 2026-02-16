# HAL Embedded MCP - AVR MCU (reproduce gpio_driver/examples/avr + MCP)
cmake_minimum_required(VERSION 3.10)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)

# AVR toolchain must be on PATH (developer adds it; see mcu/avr/README.md)
find_program(AVR_GCC avr-gcc avr-gcc.exe)
find_program(AVR_OBJCOPY avr-objcopy avr-objcopy.exe)
if(NOT AVR_GCC)
    message(FATAL_ERROR
        "avr-gcc not found. Add the AVR toolchain to your PATH.\n"
        "  See mcu/avr/README.md for how to add Arduino IDE or MSYS2 tools to PATH.")
endif()
set(CMAKE_C_COMPILER "${AVR_GCC}")
if(AVR_OBJCOPY)
    set(CMAKE_OBJCOPY "${AVR_OBJCOPY}")
else()
    set(CMAKE_OBJCOPY "avr-objcopy")
endif()

project(hal_mcp_avr C)

set(MCU "atmega328p")
set(F_CPU "16000000UL")
set(BAUD "115200" CACHE STRING "Baud rate")
set(PORT "COM3" CACHE STRING "Serial port for flashing")

# Avrdude: on PATH or set AVRDUDE_EXE / AVRDUDE_CONF (see README)
set(AVRDUDE_EXE "$ENV{AVRDUDE_EXE}" CACHE STRING "Path to avrdude (optional)")
set(AVRDUDE_CONF "$ENV{AVRDUDE_CONF}" CACHE STRING "Path to avrdude.conf (optional)")
if(AVRDUDE_EXE)
    set(AVRDUDE "${AVRDUDE_EXE}")
else()
    find_program(AVRDUDE avrdude avrdude.exe)
    if(NOT AVRDUDE)
        set(AVRDUDE "avrdude")
        message(WARNING "avrdude not found. Add to PATH or set AVRDUDE_EXE. See mcu/avr/README.md")
    endif()
endif()
if(AVRDUDE_CONF)
    set(AVRDUDE_FLAGS "-C${AVRDUDE_CONF}")
else()
    set(AVRDUDE_FLAGS "")
endif()

set(CMAKE_C_FLAGS "-mmcu=${MCU} -DF_CPU=${F_CPU} -DPLATFORM_AVR -Os -Wall -Wextra -std=c99 -ffunction-sections -fdata-sections")
set(CMAKE_EXE_LINKER_FLAGS "-mmcu=${MCU} -Wl,--gc-sections")

# Paths: hal_embedded_mcp/mcu/avr -> ../../ = hal_embedded_mcp, ../../../ = repo root
set(MCP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(MCP_MCU "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(MCP_MCU_COMMON "${MCP_MCU}/common")
set(REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
set(GPIO_DRIVER "${REPO_ROOT}/gpio_driver")
set(LOGGING_DRIVER "${REPO_ROOT}/logging_driver")
set(HELPER_UTILS "${REPO_ROOT}/helper_utils")
set(MCP_CONFIG "${MCP_ROOT}/config/config.json")
set(MCP_SCRIPTS "${MCP_ROOT}/scripts")

include_directories(
    ${GPIO_DRIVER}
    ${GPIO_DRIVER}/config
    ${GPIO_DRIVER}/helpers
    ${GPIO_DRIVER}/implementations/avr
    ${LOGGING_DRIVER}
    ${LOGGING_DRIVER}/implementations
    ${HELPER_UTILS}
    ${MCP_MCU}
    ${MCP_MCU_COMMON}
)

# 1) GPIO config from same config.json (gen_config.py from gpio_driver)
set(GEN_GPIO_CONFIG "${CMAKE_BINARY_DIR}/gpio_config_gen.c")
add_custom_command(
    OUTPUT ${GEN_GPIO_CONFIG}
    COMMAND python "${GPIO_DRIVER}/scripts/gen_config.py" "${MCP_CONFIG}" "${GEN_GPIO_CONFIG}"
    DEPENDS "${MCP_CONFIG}" "${GPIO_DRIVER}/scripts/gen_config.py"
    COMMENT "Generating GPIO config from config.json..."
)

# 2) MCP pin list from config.json
set(GEN_MCP_PINS "${CMAKE_BINARY_DIR}/mcp_pins_gen.c")
add_custom_command(
    OUTPUT ${GEN_MCP_PINS}
    COMMAND python "${MCP_SCRIPTS}/gen_mcp_from_config.py" "${MCP_CONFIG}" --c-out "${GEN_MCP_PINS}"
    DEPENDS "${MCP_CONFIG}" "${MCP_SCRIPTS}/gen_mcp_from_config.py"
    COMMENT "Generating MCP pins from config.json..."
)

set(SOURCES
    ${MCP_MCU_COMMON}/app_main.c
    ${GPIO_DRIVER}/implementations/avr/platform_adapter.c
    ${GPIO_DRIVER}/gpioLib.c
    ${GEN_GPIO_CONFIG}
    ${GPIO_DRIVER}/helpers/gpio_helper.c
    ${GPIO_DRIVER}/implementations/avr/gpioPlatform_avr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../tool_handlers_gpio.c
    ${GEN_MCP_PINS}
    ${LOGGING_DRIVER}/logLib.c
    ${LOGGING_DRIVER}/implementations/logPlatform_console.c
)

add_executable(${PROJECT_NAME}.elf ${SOURCES})

add_custom_command(
    TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex -R .eeprom ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMENT "Generating HEX"
)

add_custom_target(flash
    COMMAND ${AVRDUDE} ${AVRDUDE_FLAGS} -v -p m328p -c arduino -P ${PORT} -b ${BAUD} -D -U flash:w:${PROJECT_NAME}.hex:i
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Flashing to ${PORT}..."
)
