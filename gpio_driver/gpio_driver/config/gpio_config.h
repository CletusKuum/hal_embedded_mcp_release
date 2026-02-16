//==============================================================================
// GPIO Configuration
//------------------------------------------------------------------------------
//! @file
//! @brief GPIO pin configurations for all implementations
//------------------------------------------------------------------------------

#ifndef GPIO_CONFIG_H
#define GPIO_CONFIG_H

#include "../gpioLib.h"

#ifdef __cplusplus
extern "C" {
#endif

// GPIO Pin Configurations ====================================================

/**
 * @brief Default GPIO pin configurations
 * 
 * Each implementation (HTTP, Windows, STM32, etc.) can use these configurations
 * during initialization. Implementations read from this config during Init().
 */

// LED1 configuration
#define GPIO_CONFIG_LED1_NAME      "LED1"
#define GPIO_CONFIG_LED1_DIRECTION GPIO_DIR_OUTPUT
#define GPIO_CONFIG_LED1_PULL      GPIO_PULL_NONE

// BUTTON1 configuration
#define GPIO_CONFIG_BUTTON1_NAME      "BUTTON1"
#define GPIO_CONFIG_BUTTON1_DIRECTION GPIO_DIR_INPUT
#define GPIO_CONFIG_BUTTON1_PULL      GPIO_PULL_UP

// Configuration array structure for easy iteration
// NULL-terminated array for iteration in implementations
// Platform-specific fields are conditionally compiled based on platform
typedef struct {
    const char *pcPinName;        // Generic pin name: "LED1", "BUTTON1", etc.
    eGpioDirection_t eDirection;  // Direction: INPUT or OUTPUT
    eGpioPull_t ePull;            // Pull configuration: NONE, UP, or DOWN
    
    // ============================================================================
    // Platform-Specific Hardware Mappings (optional, based on compile-time define)
    // ============================================================================
    // These fields are only included when compiling for the specific platform.
    // Other platforms ignore these fields (they're zero/unused for them).
    // To enable for a platform, define PLATFORM_STM32, PLATFORM_AVR, PLATFORM_ARDUINO, etc.
    // ============================================================================
    
    #ifdef PLATFORM_STM32
    // STM32 hardware mapping (requires libopencm3)
    // These map the generic pin name to actual STM32 hardware port and pin
    uint32_t u32Stm32Port;        // GPIO port: GPIOA, GPIOB, GPIOC, etc. (libopencm3 enum value)
    uint16_t u16Stm32Pin;         // GPIO pin: GPIO0, GPIO1, ..., GPIO15 (libopencm3 define)
    uint8_t u8Stm32Af;            // Alternate function: 0=GPIO, 1-15=AF modes (for UART, I2C, etc.)
    #endif
    
    #ifdef PLATFORM_AVR
    // AVR hardware mapping (future support)
    uint8_t u8AvrPort;            // PORT register: PORTA, PORTB, PORTC, etc.
    uint8_t u8AvrPin;             // Pin number: 0-7
    #endif
    
    #ifdef PLATFORM_ARDUINO
    // Arduino hardware mapping
    // Maps generic pin name to Arduino digital pin number (0-53 for Mega, 0-19 for Uno/Nano)
    uint8_t u8ArduinoPin;         // Arduino digital pin: 0-19 (Uno/Nano), 0-53 (Mega)
    #endif
} sGpioPinConfig_t;

// ============================================================================
// SINGLE SOURCE OF TRUTH - All implementations use this array
// ============================================================================
// Array is defined in gpio_config.c - ONE source, used by ALL implementations
// To add a new pin (e.g., LED2), edit gpio_config.c only!
// ALL implementations (HTTP, Windows, STM32) automatically pick it up!
// ============================================================================

// Array is defined in gpio_config.c
extern const sGpioPinConfig_t g_psGpioPinConfigs[];

// Helper macro to get number of configured pins (excluding NULL terminator)
#define GPIO_CONFIG_PIN_COUNT 2

#ifdef __cplusplus
}
#endif

#endif // GPIO_CONFIG_H

