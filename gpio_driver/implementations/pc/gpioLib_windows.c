//==============================================================================
// GPIO Library - Windows GPIO Implementation
//------------------------------------------------------------------------------
//! @file
//! @brief Windows-specific GPIO implementation (file-based for
//! testing/simulation)
//------------------------------------------------------------------------------

#ifdef _WIN32

// Includes ====================================================================
#include "../../config/gpio_config.h" // Configuration for all pins - SINGLE SOURCE OF TRUTH
#include "../../gpioLib.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#ifdef _WIN32
#include <winbase.h>
#endif

// Constants ===================================================================
#define MAX_PINS 32
#define PIN_STATE_DIR "gpio_states"
#define MAX_PIN_NAME_LEN 32

// Type Definitions ============================================================
typedef struct {
  char acPinName[MAX_PIN_NAME_LEN];
  bool bConfigured;
  eGpioDirection_t eDirection;
  eGpioPull_t ePull;
  bool bValue;
} sPinState_t;

// Static Variables ============================================================
static sPinState_t g_psPins[MAX_PINS] = {0};
static uint8_t g_u8PinCount = 0;
static bool g_bInitialized = false;

// Private Functions ===========================================================

static void vCreatePinStateDir(void) { CreateDirectoryA(PIN_STATE_DIR, NULL); }

static sPinState_t *psFindPin(const char *pcPinName) {
  for (uint8_t i = 0; i < g_u8PinCount; i++) {
    if (strcmp(g_psPins[i].acPinName, pcPinName) == 0) {
      return &g_psPins[i];
    }
  }
  return NULL;
}

static void vWritePinStateToFile(const char *pcPinName, bool bValue) {
  char acFilePath[MAX_PATH];
  snprintf(acFilePath, sizeof(acFilePath), "%s\\%s.txt", PIN_STATE_DIR,
           pcPinName);

  FILE *pfFile = fopen(acFilePath, "w");
  if (pfFile != NULL) {
    fprintf(pfFile, "%d", bValue ? 1 : 0);
    fclose(pfFile);
  }
}

static bool bReadPinStateFromFile(const char *pcPinName) {
  char acFilePath[MAX_PATH];
  snprintf(acFilePath, sizeof(acFilePath), "%s\\%s.txt", PIN_STATE_DIR,
           pcPinName);

  FILE *pfFile = fopen(acFilePath, "r");
  if (pfFile != NULL) {
    int iValue;
    if (fscanf(pfFile, "%d", &iValue) == 1) {
      fclose(pfFile);
      return (iValue != 0);
    }
    fclose(pfFile);
  }
  return false;
}

// Forward Declarations =======================================================
eRetType_t eGpioWindowsConfigure(const sGpioConfig_t *psConfig);

// Functions ===================================================================

/**
 * @brief Initialize the Windows GPIO interface
 * Configures default pins for this implementation (LED1, BUTTON1)
 */
void vGpioWindowsInit(void) {
  if (g_bInitialized) {
    return;
  }

  vCreatePinStateDir();
  g_bInitialized = true;
  printf("[GPIO Windows] Initializing Windows GPIO implementation (file-based "
         "simulation)...\n");
  printf("[GPIO Windows] Pin states stored in: %s\\ directory\n",
         PIN_STATE_DIR);

  // Configure pins from config file - SINGLE SOURCE OF TRUTH
  // All configurations are in config/gpio_config.c
  // Both HTTP and Windows implementations use the SAME config file!
  printf("[GPIO Windows] Configuring pins from config...\n");

  const sGpioPinConfig_t *psPinConfig = g_psGpioPinConfigs;
  uint8_t u8ConfiguredCount = 0;

  while (psPinConfig->pcPinName != NULL) {
    sGpioConfig_t sConfig = {.pcPinName = psPinConfig->pcPinName,
                             .eDirection = psPinConfig->eDirection,
                             .ePull = psPinConfig->ePull};

    eRetType_t eRet = eGpioWindowsConfigure(&sConfig);
    if (eRet == RET_TYPE_SUCCESS) {
      printf("[GPIO Windows] [OK] %s configured (%s, %s)\n",
             psPinConfig->pcPinName,
             (psPinConfig->eDirection == GPIO_DIR_OUTPUT) ? "OUTPUT" : "INPUT",
             (psPinConfig->ePull == GPIO_PULL_UP)     ? "PULL_UP"
             : (psPinConfig->ePull == GPIO_PULL_DOWN) ? "PULL_DOWN"
                                                      : "NONE");
      u8ConfiguredCount++;
    } else {
      printf("[GPIO Windows] [WARNING] Failed to configure %s\n",
             psPinConfig->pcPinName);
    }

    psPinConfig++;
  }

  printf("[GPIO Windows] Initialization complete. Configured %u pins from "
         "config.\n",
         u8ConfiguredCount);
}

/**
 * @brief Configure a GPIO pin
 */
eRetType_t eGpioWindowsConfigure(const sGpioConfig_t *psConfig) {
  if (psConfig == NULL || psConfig->pcPinName == NULL) {
    return RET_TYPE_NULL_POINTER;
  }

  if (!g_bInitialized) {
    return RET_TYPE_NOT_INITIALIZED;
  }

  sPinState_t *psPin = psFindPin(psConfig->pcPinName);

  if (psPin == NULL) {
    if (g_u8PinCount >= MAX_PINS) {
      return RET_TYPE_FAIL;
    }

    psPin = &g_psPins[g_u8PinCount];
    strncpy(psPin->acPinName, psConfig->pcPinName, MAX_PIN_NAME_LEN - 1);
    psPin->acPinName[MAX_PIN_NAME_LEN - 1] = '\0';
    g_u8PinCount++;
  }

  psPin->bConfigured = true;
  psPin->eDirection = psConfig->eDirection;
  psPin->ePull = psConfig->ePull;

  // Initialize output pins to LOW
  if (psConfig->eDirection == GPIO_DIR_OUTPUT) {
    psPin->bValue = false;
    vWritePinStateToFile(psConfig->pcPinName, false);
  }

  printf("[GPIO] Pin '%s' configured as %s with pull %d\n", psConfig->pcPinName,
         psConfig->eDirection == GPIO_DIR_INPUT ? "INPUT" : "OUTPUT",
         psConfig->ePull);

  return RET_TYPE_SUCCESS;
}

/**
 * @brief Read a GPIO pin state
 */
eRetType_t eGpioWindowsRead(const char *pcPinName, bool *pbValue) {
  if (pcPinName == NULL || pbValue == NULL) {
    return RET_TYPE_NULL_POINTER;
  }

  if (!g_bInitialized) {
    return RET_TYPE_NOT_INITIALIZED;
  }

  sPinState_t *psPin = psFindPin(pcPinName);
  if (psPin == NULL || !psPin->bConfigured) {
    return RET_TYPE_NOT_FOUND;
  }

  if (psPin->eDirection != GPIO_DIR_INPUT) {
    return RET_TYPE_INVALID_STATE;
  }

  // Read from file (for simulation) or use cached value
  psPin->bValue = bReadPinStateFromFile(pcPinName);
  *pbValue = psPin->bValue;

  return RET_TYPE_SUCCESS;
}

/**
 * @brief Write a GPIO pin state
 */
eRetType_t eGpioWindowsWrite(const char *pcPinName, bool bValue) {
  if (pcPinName == NULL) {
    return RET_TYPE_NULL_POINTER;
  }

  if (!g_bInitialized) {
    return RET_TYPE_NOT_INITIALIZED;
  }

  sPinState_t *psPin = psFindPin(pcPinName);
  if (psPin == NULL || !psPin->bConfigured) {
    return RET_TYPE_NOT_FOUND;
  }

  if (psPin->eDirection != GPIO_DIR_OUTPUT) {
    return RET_TYPE_INVALID_STATE;
  }

  psPin->bValue = bValue;
  vWritePinStateToFile(pcPinName, bValue);

  return RET_TYPE_SUCCESS;
}

// Export interface structure
const sGpioInterface_t sGpioInterfaceWindows = {
    .vHalGpioInitFunc = vGpioWindowsInit,
    .eHalGpioConfigureFunc = eGpioWindowsConfigure,
    .eHalGpioReadFunc = eGpioWindowsRead,
    .eHalGpioWriteFunc = eGpioWindowsWrite};

#endif // _WIN32
