# HAL Embedded MCP - PC host (same common app_main; no UART callback, MCP via other transport later)
cmake_minimum_required(VERSION 3.15)
project(hal_mcp_pc LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(MCP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(MCP_MCU "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(MCP_MCU_COMMON "${MCP_MCU}/common")
set(REPO_ROOT "${MCP_ROOT}/../..")
set(GPIO_DRIVER "${REPO_ROOT}/gpio_driver")

if(NOT DEFINED PLATFORM)
    set(PLATFORM "HTTP")
endif()

if(PLATFORM STREQUAL "HTTP")
    add_compile_definitions(PLATFORM_HTTP)
    set(IMPL_SRC ${GPIO_DRIVER}/implementations/pc/gpioLib_http.c)
elseif(PLATFORM STREQUAL "WINDOWS")
    add_compile_definitions(PLATFORM_WINDOWS)
    set(IMPL_SRC ${GPIO_DRIVER}/implementations/pc/gpioLib_windows.c)
else()
    set(IMPL_SRC ${GPIO_DRIVER}/implementations/pc/gpioLib_http.c)
    add_compile_definitions(PLATFORM_HTTP)
endif()
set(LOGGING_DRIVER "${REPO_ROOT}/logging_driver")
set(HELPER_UTILS "${REPO_ROOT}/helper_utils")
set(MCP_CONFIG "${MCP_ROOT}/config/config.json")
set(MCP_SCRIPTS "${MCP_ROOT}/scripts")

# GPIO config from MCP config.json
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
    ${GPIO_DRIVER}/implementations/pc
    ${LOGGING_DRIVER}
    ${LOGGING_DRIVER}/implementations
    ${HELPER_UTILS}
    ${MCP_MCU}
    ${MCP_MCU_COMMON}
)

set(SOURCES
    ${MCP_MCU_COMMON}/app_main.c
    ${GPIO_DRIVER}/implementations/pc/platform_adapter.c
    ${GPIO_DRIVER}/gpioLib.c
    ${GEN_GPIO_CONFIG}
    ${GPIO_DRIVER}/helpers/gpio_helper.c
    ${IMPL_SRC}
    ${MCP_MCU}/tool_handlers_gpio.c
    ${GEN_MCP_PINS}
    ${LOGGING_DRIVER}/logLib.c
    ${LOGGING_DRIVER}/implementations/logPlatform_console.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

if(WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32 winhttp)
endif()
