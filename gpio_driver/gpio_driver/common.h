//==============================================================================
// Common Definitions
//------------------------------------------------------------------------------
//! @file
//! @brief Common types and definitions
//------------------------------------------------------------------------------

#ifndef COMMON_H
#define COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

// Includes ====================================================================
#include <stdint.h>
#include <stdbool.h>

// Macros ======================================================================

// Type Definitions ============================================================

/**
 * @brief Return type enumeration
 */
typedef enum {
    RET_TYPE_SUCCESS = 0,
    RET_TYPE_FAIL,
    RET_TYPE_NULL_POINTER,
    RET_TYPE_INVALID_PARAMETER,
    RET_TYPE_MEMORY_ERROR,
    RET_TYPE_NOT_FOUND,
    RET_TYPE_ALREADY_EXISTS,
    RET_TYPE_NOT_INITIALIZED,
    RET_TYPE_INVALID_STATE,
    RET_TYPE_NOT_AVAILABLE
} eRetType_t;

// External Variables =========================================================

// Function Prototypes ========================================================

#ifdef __cplusplus
}
#endif

#endif // COMMON_H

