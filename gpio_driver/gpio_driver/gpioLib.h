//==============================================================================
// GPIO Library - HAL Interface
//------------------------------------------------------------------------------
//! @file
//! @brief Simple GPIO interface for HAL registration pattern
//------------------------------------------------------------------------------

#ifndef GPIO_LIB_H
#define GPIO_LIB_H

#include <stdint.h>
#include <stdbool.h>
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

// Type Definitions ============================================================

/**
 * @brief GPIO direction
 */
typedef enum {
    GPIO_DIR_INPUT = 0,
    GPIO_DIR_OUTPUT = 1
} eGpioDirection_t;

/**
 * @brief GPIO pull configuration
 */
typedef enum {
    GPIO_PULL_NONE = 0,
    GPIO_PULL_UP = 1,
    GPIO_PULL_DOWN = 2
} eGpioPull_t;

/**
 * @brief GPIO pin configuration structure
 */
typedef struct {
    const char *pcPinName;        // Pin identifier (e.g., "LED1", "BUTTON1")
    eGpioDirection_t eDirection;  // INPUT or OUTPUT
    eGpioPull_t ePull;            // Pull-up, pull-down, or none
} sGpioConfig_t;

/**
 * @brief GPIO interface structure
 * 
 * Simple interface that any GPIO driver must implement.
 * Register this interface in main() with your chosen implementation.
 */
typedef struct {
    /**
     * @brief Initialize the GPIO interface (if needed)
     */
    void (*vHalGpioInitFunc)(void);
    
    /**
     * @brief Configure a GPIO pin
     * @param psConfig Pin configuration structure
     * @return eRetType_t RET_TYPE_SUCCESS on success
     */
    eRetType_t (*eHalGpioConfigureFunc)(const sGpioConfig_t *psConfig);
    
    /**
     * @brief Read a GPIO pin state
     * @param pcPinName Pin name/identifier
     * @param pbValue Pointer to store read value (true = HIGH, false = LOW)
     * @return eRetType_t RET_TYPE_SUCCESS on success
     */
    eRetType_t (*eHalGpioReadFunc)(const char *pcPinName, bool *pbValue);
    
    /**
     * @brief Write a GPIO pin state
     * @param pcPinName Pin name/identifier
     * @param bValue Value to write (true = HIGH, false = LOW)
     * @return eRetType_t RET_TYPE_SUCCESS on success
     */
    eRetType_t (*eHalGpioWriteFunc)(const char *pcPinName, bool bValue);
} sGpioInterface_t;

// Function Prototypes =========================================================

/**
 * @brief Register GPIO interface
 * @param psInterface GPIO interface structure
 */
void vHalRegisterGpioInterface(const sGpioInterface_t *psInterface);

/**
 * @brief Get registered GPIO interface
 * @return const sGpioInterface_t* Registered interface, or NULL if not registered
 */
const sGpioInterface_t *psHalGetGpioInterface(void);

#ifdef __cplusplus
}
#endif

#endif // GPIO_LIB_H

