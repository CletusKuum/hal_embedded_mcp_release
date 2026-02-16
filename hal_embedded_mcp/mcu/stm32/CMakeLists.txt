# HAL Embedded MCP - STM32 MCU (same common app_main; UART callback TBD for this platform)
# Requires arm-none-eabi-gcc and STM32 HAL/SDK; customize MCU/board as needed.
cmake_minimum_required(VERSION 3.10)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

project(hal_mcp_stm32 C)

set(MCP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(MCP_MCU "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(MCP_MCU_COMMON "${MCP_MCU}/common")
set(REPO_ROOT "${MCP_ROOT}/../..")
set(GPIO_DRIVER "${REPO_ROOT}/gpio_driver")
set(LOGGING_DRIVER "${REPO_ROOT}/logging_driver")
set(HELPER_UTILS "${REPO_ROOT}/helper_utils")
set(MCP_CONFIG "${MCP_ROOT}/config/config.json")
set(MCP_SCRIPTS "${MCP_ROOT}/scripts")

# Toolchain: set when cross-compiling (e.g. -DCMAKE_C_COMPILER=arm-none-eabi-gcc)
# set(CMAKE_C_COMPILER arm-none-eabi-gcc)
add_compile_definitions(PLATFORM_STM32)

set(GEN_GPIO_CONFIG "${CMAKE_BINARY_DIR}/gpio_config_gen.c")
add_custom_command(
    OUTPUT ${GEN_GPIO_CONFIG}
    COMMAND python "${GPIO_DRIVER}/scripts/gen_config.py" "${MCP_CONFIG}" "${GEN_GPIO_CONFIG}"
    DEPENDS "${MCP_CONFIG}" "${GPIO_DRIVER}/scripts/gen_config.py"
    COMMENT "Generating GPIO config from config.json..."
)

set(GEN_MCP_PINS "${CMAKE_BINARY_DIR}/mcp_pins_gen.c")
add_custom_command(
    OUTPUT ${GEN_MCP_PINS}
    COMMAND python "${MCP_SCRIPTS}/gen_mcp_from_config.py" "${MCP_CONFIG}" --c-out "${GEN_MCP_PINS}"
    DEPENDS "${MCP_CONFIG}" "${MCP_SCRIPTS}/gen_mcp_from_config.py"
    COMMENT "Generating MCP pins from config.json..."
)

include_directories(
    ${GPIO_DRIVER}
    ${GPIO_DRIVER}/config
    ${GPIO_DRIVER}/helpers
    ${GPIO_DRIVER}/implementations/stm32
    ${LOGGING_DRIVER}
    ${LOGGING_DRIVER}/implementations
    ${HELPER_UTILS}
    ${MCP_MCU}
    ${MCP_MCU_COMMON}
)

set(SOURCES
    ${MCP_MCU_COMMON}/app_main.c
    ${GPIO_DRIVER}/implementations/stm32/platform_adapter.c
    ${GPIO_DRIVER}/gpioLib.c
    ${GEN_GPIO_CONFIG}
    ${GPIO_DRIVER}/helpers/gpio_helper.c
    ${GPIO_DRIVER}/implementations/stm32/gpioPlatform_stm32.c
    ${MCP_MCU}/tool_handlers_gpio.c
    ${GEN_MCP_PINS}
    ${LOGGING_DRIVER}/logLib.c
    ${LOGGING_DRIVER}/implementations/logPlatform_console.c
)

add_executable(${PROJECT_NAME}.elf ${SOURCES})

# Add your STM32 HAL/SDK includes and link script here; e.g. target_include_directories, target_link_options
