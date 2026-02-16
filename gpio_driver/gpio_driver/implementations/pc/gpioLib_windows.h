//==============================================================================
// GPIO Library - Windows GPIO Implementation Header
//------------------------------------------------------------------------------
//! @file
//! @brief Windows GPIO implementation header (file-based for testing)
//------------------------------------------------------------------------------

#ifndef GPIO_LIB_WINDOWS_H
#define GPIO_LIB_WINDOWS_H

#ifdef __cplusplus
extern "C" {
#endif

// Includes ====================================================================
#include "gpioLib.h"

#ifdef _WIN32
// External Variables ==========================================================
extern const sGpioInterface_t sGpioInterfaceWindows;
#endif

#ifdef __cplusplus
}
#endif

#endif // GPIO_LIB_WINDOWS_H
