// ==============================================================================
// Platform Adapter - PC (Windows/HTTP)
// ==============================================================================
// Connects the universal app_main.c to the PC-specific GPIO implementations
// ==============================================================================

#include "gpioLib.h"
#include <stdint.h>

// Platform-Specific Includes
#if defined(PLATFORM_WINDOWS)
#include "gpioLib_windows.h"
#include <windows.h>

#else // Default to HTTP
#ifdef _WIN32
#include <windows.h> // For Sleep
#else
#include <unistd.h> // For usleep
#endif
#include "gpioLib_http.h"
#endif

// ==============================================================================
// Contract Implementation
// ==============================================================================

const sGpioInterface_t *psGetPlatformGpioInterface(void) {
#if defined(PLATFORM_WINDOWS)
  return &sGpioInterfaceWindows;
#else
  return &sGpioInterfaceHTTP;
#endif
}

void vPlatformDelayMs(uint32_t u32Ms) {
#ifdef _WIN32
  Sleep(u32Ms);
#else
  usleep(u32Ms * 1000);
#endif
}

// ==============================================================================
// Helper / Digital Twin Bridge Implementation
// ==============================================================================

#include "helper_common.h"
#include <stdio.h>

void vHelperSend(const char *pcCmd, const char *pcPin, int iValue) {
  // On PC/HTTP platform, the low-level driver (gpioLib_http) handles
  // synchronization with the simulator directly via HTTP.
  // We can just log here for debugging purposes.
  // printf("[Bridge Mock] {\"t\":\"%s\",\"p\":\"%s\",\"v\":%d}\n", pcCmd,
  // pcPin, iValue);
}

void vHelperSendString(const char *pcCmd, const char *pcPin,
                       const char *pcValue) {
  // Mock implementation
}
