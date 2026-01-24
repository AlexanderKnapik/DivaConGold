/*****************************************************************************/
/*                   MPR121 Capacitive Touch Sensor Driver                   */
/*****************************************************************************/
#ifndef MPR121_H_
#define MPR121_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "peripheral/i2c.h"

#include <stdint.h>

#define MPR121_ECR_CL_Pos (6U)         /* Calibration Lock */
#define MPR121_ECR_ELE_EN_Pos (0U)     /* Electrode Enable */
#define MPR121_ECR_ELEPROX_EN_Pos (4U) /* Proximity Enable */

#define MPR121_FILTER_CDC_CONFIG_FFI_Pos (6U) /* First Filter Iterations */
#define MPR121_FILTER_CDC_CONFIG_CDC_Pos (0U) /* Charge Discharge Current */

#define MPR121_FILTER_CDT_CONFIG_ESI_Pos (0U) /* Electrode Sample Interval */
#define MPR121_FILTER_CDT_CONFIG_SFI_Pos (3U) /* Second Filter Iterations */
#define MPR121_FILTER_CDT_CONFIG_CDT_Pos (5U) /* Charge Discharge Time */

enum mpr121_first_filter_samples {
    MPR121_FIRST_FILTER_SAMPLES_6 = (0U << MPR121_FILTER_CDC_CONFIG_FFI_Pos),
    MPR121_FIRST_FILTER_SAMPLES_10 = (1U << MPR121_FILTER_CDC_CONFIG_FFI_Pos),
    MPR121_FIRST_FILTER_SAMPLES_18 = (2U << MPR121_FILTER_CDC_CONFIG_FFI_Pos),
    MPR121_FIRST_FILTER_SAMPLES_34 = (3U << MPR121_FILTER_CDC_CONFIG_FFI_Pos),
};

enum mpr121_second_filter_samples {
    MPR121_SECOND_FILTER_SAMPLES_4 = (0U << MPR121_FILTER_CDT_CONFIG_SFI_Pos),
    MPR121_SECOND_FILTER_SAMPLES_6 = (1U << MPR121_FILTER_CDT_CONFIG_SFI_Pos),
    MPR121_SECOND_FILTER_SAMPLES_10 = (2U << MPR121_FILTER_CDT_CONFIG_SFI_Pos),
    MPR121_SECOND_FILTER_SAMPLES_18 = (3U << MPR121_FILTER_CDT_CONFIG_SFI_Pos),
};

enum mpr121_charge_discharge_time {
    MPR121_CDT_DISABLE = (0U << MPR121_FILTER_CDT_CONFIG_CDT_Pos),
    MPR121_CDT_500_ns = (1 << MPR121_FILTER_CDT_CONFIG_CDT_Pos),
    MPR121_CDT_1_us = (2 << MPR121_FILTER_CDT_CONFIG_CDT_Pos),
    MPR121_CDT_2_us = (3 << MPR121_FILTER_CDT_CONFIG_CDT_Pos),
    MPR121_CDT_4_us = (4 << MPR121_FILTER_CDT_CONFIG_CDT_Pos),
    MPR121_CDT_8_us = (5 << MPR121_FILTER_CDT_CONFIG_CDT_Pos),
    MPR121_CDT_16_us = (6 << MPR121_FILTER_CDT_CONFIG_CDT_Pos),
    MPR121_CDT_32_us = (7 << MPR121_FILTER_CDT_CONFIG_CDT_Pos),
};

enum mpr121_sample_interval {
    MPR121_SAMPLE_INTERVAL_1_ms = (0U << MPR121_FILTER_CDT_CONFIG_ESI_Pos),
    MPR121_SAMPLE_INTERVAL_2_ms = (1U << MPR121_FILTER_CDT_CONFIG_ESI_Pos),
    MPR121_SAMPLE_INTERVAL_4_ms = (2U << MPR121_FILTER_CDT_CONFIG_ESI_Pos),
    MPR121_SAMPLE_INTERVAL_8_ms = (3U << MPR121_FILTER_CDT_CONFIG_ESI_Pos),
    MPR121_SAMPLE_INTERVAL_16_ms = (4U << MPR121_FILTER_CDT_CONFIG_ESI_Pos),
    MPR121_SAMPLE_INTERVAL_32_ms = (5U << MPR121_FILTER_CDT_CONFIG_ESI_Pos),
    MPR121_SAMPLE_INTERVAL_64_ms = (6U << MPR121_FILTER_CDT_CONFIG_ESI_Pos),
    MPR121_SAMPLE_INTERVAL_128_ms = (7U << MPR121_FILTER_CDT_CONFIG_ESI_Pos),
};

enum mpr121_i2c_addr {
    MPR121_I2C_ADDR_0 = 0x5A,
    MPR121_I2C_ADDR_1 = 0x5B,
    MPR121_I2C_ADDR_2 = 0x5C,
    MPR121_I2C_ADDR_3 = 0x5D,
};

enum mpr121_calibration_lock {
    MPR121_ECR_CL_ENABLED_BVR = (0U << MPR121_ECR_CL_Pos),
    MPR121_ECR_CL_DISABLED = (1U << MPR121_ECR_CL_Pos),
    MPR121_ECR_CL_ENABLED_ELE0_H = (2U << MPR121_ECR_CL_Pos),
    MPR121_ECR_CL_ENABLED_ELE0_L = (3U << MPR121_ECR_CL_Pos),
};

enum mpr121_electrode_enable {
    MPR121_ELE_DISABLED = (0U << MPR121_ECR_ELE_EN_Pos),
    MPR121_ELE_EN_ELE0 = (1U << MPR121_ECR_ELE_EN_Pos),
    MPR121_ELE_EN_ELE0_ELE1 = (2U << MPR121_ECR_ELE_EN_Pos),
    MPR121_ELE_EN_ELE0_ELE2 = (3U << MPR121_ECR_ELE_EN_Pos),
    MPR121_ELE_EN_ELE0_ELE3 = (4U << MPR121_ECR_ELE_EN_Pos),
    MPR121_ELE_EN_ELE0_ELE4 = (5U << MPR121_ECR_ELE_EN_Pos),
    MPR121_ELE_EN_ELE0_ELE5 = (6U << MPR121_ECR_ELE_EN_Pos),
    MPR121_ELE_EN_ELE0_ELE6 = (7U << MPR121_ECR_ELE_EN_Pos),
    MPR121_ELE_EN_ELE0_ELE7 = (8U << MPR121_ECR_ELE_EN_Pos),
    MPR121_ELE_EN_ELE0_ELE8 = (9U << MPR121_ECR_ELE_EN_Pos),
    MPR121_ELE_EN_ELE0_ELE9 = (10U << MPR121_ECR_ELE_EN_Pos),
    MPR121_ELE_EN_ELE0_ELE10 = (11U << MPR121_ECR_ELE_EN_Pos),
    MPR121_ELE_EN_ELE0_ELE11 = (12U << MPR121_ECR_ELE_EN_Pos),
};

enum mpr121_eleprox_enable {
    MPR121_ELEPROX_DISABLED = (0U << MPR121_ECR_ELEPROX_EN_Pos),
    MPR121_ELEPROX_ELE0_ELE1 = (1U << MPR121_ECR_ELEPROX_EN_Pos),
    MPR121_ELEPROX_ELE0_ELE3 = (2U << MPR121_ECR_ELEPROX_EN_Pos),
    MPR121_ELEPROX_ELE0_ELE11 = (3U << MPR121_ECR_ELEPROX_EN_Pos),
};

struct mpr121_filter_control_value_edge {
    uint8_t max_half_delta;     /* 0 to 63 */
    uint8_t noise_half_delta;   /* 0 to 63 */
    uint8_t noise_count_limit;  /* 0 to 255 */
    uint8_t filter_delay_limit; /* 0 to 255 */
};

struct mpr121_filter_control_value_touched {
    uint8_t noise_half_delta;   /* 0 to 63 */
    uint8_t noise_count_limit;  /* 0 to 255 */
    uint8_t filter_delay_limit; /* 0 to 255 */
};

struct mpr121_filter_control {
    struct mpr121_filter_control_value_edge rising;
    struct mpr121_filter_control_value_edge falling;
    struct mpr121_filter_control_value_touched touched;
};

struct mpr121_config {
    struct i2c_config i2c;

    enum mpr121_electrode_enable electrodes;
    enum mpr121_eleprox_enable eleprox;
    enum mpr121_calibration_lock calibration_lock;
    bool auto_configure;

    uint8_t touch_threshold;   /* 0 to 255 */
    uint8_t release_threshold; /* 0 to 255 */
    uint8_t touch_debounce;    /* 0 to 7 */
    uint8_t release_debounce;  /* 0 to 7 */

    struct mpr121_filter_control electrode_filter;
    struct mpr121_filter_control eleprox_filter;

    /* Filter and Global CDC/CDR Configuration (0x5C, 0x5D) */
    enum mpr121_first_filter_samples first_filter_samples;
    enum mpr121_second_filter_samples second_filter_samples;
    uint8_t charge_discharge_current_uA; /* 0 to 63 */
    enum mpr121_charge_discharge_time charge_discharge_time;
    enum mpr121_sample_interval sample_interval;
};

typedef struct mpr121_handle *mpr121_handle_t;
typedef const struct mpr121_handle *mpr121_const_handle_t;

/**
 * @brief Initialise the MPR121 IC and I2C interface.
 *
 * @param [in] config The configuration to initialise with.
 * @param [in] addr The I2C address to initialise with the configuration.
 * @return mpr121_handle_t
 * @retval NULL The MPR121 failed to initialise.
 */
mpr121_handle_t mpr121_open(const struct mpr121_config *config, enum mpr121_i2c_addr addr);

/**
 * @brief De-initialise the MPR121.
 *
 * Will exit early if attempting to de-initialise a NULL pointer.
 *
 * @todo Currently, de-initialising one MPR121 will de-initialise the entire
 * I2C interface for all devices on the bus.
 *
 * @param mpr121 The MPR121 to de-initialise.
 * @return None.
 */
void mpr121_close(mpr121_handle_t mpr121);

/**
 * @brief Read the touched status of all electrodes.
 *
 * The stored status is a mask of all electrodes with electrode 0 being bit 0.
 *
 * @param [in] mpr121 The MPR121 to read from.
 * @param [out] status Pointer to store the touched state of all electrodes.
 * @return Error code.
 * @retval E_NULL_POINTER Null MPR121.
 * @retval E_TIMEOUT The read timed out
 * @retval E_IO The read only partially completed.
 */
enum error mpr121_read(mpr121_const_handle_t mpr121, uint16_t *status);

/**
 * @brief Read the touched status of a single electrode.
 *
 * @param [in] mpr121 The MPR121 to read from.
 * @param [in] pin The electrode to read. Must be from 0 to 12.
 * @param [out] status Pointer to store the touched state of all electrodes.
 * @return Error code.
 * @retval E_NULL_POINTER Null MPR121.
 * @retval E_INVALID_INPUT Invalid electrode pin.
 * @retval E_TIMEOUT The read timed out
 * @retval E_IO The read only partially completed.
 * @retval E_HARDWARE Over-current detected on REXT pin. When detected, the
 * MPR121 goes to STOP mode, registers 0x00-0x2A and bits [5:0] are cleared
 * in MPR121_ECR.
 */
enum error mpr121_read_pin(mpr121_const_handle_t mpr121, uint8_t pin, bool *status);

#ifdef __cplusplus
}
#endif

#endif // MPR121_H_
