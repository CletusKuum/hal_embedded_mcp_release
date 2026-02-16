//==============================================================================
// GPIO Configuration Implementation
//------------------------------------------------------------------------------
//! @file
//! @brief GPIO pin configurations - SINGLE SOURCE OF TRUTH
//------------------------------------------------------------------------------

#include "gpio_config.h"
#include <stddef.h>

// ============================================================================
// SINGLE SOURCE OF TRUTH - All implementations use this array
// ============================================================================
// To add a new pin, just add it to this array below
// ALL implementations (HTTP, Windows, STM32, etc.) automatically pick it up!
// ============================================================================

const sGpioPinConfig_t g_psGpioPinConfigs[] = {
    {
        .pcPinName = GPIO_CONFIG_LED1_NAME,
        .eDirection = GPIO_CONFIG_LED1_DIRECTION,
        .ePull = GPIO_CONFIG_LED1_PULL,
        #ifdef PLATFORM_STM32
        // STM32 hardware mapping for LED1
        // IMPORTANT: Adjust these values based on your STM32 board!
        // u32Stm32Port: 0=GPIOA, 1=GPIOB, 2=GPIOC, etc.
        // u16Stm32Pin: 0-15 (GPIO0-GPIO15)
        // u8Stm32Af: 0=GPIO mode, 1-15=Alternate function modes
        .u32Stm32Port = 0,  // GPIOA (adjust for your board)
        .u16Stm32Pin = 0,   // GPIO0 (adjust for your board)
        .u8Stm32Af = 0,      // GPIO mode (not alternate function)
        #endif
        #ifdef PLATFORM_AVR
        // AVR hardware mapping for LED1
        // Arduino Nano pin 13 = PORTB pin 5 (PB5)
        .u8AvrPort = 1,     // PORTB (0=PORTA, 1=PORTB, 2=PORTC, 3=PORTD)
        .u8AvrPin = 5,       // Pin 5 (PB5 = Arduino pin 13 = built-in LED)
        #endif
        #ifdef PLATFORM_ARDUINO
        // Arduino hardware mapping for LED1
        .u8ArduinoPin = 13,  // D13 (built-in LED on most Arduino boards)
        #endif
    },
    {
        .pcPinName = GPIO_CONFIG_BUTTON1_NAME,
        .eDirection = GPIO_CONFIG_BUTTON1_DIRECTION,
        .ePull = GPIO_CONFIG_BUTTON1_PULL,
        #ifdef PLATFORM_STM32
        // STM32 hardware mapping for BUTTON1
        // IMPORTANT: Adjust these values based on your STM32 board!
        .u32Stm32Port = 1,  // GPIOB (adjust for your board)
        .u16Stm32Pin = 5,   // GPIO5 (adjust for your board)
        .u8Stm32Af = 0,      // GPIO mode
        #endif
        #ifdef PLATFORM_AVR
        // AVR hardware mapping for BUTTON1 (future support)
        .u8AvrPort = 1,     // PORTB
        .u8AvrPin = 5,       // PB5
        #endif
        #ifdef PLATFORM_ARDUINO
        // Arduino hardware mapping for BUTTON1
        .u8ArduinoPin = 2,   // D2 (with internal pull-up)
        #endif
    },
    { .pcPinName = NULL, .eDirection = 0, .ePull = 0 } // NULL terminator - marks end of array
};

