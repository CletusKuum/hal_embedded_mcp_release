//==============================================================================
// GPIO Library - Arduino Platform Implementation
//------------------------------------------------------------------------------
//! @file
//! @brief Arduino GPIO implementation using digitalWrite/digitalRead
//! 
//! This file contains all Arduino-specific code (pinMode, digitalWrite, etc.).
//! The application code remains completely platform-independent.
//------------------------------------------------------------------------------

#include "gpioPlatform_arduino.h"

#ifdef PLATFORM_ARDUINO

#include <Arduino.h>
#include <string.h>

// ============================================================================
// Arduino GPIO Helper Functions
// ============================================================================

/**
 * @brief Map pin name to Arduino pin number (from config)
 * @param pcPinName Pin name (e.g., "LED1", "BUTTON1")
 * @return uint8_t Arduino pin number, or 0xFF if not found
 */
static uint8_t u8ArduinoMapPinName(const char *pcPinName)
{
    if (pcPinName == NULL)
        return 0xFF;
    
    extern const sGpioPinConfig_t g_psGpioPinConfigs[];
    
    // Search config array for matching pin name
    for (int i = 0; g_psGpioPinConfigs[i].pcPinName != NULL; i++)
    {
        if (strcmp(pcPinName, g_psGpioPinConfigs[i].pcPinName) == 0)
        {
            // Found matching pin - return Arduino pin number from config
            return g_psGpioPinConfigs[i].u8ArduinoPin;
        }
    }
    
    return 0xFF;  // Pin not found in config
}

// ============================================================================
// Arduino GPIO Interface Implementation
// ============================================================================

/**
 * @brief Initialize Arduino GPIO (minimal setup)
 * 
 * Actual pin configurations come from gpio_config.c (SINGLE SOURCE OF TRUTH)
 * This function initializes the Arduino HAL framework only.
 * Specific pins are configured via eArduinoGpioConfigure() when app calls it.
 */
static void vArduinoGpioInit(void)
{
    // Arduino HAL initialization
    // Individual pins are configured on-demand via eArduinoGpioConfigure()
    // Configuration data comes from gpio_config.c
}

/**
 * @brief Configure pins from the global config array
 * 
 * Called to apply all configurations from g_psGpioPinConfigs[]
 * This allows the application to auto-configure all pins without
 * platform-specific knowledge.
 */
static void vArduinoGpioConfigureAll(void)
{
    extern const sGpioPinConfig_t g_psGpioPinConfigs[];
    
    // Iterate through all pin configurations
    for (int i = 0; g_psGpioPinConfigs[i].pcPinName != NULL; i++)
    {
        eArduinoGpioConfigure(&g_psGpioPinConfigs[i]);
    }
}

/**
 * @brief Configure a GPIO pin (direction and pull settings)
 * 
 * @param psConfig Pin configuration (direction, pull-up/down)
 * @return eRetType_t RET_TYPE_SUCCESS on success, RET_TYPE_FAIL if pin not found
 */
static eRetType_t eArduinoGpioConfigure(const sGpioConfig_t *psConfig)
{
    if (psConfig == NULL || psConfig->pcPinName == NULL)
        return RET_TYPE_FAIL;
    
    uint8_t uPin = u8ArduinoMapPinName(psConfig->pcPinName);
    if (uPin == 0xFF)
        return RET_TYPE_FAIL;
    
    // Configure pin direction and pull settings
    if (psConfig->eDirection == GPIO_DIR_OUTPUT)
    {
        pinMode(uPin, OUTPUT);
    }
    else  // GPIO_DIR_INPUT
    {
        if (psConfig->ePull == GPIO_PULL_UP)
        {
            pinMode(uPin, INPUT_PULLUP);
        }
        else
        {
            pinMode(uPin, INPUT);
        }
        // Note: GPIO_PULL_DOWN is not directly supported on standard Arduino
        // For pull-down, use an external resistor
    }
    
    return RET_TYPE_SUCCESS;
}

/**
 * @brief Read a GPIO pin state
 * 
 * @param pcPinName Pin name to read
 * @param pbValue Pointer to store state (true = HIGH, false = LOW)
 * @return eRetType_t RET_TYPE_SUCCESS on success
 */
static eRetType_t eArduinoGpioRead(const char *pcPinName, bool *pbValue)
{
    if (pcPinName == NULL || pbValue == NULL)
        return RET_TYPE_FAIL;
    
    uint8_t uPin = u8ArduinoMapPinName(pcPinName);
    if (uPin == 0xFF)
        return RET_TYPE_FAIL;
    
    // Read digital pin state: HIGH (1) or LOW (0)
    *pbValue = (digitalRead(uPin) == HIGH);
    
    return RET_TYPE_SUCCESS;
}

/**
 * @brief Write a GPIO pin state
 * 
 * @param pcPinName Pin name to write
 * @param bValue State to write (true = HIGH, false = LOW)
 * @return eRetType_t RET_TYPE_SUCCESS on success
 */
static eRetType_t eArduinoGpioWrite(const char *pcPinName, bool bValue)
{
    if (pcPinName == NULL)
        return RET_TYPE_FAIL;
    
    uint8_t uPin = u8ArduinoMapPinName(pcPinName);
    if (uPin == 0xFF)
        return RET_TYPE_FAIL;
    
    // Write digital pin state
    digitalWrite(uPin, bValue ? HIGH : LOW);
    
    return RET_TYPE_SUCCESS;
}

// ============================================================================
// Arduino GPIO Interface Registration
// ============================================================================

const sGpioInterface_t sGpioInterfaceArduino = {
    .vHalGpioInitFunc = vArduinoGpioInit,
    .eHalGpioConfigureFunc = eArduinoGpioConfigure,
    .eHalGpioReadFunc = eArduinoGpioRead,
    .eHalGpioWriteFunc = eArduinoGpioWrite,
};

#endif // PLATFORM_ARDUINO
