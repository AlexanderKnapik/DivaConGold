#include "i2c.h"

#include "common/bsp.h"
#include "common/error.h"

#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include <pico/error.h>

#include <stddef.h>
#include <stdint.h>

i2c_inst_t *i2c_open(const struct i2c_config *config)
{
    if (!config) {
        return NULL;
    }

    /*
     * If the I2C instance is already initialised, don't attempt to
     * re-initialise it. Just return the instance instead.
     *
     * I'm pretty sure this causes issues with the MPR121 I2C bus getting
     * stuck on boot if initialised multiple times.
     */
    if (!config->instance->hw->enable) {
        i2c_init(config->instance, config->speed_hz);

        gpio_set_function(config->sda_pin, GPIO_FUNC_I2C);
        gpio_set_drive_strength(config->sda_pin, config->drive);
        gpio_pull_up(config->sda_pin);
        gpio_set_slew_rate(config->sda_pin, config->slew);

        gpio_set_function(config->scl_pin, GPIO_FUNC_I2C);
        gpio_pull_up(config->scl_pin);
        gpio_set_slew_rate(config->scl_pin, config->slew);
        gpio_set_drive_strength(config->scl_pin, config->drive);
    }

    return config->instance;
}

void i2c_close(i2c_inst_t *i2c)
{
    /* I2C instance already closed, exit early. */
    i2c_deinit(i2c);
}

enum error get_error_code(enum pico_error_codes error, uint16_t size)
{
    if (error == size) {
        return E_SUCCESS;
    }

    if (error == PICO_ERROR_TIMEOUT) {
        return E_TIMEOUT;
    }

    return E_IO;
}

static enum error internal_write(i2c_inst_t *i2c, uint8_t address, const uint8_t *data,
                                 uint16_t size, bool no_stop, uint16_t timeout_ms)
{
    if (!i2c || !data) {
        return E_NULL_POINTER;
    }

    if (size == 0) {
        return E_INVALID_INPUT;
    }

    const enum error err =
        i2c_write_timeout_us(i2c, address, data, size, no_stop, (uint)(timeout_ms * 1000));

    return get_error_code((enum pico_error_codes)err, size);
}

enum error i2c_write(i2c_inst_t *i2c, uint8_t address, const uint8_t *data, uint16_t size,
                     uint16_t timeout_ms)
{
    return internal_write(i2c, address, data, size, false, timeout_ms);
}

enum error i2c_read(i2c_inst_t *i2c, uint8_t address, uint8_t *data, uint16_t size,
                    uint16_t timeout_ms)
{
    if (!i2c || !data) {
        return E_NULL_POINTER;
    }

    if (size == 0) {
        return E_INVALID_INPUT;
    }

    /* Pico i2c_write functions handles the XOR for the read bit */
    const enum error err =
        i2c_read_timeout_us(i2c, address, data, size, false, (uint)(timeout_ms * 1000));

    return get_error_code((enum pico_error_codes)err, size);
}

enum error i2c_transfer(i2c_inst_t *i2c, uint8_t address, const uint8_t *src, uint16_t src_size,
                        uint8_t *dest, uint16_t dest_size, uint16_t timeout_ms)
{
    /* Write the register to be read. function call handles the read bit XOR */
    enum error err = internal_write(i2c, address, src, src_size, true, timeout_ms);

    if (err == E_SUCCESS) {
        err = i2c_read(i2c, address, dest, dest_size, timeout_ms);
    }

    return err;
}