/*****************************************************************************/
/*                        Utility functions and macros                       */
/*****************************************************************************/
#ifndef UTIL_H_
#define UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define ARRAY_SIZE(x) ((sizeof(x)) / (sizeof(x[0])))

#ifndef UINT8_WIDTH
#define UINT8_WIDTH (8U)
#endif

#ifndef UINT16_WIDTH
#define UINT16_WIDTH (16U)
#endif

#ifndef UINT32_WIDTH
#define UINT32_WIDTH (32U)
#endif

#ifndef UINT64_WIDTH
#define UINT64_WIDTH (64U)
#endif

#ifndef INT8_WIDTH
#define int8_WIDTH (8U)
#endif

#ifndef INT16_WIDTH
#define INT16_WIDTH (16U)
#endif

#ifndef INT32_WIDTH
#define INT32_WIDTH (32U)
#endif

#ifndef INT64_WIDTH
#define INT64_WIDTH (64U)
#endif

#ifndef MIN
#define MIN(a, b)                                                                                  \
    ({                                                                                             \
        __typeof__(a) _a = (a);                                                                    \
        __typeof__(b) _b = (b);                                                                    \
        _a < _b ? _a : _b;                                                                         \
    })
#endif

#ifndef MAX
#define MAX(a, b)                                                                                  \
    ({                                                                                             \
        __typeof__(a) _a = (a);                                                                    \
        __typeof__(b) _b = (b);                                                                    \
        _a > _b ? _a : _b;                                                                         \
    })
#endif

/**
 * @brief Get the most significant bit of a u16 integer
 */
uint8_t msb_u16(uint16_t val);

/**
 * @brief Get the least significant bit of a u16 integer
 */
uint8_t lsb_u16(uint16_t val);

/**
 * @brief Get the maximum value of an n bits wide u8 integer
 */
uint8_t max_n_bit_value_u8(uint8_t n);

/**
 * @brief Get the maximum value of an n bits wide u16 integer
 */
uint16_t max_n_bit_value_u16(uint8_t n);

/**
 * @brief Get the maximum value of an n bits wide u32 integer
 */
uint32_t max_n_bit_value_u32(uint8_t n);

/**
 * @brief Get the maximum value of an n bits wide u64 integer
 */
uint64_t max_n_bit_value_u64(uint8_t n);

/**
 * @brief Convert a u8 array into a u16 int.
 */
uint16_t byte_array_to_u16(const uint8_t array[2]);

#ifdef __cplusplus
}
#endif

#endif // UTIL_H_
