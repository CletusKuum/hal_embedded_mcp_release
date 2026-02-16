//==============================================================================
// GPIO Library - AVR GPIO Implementation
//------------------------------------------------------------------------------
//! @file
//! @brief AVR-specific GPIO implementation using direct register access
//------------------------------------------------------------------------------

#ifdef PLATFORM_AVR

// Includes ====================================================================
#include "config/gpio_config.h" // Configuration for all pins - SINGLE SOURCE OF TRUTH
#include "gpioLib.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants ===================================================================
#define MAX_PINS 32
#define MAX_PIN_NAME_LEN 32

// AVR Port register addresses (for direct access)
#define AVR_PORT_A 0
#define AVR_PORT_B 1
#define AVR_PORT_C 2
#define AVR_PORT_D 3
#define AVR_PORT_E 4
#define AVR_PORT_F 5

// Type Definitions ============================================================
// Internal pin state tracking
typedef struct {
  char acPinName[MAX_PIN_NAME_LEN];
  bool bConfigured;
  volatile uint8_t *pu8DdrReg;  // Pointer to DDR register (DDRB, DDRC, etc.)
  volatile uint8_t *pu8PortReg; // Pointer to PORT register (PORTB, PORTC, etc.)
  volatile uint8_t *pu8PinReg;  // Pointer to PIN register (PINB, PINC, etc.)
  uint8_t u8PinMask;            // Pin bitmask (1 << pin_number)
  eGpioDirection_t eDirection;
  eGpioPull_t ePull;
  bool bValue;
  bool bSimulatedValue; // Stored state from Digital Twin
} sPinState_t;

// Static Variables ============================================================
static sPinState_t g_psPins[MAX_PINS] = {0};
static uint8_t g_u8PinCount = 0;
static bool g_bInitialized = false;

// Private Functions ===========================================================

/**
 * @brief Get AVR port register pointers based on port index
 */
static void vGetAvrPortRegisters(uint8_t u8PortIndex,
                                 volatile uint8_t **ppu8Ddr,
                                 volatile uint8_t **ppu8Port,
                                 volatile uint8_t **ppu8Pin) {
  switch (u8PortIndex) {
#ifdef DDRA
  case AVR_PORT_A:
    *ppu8Ddr = &DDRA;
    *ppu8Port = &PORTA;
    *ppu8Pin = &PINA;
    break;
#endif
  case AVR_PORT_B:
    *ppu8Ddr = &DDRB;
    *ppu8Port = &PORTB;
    *ppu8Pin = &PINB;
    break;
  case AVR_PORT_C:
    *ppu8Ddr = &DDRC;
    *ppu8Port = &PORTC;
    *ppu8Pin = &PINC;
    break;
  case AVR_PORT_D:
    *ppu8Ddr = &DDRD;
    *ppu8Port = &PORTD;
    *ppu8Pin = &PIND;
    break;
#ifdef DDRE
  case AVR_PORT_E:
    *ppu8Ddr = &DDRE;
    *ppu8Port = &PORTE;
    *ppu8Pin = &PINE;
    break;
#endif
#ifdef DDRF
  case AVR_PORT_F:
    *ppu8Ddr = &DDRF;
    *ppu8Port = &PORTF;
    *ppu8Pin = &PINF;
    break;
#endif
  default:
    // Default to PORTB
    *ppu8Ddr = &DDRB;
    *ppu8Port = &PORTB;
    *ppu8Pin = &PINB;
    break;
  }
}

/**
 * @brief Find pin by name
 */
static sPinState_t *psFindPin(const char *pcPinName) {
  for (uint8_t i = 0; i < g_u8PinCount; i++) {
    if (strcmp(g_psPins[i].acPinName, pcPinName) == 0) {
      return &g_psPins[i];
    }
  }
  return NULL;
}

// Forward Declarations =======================================================
eRetType_t eGpioAVRConfigure(const sGpioConfig_t *psConfig);
eRetType_t eGpioAVRRead(const char *pcPinName, bool *pbValue);
eRetType_t eGpioAVRWrite(const char *pcPinName, bool bValue);

// Functions ===================================================================

/**
 * @brief Initialize the AVR GPIO interface
 * Configures all pins from config/gpio_config.c - SINGLE SOURCE OF TRUTH
 */
void vGpioAVRInit(void) {
  if (g_bInitialized) {
    return;
  }

  g_bInitialized = true;
  // printf("[GPIO AVR] Initializing AVR GPIO implementation (direct register
  // access)...\n");

  // Configure pins from config file - SINGLE SOURCE OF TRUTH
  // printf("[GPIO AVR] Configuring GPIO pins from config...\n");

  const sGpioPinConfig_t *psPinConfig = g_psGpioPinConfigs;
  uint8_t u8ConfiguredCount = 0;

  while (psPinConfig->pcPinName != NULL) {
    // Get AVR port register pointers based on platform-specific config
    volatile uint8_t *pu8Ddr = NULL;
    volatile uint8_t *pu8Port = NULL;
    volatile uint8_t *pu8Pin = NULL;
    vGetAvrPortRegisters(psPinConfig->u8AvrPort, &pu8Ddr, &pu8Port, &pu8Pin);

    uint8_t u8PinMask = (1 << psPinConfig->u8AvrPin);

    // printf("[GPIO AVR] Config: %s -> Port:%d, Pin:%d\n",
    // psPinConfig->pcPinName,
    //        psPinConfig->u8AvrPort, psPinConfig->u8AvrPin);

    // Configure pin based on direction
    if (psPinConfig->eDirection == GPIO_DIR_OUTPUT) {
      // Set as output: set bit in DDR register
      *pu8Ddr |= u8PinMask;

      // Set initial output value (default LOW)
      *pu8Port &= ~u8PinMask;
    } else // GPIO_DIR_INPUT
    {
      // Set as input: clear bit in DDR register
      *pu8Ddr &= ~u8PinMask;

      // Configure pull-up/pull-down
      if (psPinConfig->ePull == GPIO_PULL_UP) {
        // Enable pull-up: set bit in PORT register
        *pu8Port |= u8PinMask;
      } else if (psPinConfig->ePull == GPIO_PULL_DOWN) {
        // AVR doesn't have internal pull-down, so disable pull-up
        *pu8Port &= ~u8PinMask;
      } else // GPIO_PULL_NONE
      {
        // No pull: clear bit in PORT register
        *pu8Port &= ~u8PinMask;
      }
    }

    // Store pin state for read/write operations
    if (g_u8PinCount < MAX_PINS) {
      sPinState_t *psPin = &g_psPins[g_u8PinCount];
      strncpy(psPin->acPinName, psPinConfig->pcPinName, MAX_PIN_NAME_LEN - 1);
      psPin->acPinName[MAX_PIN_NAME_LEN - 1] = '\0';
      psPin->bConfigured = true;
      psPin->pu8DdrReg = pu8Ddr;
      psPin->pu8PortReg = pu8Port;
      psPin->pu8PinReg = pu8Pin;
      psPin->u8PinMask = u8PinMask;
      psPin->eDirection = psPinConfig->eDirection;
      psPin->ePull = psPinConfig->ePull;
      psPin->bValue = false;

      // Default simulated state depends on Pull (Active Low -> Default High)
      if (psPin->ePull == GPIO_PULL_UP) {
        psPin->bSimulatedValue = true; // Default Released (High)
      } else {
        psPin->bSimulatedValue = false; // Default Low
      }

      g_u8PinCount++;

      u8ConfiguredCount++;

      const char *pcPortName = "UNKNOWN";
      switch (psPinConfig->u8AvrPort) {
      case AVR_PORT_A:
        pcPortName = "PORTA";
        break;
      case AVR_PORT_B:
        pcPortName = "PORTB";
        break;
      case AVR_PORT_C:
        pcPortName = "PORTC";
        break;
      case AVR_PORT_D:
        pcPortName = "PORTD";
        break;
      case AVR_PORT_E:
        pcPortName = "PORTE";
        break;
      case AVR_PORT_F:
        pcPortName = "PORTF";
        break;
      }

      // printf("[GPIO AVR] [OK] %s configured (%s, %s, %s Pin %u)\n",
      //        psPinConfig->pcPinName,
      //        (psPinConfig->eDirection == GPIO_DIR_OUTPUT) ? "OUTPUT" :
      //        "INPUT", (psPinConfig->ePull == GPIO_PULL_UP)     ? "PULL_UP" :
      //        (psPinConfig->ePull == GPIO_PULL_DOWN) ? "PULL_DOWN"
      //                                                 : "NONE",
      //        pcPortName, psPinConfig->u8AvrPin);
    } else {
      // printf("[GPIO AVR] [WARNING] Maximum pin count reached, skipping %s\n",
      //        psPinConfig->pcPinName);
    }

    psPinConfig++;
  }

  // printf(
  //     "[GPIO AVR] Initialization complete. Configured %u pins from
  //     config.\n", u8ConfiguredCount);
}

/**
 * @brief Configure a GPIO pin (called during Init, or manually)
 */
eRetType_t eGpioAVRConfigure(const sGpioConfig_t *psConfig) {
  if (psConfig == NULL || psConfig->pcPinName == NULL) {
    return RET_TYPE_NULL_POINTER;
  }

  if (!g_bInitialized) {
    return RET_TYPE_NOT_INITIALIZED;
  }

  // Find pin in our config
  sPinState_t *psPin = psFindPin(psConfig->pcPinName);
  if (psPin != NULL && psPin->bConfigured) {
    // Pin already configured during Init
    return RET_TYPE_SUCCESS;
  }

  return RET_TYPE_FAIL; // Pin name not found in config
}

/**
 * @brief Read GPIO pin state
 */
eRetType_t eGpioAVRRead(const char *pcPinName, bool *pbValue) {
  if (pcPinName == NULL || pbValue == NULL) {
    return RET_TYPE_NULL_POINTER;
  }

  if (!g_bInitialized) {
    return RET_TYPE_NOT_INITIALIZED;
  }

  sPinState_t *psPin = psFindPin(pcPinName);
  if (psPin == NULL || !psPin->bConfigured) {
    return RET_TYPE_FAIL;
  }

  // Read from PIN register (for input) or PORT register (for output)
  // Read from PIN register (for input) or PORT register (for output)
  if (psPin->eDirection == GPIO_DIR_INPUT) {
    // Input: read PIN register
    // RAW PHYSICAL ONLY - Merging happens in Helper
    *pbValue = ((*(psPin->pu8PinReg)) & psPin->u8PinMask) != 0;
  } else {
    // Output: read last written value (from PORT register)
    *pbValue = ((*(psPin->pu8PortReg)) & psPin->u8PinMask) != 0;
  }

  psPin->bValue = *pbValue;
  return RET_TYPE_SUCCESS;
}

/**
 * @brief Set the simulated input value from Digital Twin
 */
void vGpioAVRSetSimulated(const char *pcPinName, bool bValue) {
  if (!g_bInitialized)
    return;

  sPinState_t *psPin = psFindPin(pcPinName);
  if (psPin != NULL) {
    psPin->bSimulatedValue = bValue;
  }
}

/**
 * @brief Get the simulated input value from Digital Twin
 */
bool bGpioAVRGetSimulated(const char *pcPinName) {
  if (!g_bInitialized)
    return false; // Default released? Or false?

  sPinState_t *psPin = psFindPin(pcPinName);
  if (psPin != NULL) {
    return psPin->bSimulatedValue;
  }
  return false; // Default
}

/**
 * @brief Write GPIO pin state
 */
eRetType_t eGpioAVRWrite(const char *pcPinName, bool bValue) {
  if (pcPinName == NULL) {
    return RET_TYPE_NULL_POINTER;
  }

  if (!g_bInitialized) {
    return RET_TYPE_NOT_INITIALIZED;
  }

  sPinState_t *psPin = psFindPin(pcPinName);
  if (psPin == NULL || !psPin->bConfigured) {
    return RET_TYPE_FAIL;
  }

  // Only write if pin is configured as output
  if (psPin->eDirection != GPIO_DIR_OUTPUT) {
    return RET_TYPE_INVALID_STATE;
  }

  // Write to PORT register
  if (bValue) {
    *(psPin->pu8PortReg) |= psPin->u8PinMask; // Set HIGH
  } else {
    *(psPin->pu8PortReg) &= ~psPin->u8PinMask; // Set LOW
  }

  psPin->bValue = bValue;
  return RET_TYPE_SUCCESS;
}

// AVR GPIO Interface Structure =============================================
const sGpioInterface_t sGpioInterfaceAVR = {.vHalGpioInitFunc = vGpioAVRInit,
                                            .eHalGpioConfigureFunc =
                                                eGpioAVRConfigure,
                                            .eHalGpioReadFunc = eGpioAVRRead,
                                            .eHalGpioWriteFunc = eGpioAVRWrite};

#endif // PLATFORM_AVR
