//==============================================================================
// Logging Library - Arduino Platform Implementation
//------------------------------------------------------------------------------
//! @file
//! @brief Arduino logging implementation using Serial.print()
//! 
//! This implementation wraps Arduino's Serial.print() for logging output.
//! Works on Arduino boards with Serial Monitor support.
//------------------------------------------------------------------------------

#include "logPlatform_arduino.h"

#ifdef PLATFORM_ARDUINO

#include <Arduino.h>
#include <string.h>

// ============================================================================
// Arduino Logging Implementation
// ============================================================================

/**
 * @brief Initialize Arduino Serial logging
 * 
 * Automatically initializes the logging system with Arduino Serial configuration.
 */
static void vArduinoLogInit(void)
{
    // Initialize Serial for output
    Serial.begin(9600);
    while (!Serial) {
        ; // Wait for serial port to connect (needed for some boards)
    }
    
    // Initialize logging system with Arduino configuration
    sLogConfig_t sLogConfig = {
        .eLevel = LOG_LEVEL_INFO,
        .eMode = LOG_MODE_CONSOLE,  // Serial is treated as console
        .pcBuildVersion = "1.0.0",
        .bThreadSafe = false
    };
    
    eLogInit(&sLogConfig);
}

/**
 * @brief Output log message to Arduino Serial
 * 
 * @param pcMessage Formatted log message (JSON or plain text)
 */
static void vArduinoLogOutput(const char *pcMessage)
{
    if (pcMessage == NULL)
        return;
    
    // Output to Serial Monitor
    Serial.println(pcMessage);
    Serial.flush(); // Ensure data is sent
}

/**
 * @brief Cleanup Arduino Serial logging
 */
static void vArduinoLogCleanup(void)
{
    // Serial doesn't need cleanup on Arduino
}

// ============================================================================
// Arduino Logging Interface Registration
// ============================================================================

const sLogInterface_t sLogInterfaceArduino = {
    .vHalLogInitFunc = vArduinoLogInit,
    .vHalLogOutputFunc = vArduinoLogOutput,
    .vHalLogCleanupFunc = vArduinoLogCleanup,
};

#endif // PLATFORM_ARDUINO
