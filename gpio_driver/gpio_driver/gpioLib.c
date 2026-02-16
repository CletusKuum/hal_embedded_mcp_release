//==============================================================================
// GPIO Library - HAL Interface Implementation
//------------------------------------------------------------------------------
//! @file
//! @brief GPIO interface implementation for HAL registration pattern
//------------------------------------------------------------------------------

// Includes ====================================================================
#include "gpioLib.h"
#include <stddef.h>

// Static Variables ============================================================
static const sGpioInterface_t *g_psGpioInterface = NULL;

// Functions ===================================================================

void vHalRegisterGpioInterface(const sGpioInterface_t *psInterface)
{
    g_psGpioInterface = psInterface;
}

const sGpioInterface_t *psHalGetGpioInterface(void)
{
    return g_psGpioInterface;
}

