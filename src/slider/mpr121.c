#include "mpr121.h"

#include "common/bsp.h"
#include "common/error.h"
#include "common/util.h"
#include "peripheral/i2c.h"

#include <hardware/gpio.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MPR121_ELECTRODE_COUNT (13U)
#define MPR121_ELECTRODE_MAX_ADDRESS (MPR121_ELECTRODE_COUNT - 1)

#define MPR121_I2C_ADDR_MIN (MPR121_I2C_ADDR_0)
#define MPR121_I2C_ADDR_MAX (MPR121_I2C_ADDR_3)

#define MPR121_TOUCH_STATUS_H_OVCF_Msk (0x80)

#define MPR121_ECR_ELE_EN_Msk (0x0F)
#define MPR121_ECR_ELEPROX_EN_Msk (0x30)

#define MPR121_FILTER_CDC_CONFIG_CDC_Msk (0x3F) /* Charge Discharge Current */
#define MPR121_FILTER_CDC_CONFIG_FFI_Msk (0xC0) /* First Filter Iterations */

#define MPR121_FILTER_CDT_CONFIG_ESI_Msk (0x07) /* Electrode Sample Interval */
#define MPR121_FILTER_CDT_CONFIG_SFI_Msk (0x18) /* Second Filter Iterations */
#define MPR121_FILTER_CDT_CONFIG_CDT_Msk (0xE0) /* Charge Discharge Time */

#define MPR121_MHD_Msk (0x3F) /* Max Half Delta */
#define MPR121_NHD_Msk (0x3F) /* Noise Half Delta */
#define MPR121_NCL_Msk (0xFF) /* Noise Count Limit */
#define MPR121_FDL_Msk (0xFF) /* Filter Delay Limit */

#define MPR121_AUTOCONF_L_ENABLE_Pos (1 << 0U)        /* Automatic Configuration Enable */
#define MPR121_AUTOCONF_L_RECONF_ENABLE_Pos (1 << 1U) /* Automatic Reconfiguration Enable */
#define MPR121_AUTOCONF_L_BVA_Pos (1 << 2U)           /* Baseline Value Adjust */
#define MPR121_AUTOCONF_L_RETRY_Pos (1 << 4U)
#define MPR121_AUTOCONF_L_RETRY_NONE_Msk (0x00 << 4U)
#define MPR121_AUTOCONF_L_RETRY_2_Msk (0x01 << 4U)
#define MPR121_AUTOCONF_L_RETRY_4_Msk (0x10 << 4U)
#define MPR121_AUTOCONF_L_RETRY_8_Msk (0x11 << 4U)
#define MPR121_AUTOCONF_L_FFI_Pos (1 << 4U)

#define MPR121_AUTOCONF_H_ACFIE_Pos (1 << 0U)
#define MPR121_AUTOCONF_H_ARFIE_Pos (1 << 1U)
#define MPR121_AUTOCONF_H_OORIE_Pos (1 << 2U)
#define MPR121_AUTOCONF_H_SCTS_Pos (1 << 7U)

#define MPR121_ECR_ADDR (0x5E) /* Electrode Configuration Register */

#define MPR121_DEBOUNCE_TOUCH_Pos (0U)
#define MPR121_DEBOUNCE_TOUCH_Msk (0x07U)
#define MPR121_DEBOUNCE_RELEASE_Pos (4U)
#define MPR121_DEBOUNCE_RELEASE_Msk (0x70U)
#define MPR121_DEBOUNCE_MAX_VALUE (7U)

#define MPR121_SOFTRESET_TRIGGER (0x63U)

enum mpr121_baseline_value_adjust {
    /* Baseline is not changed */
    MPR121_BASELINE_ADJ_DISABLED = (0U << MPR121_AUTOCONF_L_BVA_Pos),
    /* Baseline is cleared */
    MPR121_BASELINE_ADJ_CLEARED = (0U << MPR121_AUTOCONF_L_BVA_Pos),
    /* Baseline is set to AUTO-CONFIG baseline with lower 3 bits cleared */
    /* Clearing 3 LSB helps protect against false touches */
    MPR121_BASELINE_ADJ_AUTO_3_LSB_CLEARED = (0U << MPR121_AUTOCONF_L_BVA_Pos),
    /* Baseline is set to the AUTO-CONFIG baseline */
    MPR121_BASELINE_ADJ_AUTO_ALL = (0U << MPR121_AUTOCONF_L_BVA_Pos),
};

enum mpr121_register {
    MPR121_TOUCH_STATUS_L = 0x00,
    MPR121_TOUCH_STATUS_H = 0x01,

    MPR121_OUT_OF_RANGE_L = 0x02,
    MPR121_OUT_OF_RANGE_H = 0x03,

    MPR121_FILT_DATA_0_LSB = 0x04,
    MPR121_FILT_DATA_0_MSB = 0x05,
    MPR121_FILT_DATA_1_LSB = 0x06,
    MPR121_FILT_DATA_1_MSB = 0x07,
    MPR121_FILT_DATA_2_LSB = 0x08,
    MPR121_FILT_DATA_2_MSB = 0x09,
    MPR121_FILT_DATA_3_LSB = 0x0A,
    MPR121_FILT_DATA_3_MSB = 0x0B,
    MPR121_FILT_DATA_4_LSB = 0x0C,
    MPR121_FILT_DATA_4_MSB = 0x0D,
    MPR121_FILT_DATA_5_LSB = 0x0E,
    MPR121_FILT_DATA_5_MSB = 0x0F,
    MPR121_FILT_DATA_6_LSB = 0x10,
    MPR121_FILT_DATA_6_MSB = 0x11,
    MPR121_FILT_DATA_7_LSB = 0x12,
    MPR121_FILT_DATA_7_MSB = 0x13,
    MPR121_FILT_DATA_8_LSB = 0x14,
    MPR121_FILT_DATA_8_MSB = 0x15,
    MPR121_FILT_DATA_9_LSB = 0x16,
    MPR121_FILT_DATA_9_MSB = 0x17,
    MPR121_FILT_DATA_10_LSB = 0x18,
    MPR121_FILT_DATA_10_MSB = 0x19,
    MPR121_FILT_DATA_11_LSB = 0x1A,
    MPR121_FILT_DATA_11_MSB = 0x1B,
    MPR121_FILT_DATA_ELEPROX_LSB = 0x1C,
    MPR121_FILT_DATA_ELEPROX_MSB = 0x1D,

    MPR121_BASELINE_0 = 0x1E,
    MPR121_BASELINE_1 = 0x1F,
    MPR121_BASELINE_2 = 0x20,
    MPR121_BASELINE_3 = 0x21,
    MPR121_BASELINE_4 = 0x22,
    MPR121_BASELINE_5 = 0x23,
    MPR121_BASELINE_6 = 0x24,
    MPR121_BASELINE_7 = 0x25,
    MPR121_BASELINE_8 = 0x26,
    MPR121_BASELINE_9 = 0x27,
    MPR121_BASELINE_10 = 0x28,
    MPR121_BASELINE_11 = 0x29,
    MPR121_BASELINE_ELEPROX = 0x2A,

    MPR121_MHD_RISING = 0x2B,  /* Maximum Half Delta */
    MPR121_NHD_RISING = 0x2C,  /* Noise Half Delta */
    MPR121_NCL_RISING = 0x2D,  /* Noise Count Limit */
    MPR121_FDL_RISING = 0x2E,  /* Filter Delay Count Limit */
    MPR121_MHD_FALLING = 0x2F, /* Maximum Half Delta */
    MPR121_NHD_FALLING = 0x30, /* Noise Half Delta */
    MPR121_NCL_FALLING = 0x31, /* Noise Count Limit */
    MPR121_FDL_FALLING = 0x32, /* Filter Delay Count Limit */
    MPR121_NHD_TOUCHED = 0x33, /* Noise Half Delta Amount */
    MPR121_NCL_TOUCHED = 0x34, /* Noise Count Limit */
    MPR121_FDL_TOUCHED = 0x35, /* Filter Delay Count Limit */

    MPR121_ELEPROX_MHD_RISING = 0x36,  /* Maximum Half Delta */
    MPR121_ELEPROX_NHD_RISING = 0x37,  /* Noise Half Delta */
    MPR121_ELEPROX_NCL_RISING = 0x38,  /* Noise Count Limit */
    MPR121_ELEPROX_FDL_RISING = 0x39,  /* Filter Delay Count Limit */
    MPR121_ELEPROX_MHD_FALLING = 0x3A, /* Maximum Half Delta */
    MPR121_ELEPROX_NHD_FALLING = 0x3B, /* Noise Half Delta */
    MPR121_ELEPROX_NCL_FALLING = 0x3C, /* Noise Count Limit */
    MPR121_ELEPROX_FDL_FALLING = 0x3D, /* Filter Delay Count Limit */
    MPR121_ELEPROX_NHD_TOUCHED = 0x3E, /* Noise Half Delta Amount */
    MPR121_ELEPROX_NCL_TOUCHED = 0x3F, /* Noise Count Limit */
    MPR121_ELEPROX_FDL_TOUCHED = 0x40, /* Filter Delay Count Limit */

    MPR121_TOUCH_THRESHOLD_0 = 0x41,
    MPR121_RELEASE_THRESHOLD_0 = 0x42,
    MPR121_TOUCH_THRESHOLD_1 = 0x43,
    MPR121_RELEASE_THRESHOLD_1 = 0x44,
    MPR121_TOUCH_THRESHOLD_2 = 0x45,
    MPR121_RELEASE_THRESHOLD_2 = 0x46,
    MPR121_TOUCH_THRESHOLD_3 = 0x47,
    MPR121_RELEASE_THRESHOLD_3 = 0x48,
    MPR121_TOUCH_THRESHOLD_4 = 0x49,
    MPR121_RELEASE_THRESHOLD_4 = 0x4A,
    MPR121_TOUCH_THRESHOLD_5 = 0x4B,
    MPR121_RELEASE_THRESHOLD_5 = 0x4C,
    MPR121_TOUCH_THRESHOLD_6 = 0x4D,
    MPR121_RELEASE_THRESHOLD_6 = 0x4E,
    MPR121_TOUCH_THRESHOLD_7 = 0x4F,
    MPR121_RELEASE_THRESHOLD_7 = 0x50,
    MPR121_TOUCH_THRESHOLD_8 = 0x51,
    MPR121_RELEASE_THRESHOLD_8 = 0x52,
    MPR121_TOUCH_THRESHOLD_9 = 0x53,
    MPR121_RELEASE_THRESHOLD_9 = 0x54,
    MPR121_TOUCH_THRESHOLD_10 = 0x55,
    MPR121_RELEASE_THRESHOLD_10 = 0x56,
    MPR121_TOUCH_THRESHOLD_11 = 0x57,
    MPR121_RELEASE_THRESHOLD_11 = 0x58,
    MPR121_TOUCH_THRESHOLD_ELEPROX = 0x59,
    MPR121_RELEASE_THRESHOLD_ELEPROX = 0x5A,

    MPR121_DEBOUNCE = 0x5B,
    MPR121_FILTER_CDC_CONFIG = 0x5C, /* Filter and Charge Discharge Current */
    MPR121_FILTER_CDT_CONFIG = 0x5D, /* Filter and Charge Discharge Time */
    MPR121_ELECTRODE_CONFIG = 0x5E,

    MPR121_CURRENT_0 = 0x5F,
    MPR121_CURRENT_1 = 0x60,
    MPR121_CURRENT_2 = 0x61,
    MPR121_CURRENT_3 = 0x62,
    MPR121_CURRENT_4 = 0x63,
    MPR121_CURRENT_5 = 0x64,
    MPR121_CURRENT_6 = 0x65,
    MPR121_CURRENT_7 = 0x66,
    MPR121_CURRENT_8 = 0x67,
    MPR121_CURRENT_9 = 0x68,
    MPR121_CURRENT_10 = 0x69,
    MPR121_CURRENT_11 = 0x6A,
    MPR121_CURRENT_ELEPROX = 0x6B,

    MPR121_CHARGE_TIME_0_1 = 0x6C,
    MPR121_CHARGE_TIME_2_3 = 0x6D,
    MPR121_CHARGE_TIME_4_5 = 0x6E,
    MPR121_CHARGE_TIME_6_7 = 0x6F,
    MPR121_CHARGE_TIME_8_9 = 0x70,
    MPR121_CHARGE_TIME_10_11 = 0x71,
    MPR121_CHARGE_TIME_ELEPROX = 0x72,

    MPR121_GPIO_CTRL_0 = 0x73,
    MPR121_GPIO_CTRL_1 = 0x74,
    MPR121_GPIO_DATA = 0x75,
    MPR121_GPIO_DIR = 0x76,
    MPR121_GPIO_EN = 0x77,
    MPR121_GPIO_SET = 0x78,
    MPR121_GPIO_CLR = 0x79,
    MPR121_GPIO_TOGGLE = 0x7A,
    MPR121_AUTO_CONFIG_0 = 0x7B,
    MPR121_AUTO_CONFIG_1 = 0x7C,
    MPR121_UP_LIMIT = 0x7D,
    MPR121_LOW_LIMIT = 0x7E,
    MPR121_TARGET_LIMIT = 0x7F,

    MPR121_SOFT_RESET = 0x80,
};

enum mpr121_mode {
    MPR121_MODE_STOP = 0,
    MPR121_MODE_RUN,
};

struct mpr121_handle {
    i2c_inst_t *i2c;
    uint8_t i2c_address;
    enum mpr121_mode mode;
    uint8_t ecr; /* Saved Electrode Configuration Register */
};

struct write_task {
    uint8_t reg;
    uint8_t data;
};

static enum mpr121_mode get_mode(mpr121_const_handle_t mpr121)
{
    return mpr121->mode;
}

/**
 * @brief Write a single byte to the MPR121.
 *
 * The MPR121 only supports a single byte write mode.
 *
 * @param [in] mpr121 The MPR121 to write to.
 * @param [in] reg The register to write to.
 * @param [in] data The data to write to the register.
 * @return Error code.
 */
static enum error write_register(mpr121_const_handle_t mpr121, enum mpr121_register reg,
                                 uint8_t data)
{
    if (!mpr121) {
        return E_NULL_POINTER;
    }

    /* Not an MPR121 register */
    if (reg > MPR121_SOFT_RESET) {
        return E_INVALID_INPUT;
    }

    /* Only 0x5E, 0x73 <-> 0x7A & 0x80 can be written to in RUN mode. */
    if (get_mode(mpr121) == MPR121_MODE_RUN &&
        !(reg == MPR121_ELECTRODE_CONFIG ||
          (reg >= MPR121_GPIO_CTRL_0 && reg <= MPR121_GPIO_TOGGLE) || reg == MPR121_SOFT_RESET)) {
        return E_INVALID_STATE;
    }

    const uint8_t buf[2] = {reg, data};

    return i2c_write(mpr121->i2c, mpr121->i2c_address, buf, 2, MPR121_I2C_TIMEOUT_MS);
}

/**
 * @brief Read an MPR121 register contents.
 *
 * @param [in] mpr121 The MPR121 to read from.
 * @param [in] reg The register to read.
 * @param [out] data Pointer to store the read register contents.
 * @param [in] size The amount of bytes to read from the register.
 * @return Error code.
 */
static enum error read_register(mpr121_const_handle_t mpr121, uint8_t reg, uint8_t *data,
                                uint8_t size)
{
    if (!mpr121) {
        return E_NULL_POINTER;
    }

    return i2c_transfer(mpr121->i2c, mpr121->i2c_address, &reg, 1, data, size,
                        MPR121_I2C_TIMEOUT_MS);
}

/**
 * @brief Set the MPR121 operating mode.
 *
 * In order to write to the registers, the MPR121 must be in the STOP mode.
 * This is when all electrodes are disabled, and no touch sensing is in
 * operation. However, the GPIO/LED registers (0x73 to 0x7A) may be written
 * to in the STOP mode.
 *
 * While in the RUN mode, output registers 0x00 to 0x2A are periodically
 * updated.
 *
 * @param [in] mpr121 The MPR121 to set the mode of.
 * @param [in] mode The mode to set the MPR121 to.
 * @return Error code.
 */
static enum error set_mode(mpr121_handle_t mpr121, enum mpr121_mode mode)
{
    enum error err = E_SUCCESS;

    if (mode == MPR121_MODE_STOP) {
        /*
         * Backup the ECR register so that the correct amount of electrodes
         * are re-enabled when transitioning to the RUN mode.
         */
        err = read_register(mpr121, MPR121_ELECTRODE_CONFIG, &mpr121->ecr, 1);

        /* Don't backup the over current fault flag */
        mpr121->ecr &= ~(MPR121_TOUCH_STATUS_H_OVCF_Msk);

        if (err == E_SUCCESS) {
            err = write_register(
                mpr121, MPR121_ELECTRODE_CONFIG,
                (MPR121_ELE_DISABLED | MPR121_ELEPROX_DISABLED | MPR121_ECR_CL_ENABLED_BVR));
        }
    }
    else if (mode == MPR121_MODE_RUN) {
        /* Restore the backed up ECR register contents. */
        err = write_register(mpr121, MPR121_ELECTRODE_CONFIG, mpr121->ecr);
    }
    else {
        return E_INVALID_INPUT;
    }

    if (err == E_SUCCESS) {
        mpr121->mode = mode;
    }

    return err;
}

/**
 * @brief Set the touch and release threshold of a single electrodes.
 *
 * Touch and release values are from 0 to 255.
 *
 * @param [in] mpr121 The MPR121 to set the thresholds of.
 * @param [in] pin The electrode to configure
 * @param [in] touch The touch threshold.
 * @param [in] release The release threshold.
 * @return Error code.
 * @retval E_NULL_POINTER Null MPR121.
 * @retval E_TIMEOUT The write timed out
 * @retval E_IO The write only partially completed.
 */
static enum error mpr121_ioctl_threshold_electrode(mpr121_const_handle_t mpr121, uint8_t electrode,
                                                   uint8_t touch, uint8_t release)
{
    /* +1 for the eleprox electrode */
    if (electrode > (MPR121_ELECTRODE_MAX_ADDRESS + 1)) {
        return E_INVALID_INPUT;
    }

    /* [+ (electrode << 1)] as (2 * electrode) for the next high and low bytes */
    enum error err = write_register(mpr121, (MPR121_TOUCH_THRESHOLD_0 + (electrode << 1)), touch);

    if (err == E_SUCCESS) {
        err = write_register(mpr121, (MPR121_RELEASE_THRESHOLD_0 + (electrode << 1)), release);
    }

    return err;
}

/**
 * @brief Set the touch and release threshold of all electrodes.
 *
 * Touch condition: (Baseline - Electrode Filtered Data) > Touch Threshold
 * Release condition: (Baseline - Electrode Filtered Data) < Release Threshold]
 *
 * Thresholds are typically within the range 4 to 16. The touch threshold is
 * several counts larger than the release threshold to provide for hysteresis
 * to prevent noise and jitter.
 *
 * @param [in] mpr121 The MPR121 to set the thresholds of.
 * @param [in] touch The touch threshold.
 * @param [in] release The release threshold.
 * @return Error code.
 * @retval E_NULL_POINTER Null MPR121.
 * @retval E_TIMEOUT The write timed out
 * @retval E_IO The write only partially completed.
 */
static enum error mpr121_ioctl_threshold(mpr121_const_handle_t mpr121, uint8_t touch,
                                         uint8_t release)
{
    enum error err = E_SUCCESS;

    for (uint8_t electrode = 0; electrode < MPR121_ELECTRODE_COUNT; electrode++) {
        err = mpr121_ioctl_threshold_electrode(mpr121, electrode, touch, release);

        if (err != E_SUCCESS) {
            break;
        }
    }

    return err;
}

static enum error mpr121_ioctl_debounce(mpr121_const_handle_t mpr121, uint8_t touch,
                                        uint8_t release)
{
    if (touch > MPR121_DEBOUNCE_MAX_VALUE || release > MPR121_DEBOUNCE_MAX_VALUE) {
        return E_INVALID_INPUT;
    }

    return write_register(mpr121, MPR121_DEBOUNCE, ((release << 4) | touch));
}

static enum error write_tasks(mpr121_const_handle_t mpr121, const struct write_task *tasks,
                              uint8_t size)
{
    enum error err = E_SUCCESS;

    for (uint8_t idx = 0; idx < size; idx++) {
        const struct write_task *task = &tasks[idx];
        err = write_register(mpr121, task->reg, task->data);

        if (err != E_SUCCESS) {
            break;
        }
    }

    return err;
}

/**
 * @brief Soft reset the MPR121
 *
 * On soft reset, all registers apart from 0x5C and 0x5D (CONFIG_1 & 2) are
 * cleared.
 *
 * @param [in] mpr121 The MPR121 to reset.
 * @return Error code.
 * @retval E_NULL_POINTER Null MPR121.
 * @retval E_TIMEOUT The soft-reset write timed out
 * @retval E_IO The soft-reset write only partially completed.
 */
enum error mpr121_ioctl_reset(mpr121_handle_t mpr121)
{
    return write_register(mpr121, MPR121_SOFT_RESET, MPR121_SOFTRESET_TRIGGER);
}

static enum error init_config(mpr121_handle_t mpr121, const struct mpr121_config *config)
{
    const struct write_task config_tasks[] = {
        {MPR121_MHD_RISING, (config->electrode_filter.rising.max_half_delta & MPR121_MHD_Msk)},
        {MPR121_NHD_RISING, (config->electrode_filter.rising.noise_half_delta & MPR121_NHD_Msk)},
        {MPR121_NCL_RISING, (config->electrode_filter.rising.noise_count_limit & MPR121_NCL_Msk)},
        {MPR121_FDL_RISING, (config->electrode_filter.rising.filter_delay_limit & MPR121_FDL_Msk)},

        {MPR121_MHD_FALLING, (config->electrode_filter.falling.max_half_delta & MPR121_MHD_Msk)},
        {MPR121_NHD_FALLING, (config->electrode_filter.falling.noise_half_delta & MPR121_NHD_Msk)},
        {MPR121_NCL_FALLING, (config->electrode_filter.falling.noise_count_limit & MPR121_NCL_Msk)},
        {MPR121_FDL_FALLING,
         (config->electrode_filter.falling.filter_delay_limit & MPR121_FDL_Msk)},

        {MPR121_NHD_TOUCHED, (config->electrode_filter.touched.noise_half_delta & MPR121_NHD_Msk)},
        {MPR121_NCL_TOUCHED, (config->electrode_filter.touched.noise_count_limit & MPR121_NCL_Msk)},
        {MPR121_FDL_TOUCHED,
         (config->electrode_filter.touched.filter_delay_limit & MPR121_FDL_Msk)},
        {MPR121_FILTER_CDC_CONFIG,
         (config->charge_discharge_current_uA & MPR121_FILTER_CDC_CONFIG_CDC_Msk) |
             (config->first_filter_samples & MPR121_FILTER_CDC_CONFIG_FFI_Msk)},
        {MPR121_FILTER_CDT_CONFIG,
         (config->sample_interval & MPR121_FILTER_CDT_CONFIG_ESI_Msk) |
             (config->second_filter_samples & MPR121_FILTER_CDT_CONFIG_SFI_Msk) |
             (config->charge_discharge_time & MPR121_FILTER_CDT_CONFIG_CDT_Msk)},
    };

    /* Reset and ensure that the handle's mode is in STOP mode */
    enum error err = mpr121_ioctl_reset(mpr121);

    if (err == E_SUCCESS) {
        err = set_mode(mpr121, MPR121_MODE_STOP);
    }

    /* Set the electrode thresholds */
    if (err == E_SUCCESS) {
        err = mpr121_ioctl_threshold(mpr121, config->touch_threshold, config->release_threshold);
    }

    if (err == E_SUCCESS) {
        err = mpr121_ioctl_debounce(mpr121, config->touch_debounce, config->release_debounce);
    }

    if (err == E_SUCCESS) {
        err = write_tasks(mpr121, config_tasks, ARRAY_SIZE(config_tasks));
    }

    if (err == E_SUCCESS && config->auto_configure) {
        const struct write_task autoconfig_tasks[] = {
            /* Enable Auto-(Re)Config, baseline value to 5MSBs */
            {MPR121_AUTO_CONFIG_0, 0x0B},

            /* Auto-config configuration for Vdd = 3.3V */
            {MPR121_UP_LIMIT, 200},     /* ((Vdd - 0.7)/Vdd) * 256 */
            {MPR121_TARGET_LIMIT, 180}, /* UPLIMIT * 0.9 */
            {MPR121_LOW_LIMIT, 130},    /* UPLIMIT * 0.65 */
        };

        err = write_tasks(mpr121, autoconfig_tasks, ARRAY_SIZE(autoconfig_tasks));
    }

    if (err == E_SUCCESS) {
        /* Preset the ECR register for the Run mode to restore. */
        mpr121->ecr = ((uint8_t)config->electrodes | (uint8_t)config->eleprox |
                       (uint8_t)config->calibration_lock);

        err = set_mode(mpr121, MPR121_MODE_RUN);
    }

    return err;
}

mpr121_handle_t mpr121_open(const struct mpr121_config *config, enum mpr121_i2c_addr addr)
{
    if (!config) {
        return NULL;
    }

    if (addr < MPR121_I2C_ADDR_0 || addr > MPR121_I2C_ADDR_3) {
        return NULL;
    }

    mpr121_handle_t mpr121 = malloc(sizeof(*mpr121));

    if (mpr121) {
        mpr121->i2c = i2c_open(&config->i2c);
        mpr121->i2c_address = addr;
    }

    /* Checks for NULL mpr121 */
    if (init_config(mpr121, config) != E_SUCCESS) {
        return NULL;
    }

    return mpr121;
}

void mpr121_close(mpr121_handle_t mpr121)
{
    if (mpr121) {
        i2c_close(mpr121->i2c);
        free(mpr121);
    }
}

static enum error read_register_u16(mpr121_const_handle_t mpr121, uint8_t reg, uint16_t *data)
{
    uint8_t tmp[2] = {};
    const enum error err = read_register(mpr121, reg, tmp, 2);

    if (err == E_SUCCESS && data) {
        *data = byte_array_to_u16(tmp);
    }

    return err;
}

enum error mpr121_read(mpr121_const_handle_t mpr121, uint16_t *status)
{
    uint16_t tmp = 0;
    const enum error err = read_register_u16(mpr121, MPR121_TOUCH_STATUS_L, &tmp);

    if (err == E_SUCCESS && status) {
        *status = (tmp & max_n_bit_value_u16(MPR121_ELECTRODE_COUNT));
    }

    /* Over-current fault bit was set */
    if ((bool)(tmp & (uint16_t)(MPR121_TOUCH_STATUS_H_OVCF_Msk << UINT8_WIDTH))) {
        return E_HARDWARE;
    }

    return err;
}

enum error mpr121_read_pin(mpr121_const_handle_t mpr121, uint8_t pin, bool *status)
{
    if (pin > MPR121_ELECTRODE_MAX_ADDRESS) {
        return E_INVALID_INPUT;
    }

    uint16_t touched_state = 0;
    const enum error err = mpr121_read(mpr121, &touched_state);

    if (status) {
        *status = (bool)(touched_state & (1 << pin));
    }

    return err;
}
