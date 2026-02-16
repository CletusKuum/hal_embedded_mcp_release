//==============================================================================
// GPIO Library - STM32 GPIO Implementation
//------------------------------------------------------------------------------
//! @file
//! @brief STM32-specific GPIO implementation using libopencm3
//------------------------------------------------------------------------------

#ifdef PLATFORM_STM32

// Includes ====================================================================
#include "../../config/gpio_config.h" // Configuration for all pins - SINGLE SOURCE OF TRUTH
#include "../../gpioLib.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Constants ===================================================================
#define MAX_PINS 32
#define MAX_PIN_NAME_LEN 32

// Type Definitions ============================================================
// Map libopencm3 port enum to actual GPIO port address
typedef enum {
  STM32_GPIO_PORT_A = 0, // Maps to GPIOA
  STM32_GPIO_PORT_B = 1, // Maps to GPIOB
  STM32_GPIO_PORT_C = 2, // Maps to GPIOC
  STM32_GPIO_PORT_D = 3, // Maps to GPIOD
  STM32_GPIO_PORT_E = 4, // Maps to GPIOE
  STM32_GPIO_PORT_F = 5, // Maps to GPIOF
  STM32_GPIO_PORT_G = 6  // Maps to GPIOG
} eStm32GpioPort_t;

// Internal pin state tracking
typedef struct {
  char acPinName[MAX_PIN_NAME_LEN];
  bool bConfigured;
  uint32_t u32GpioPort; // libopencm3 GPIO port (GPIOA, GPIOB, etc.)
  uint16_t u16GpioPin;  // libopencm3 GPIO pin (GPIO0, GPIO1, etc.)
  eGpioDirection_t eDirection;
  eGpioPull_t ePull;
  bool bValue;
} sPinState_t;

// Static Variables ============================================================
static sPinState_t g_psPins[MAX_PINS] = {0};
static uint8_t g_u8PinCount = 0;
static bool g_bInitialized = false;

// Private Functions ===========================================================

/**
 * @brief Convert STM32 port index to libopencm3 GPIO port constant
 * @param ePortIndex Port index from config (0=GPIOA, 1=GPIOB, etc.)
 * @return libopencm3 GPIO port constant (GPIOA, GPIOB, etc.)
 */
static uint32_t u32GetGpioPort(eStm32GpioPort_t ePortIndex) {
  switch (ePortIndex) {
  case STM32_GPIO_PORT_A:
    return GPIOA;
  case STM32_GPIO_PORT_B:
    return GPIOB;
  case STM32_GPIO_PORT_C:
    return GPIOC;
  case STM32_GPIO_PORT_D:
    return GPIOD;
  case STM32_GPIO_PORT_E:
    return GPIOE;
  case STM32_GPIO_PORT_F:
    return GPIOF;
  case STM32_GPIO_PORT_G:
    return GPIOG;
  default:
    return GPIOA; // Default to GPIOA if invalid
  }
}

/**
 * @brief Enable RCC clock for GPIO port by port index
 */
static void vEnableGpioPortClockByIndex(eStm32GpioPort_t ePortIndex) {
  switch (ePortIndex) {
  case STM32_GPIO_PORT_A:
    rcc_periph_clock_enable(RCC_GPIOA);
    break;
  case STM32_GPIO_PORT_B:
    rcc_periph_clock_enable(RCC_GPIOB);
    break;
  case STM32_GPIO_PORT_C:
    rcc_periph_clock_enable(RCC_GPIOC);
    break;
  case STM32_GPIO_PORT_D:
    rcc_periph_clock_enable(RCC_GPIOD);
    break;
  case STM32_GPIO_PORT_E:
    rcc_periph_clock_enable(RCC_GPIOE);
    break;
  case STM32_GPIO_PORT_F:
    rcc_periph_clock_enable(RCC_GPIOF);
    break;
  case STM32_GPIO_PORT_G:
    rcc_periph_clock_enable(RCC_GPIOG);
    break;
  default:
    break;
  }
}

/**
 * @brief Convert pin number (0-15) to libopencm3 GPIO pin bitmask
 * @param u16PinNumber Pin number from config (0-15)
 * @return libopencm3 GPIO pin bitmask (GPIO0 = (1<<0), GPIO1 = (1<<1), etc.)
 */
static uint16_t u16GetGpioPinMask(uint16_t u16PinNumber) {
  if (u16PinNumber > 15) {
    return 0;
  }
  return (1 << u16PinNumber); // Convert to bitmask: 0->GPIO0, 1->GPIO1, etc.
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
eRetType_t eGpioSTM32Configure(const sGpioConfig_t *psConfig);
eRetType_t eGpioSTM32Read(const char *pcPinName, bool *pbValue);
eRetType_t eGpioSTM32Write(const char *pcPinName, bool bValue);

// Functions ===================================================================

/**
 * @brief Initialize the STM32 GPIO interface
 * Configures all pins from config/gpio_config.c - SINGLE SOURCE OF TRUTH
 */
void vGpioSTM32Init(void) {
  if (g_bInitialized) {
    return;
  }

  g_bInitialized = true;
  printf(
      "[GPIO STM32] Initializing STM32 GPIO implementation (libopencm3)...\n");

  // Configure pins from config file - SINGLE SOURCE OF TRUTH
  // All configurations are in config/gpio_config.c
  // Same config file used by HTTP, Windows, and STM32!
  printf("[GPIO STM32] Configuring pins from config...\n");

  const sGpioPinConfig_t *psPinConfig = g_psGpioPinConfigs;
  uint8_t u8ConfiguredCount = 0;

  while (psPinConfig->pcPinName != NULL) {
    // Use platform-specific STM32 fields from config!
    // These fields only exist when PLATFORM_STM32 is defined
    eStm32GpioPort_t ePortIndex = (eStm32GpioPort_t)psPinConfig->u32Stm32Port;
    uint32_t u32GpioPort = u32GetGpioPort(ePortIndex);
    uint16_t u16GpioPin = u16GetGpioPinMask(psPinConfig->u16Stm32Pin);

    // Enable GPIO port clock (use index to enable correct RCC)
    vEnableGpioPortClockByIndex(ePortIndex);

    // Convert our HAL enums to libopencm3 values
    uint8_t u8Mode = (psPinConfig->eDirection == GPIO_DIR_OUTPUT)
                         ? GPIO_MODE_OUTPUT
                         : GPIO_MODE_INPUT;
    uint8_t u8Pull = 0;
    switch (psPinConfig->ePull) {
    case GPIO_PULL_UP:
      u8Pull = GPIO_PUPD_PULLUP;
      break;
    case GPIO_PULL_DOWN:
      u8Pull = GPIO_PUPD_PULLDOWN;
      break;
    case GPIO_PULL_NONE:
    default:
      u8Pull = GPIO_PUPD_NONE;
      break;
    }

    // Configure GPIO pin using libopencm3
    gpio_mode_setup(u32GpioPort, u8Mode, u8Pull, u16GpioPin);

    // Set alternate function if specified (for UART, I2C, etc.)
    if (psPinConfig->u8Stm32Af != 0) {
      gpio_set_af(u32GpioPort, psPinConfig->u8Stm32Af, u16GpioPin);
    }

    // Store pin state for read/write operations
    if (g_u8PinCount < MAX_PINS) {
      sPinState_t *psPin = &g_psPins[g_u8PinCount];
      strncpy(psPin->acPinName, psPinConfig->pcPinName, MAX_PIN_NAME_LEN - 1);
      psPin->acPinName[MAX_PIN_NAME_LEN - 1] = '\0';
      psPin->bConfigured = true;
      psPin->u32GpioPort = u32GpioPort;
      psPin->u16GpioPin = u16GpioPin;
      psPin->eDirection = psPinConfig->eDirection;
      psPin->ePull = psPinConfig->ePull;
      psPin->bValue = false; // Initialize to LOW
      g_u8PinCount++;
      u8ConfiguredCount++;

      printf("[GPIO STM32] [OK] %s configured (Port %c, Pin %u, %s, %s)\n",
             psPinConfig->pcPinName, 'A' + psPinConfig->u32Stm32Port,
             psPinConfig->u16Stm32Pin,
             (psPinConfig->eDirection == GPIO_DIR_OUTPUT) ? "OUTPUT" : "INPUT",
             (psPinConfig->ePull == GPIO_PULL_UP)     ? "PULL_UP"
             : (psPinConfig->ePull == GPIO_PULL_DOWN) ? "PULL_DOWN"
                                                      : "NONE");
    } else {
      printf("[GPIO STM32] [WARNING] Maximum pin count reached, skipping %s\n",
             psPinConfig->pcPinName);
    }

    psPinConfig++;
  }

  printf(
      "[GPIO STM32] Initialization complete. Configured %u pins from config.\n",
      u8ConfiguredCount);
}

/**
 * @brief Configure a GPIO pin (called during Init, or manually)
 */
eRetType_t eGpioSTM32Configure(const sGpioConfig_t *psConfig) {
  if (psConfig == NULL || psConfig->pcPinName == NULL) {
    return RET_TYPE_NULL_POINTER;
  }

  if (!g_bInitialized) {
    return RET_TYPE_NOT_INITIALIZED;
  }

  // Find pin in our config to get STM32 hardware mapping
  const sGpioPinConfig_t *psPinConfig = g_psGpioPinConfigs;
  while (psPinConfig->pcPinName != NULL) {
    if (strcmp(psPinConfig->pcPinName, psConfig->pcPinName) == 0) {
      // Found the pin config - configure it
      eStm32GpioPort_t ePortIndex = (eStm32GpioPort_t)psPinConfig->u32Stm32Port;
      uint32_t u32GpioPort = u32GetGpioPort(ePortIndex);
      uint16_t u16GpioPin = u16GetGpioPinMask(psPinConfig->u16Stm32Pin);

      vEnableGpioPortClockByIndex(ePortIndex);

      uint8_t u8Mode = (psConfig->eDirection == GPIO_DIR_OUTPUT)
                           ? GPIO_MODE_OUTPUT
                           : GPIO_MODE_INPUT;
      uint8_t u8Pull = 0;
      switch (psConfig->ePull) {
      case GPIO_PULL_UP:
        u8Pull = GPIO_PUPD_PULLUP;
        break;
      case GPIO_PULL_DOWN:
        u8Pull = GPIO_PUPD_PULLDOWN;
        break;
      default:
        u8Pull = GPIO_PUPD_NONE;
        break;
      }

      gpio_mode_setup(u32GpioPort, u8Mode, u8Pull, u16GpioPin);

      if (psPinConfig->u8Stm32Af != 0) {
        gpio_set_af(u32GpioPort, psPinConfig->u8Stm32Af, u16GpioPin);
      }

      return RET_TYPE_SUCCESS;
    }
    psPinConfig++;
  }

  return RET_TYPE_FAIL; // Pin name not found in config
}

/**
 * @brief Read a GPIO pin state
 */
eRetType_t eGpioSTM32Read(const char *pcPinName, bool *pbValue) {
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

  // Read from STM32 GPIO IDR register using libopencm3
  bool bPinValue = gpio_get(psPin->u32GpioPort, psPin->u16GpioPin) != 0;
  *pbValue = bPinValue;
  psPin->bValue = bPinValue; // Update cached value

  return RET_TYPE_SUCCESS;
}

/**
 * @brief Write a GPIO pin state
 */
eRetType_t eGpioSTM32Write(const char *pcPinName, bool bValue) {
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

  if (psPin->eDirection != GPIO_DIR_OUTPUT) {
    return RET_TYPE_FAIL; // Pin is not configured as output
  }

  // Write to STM32 GPIO using libopencm3
  if (bValue) {
    gpio_set(psPin->u32GpioPort, psPin->u16GpioPin); // Set HIGH
  } else {
    gpio_clear(psPin->u32GpioPort, psPin->u16GpioPin); // Set LOW
  }

  psPin->bValue = bValue; // Update cached value

  return RET_TYPE_SUCCESS;
}

// STM32 GPIO Interface Structure =============================================
const sGpioInterface_t sGpioInterfaceSTM32 = {
    .vHalGpioInitFunc = vGpioSTM32Init,
    .eHalGpioConfigureFunc = eGpioSTM32Configure,
    .eHalGpioReadFunc = eGpioSTM32Read,
    .eHalGpioWriteFunc = eGpioSTM32Write};

#endif // PLATFORM_STM32
