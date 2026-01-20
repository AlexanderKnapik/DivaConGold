#include "i2c.h"

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

    i2c_init(config->instance, config->speed_hz);
    gpio_set_function(config->sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(config->scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(config->sda_pin);
    gpio_pull_up(config->scl_pin);

    return config->instance;
}

enum error i2c_close(i2c_inst_t *i2c)
{
    if (!i2c) {
        return E_NULL_POINTER;
    }

    i2c_deinit(i2c);

    return E_SUCCESS;
}

enum error get_error_code(int val, uint16_t size)
{
    if (val == size) {
        return E_SUCCESS;
    }

    if (val == PICO_ERROR_TIMEOUT) {
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

    return get_error_code(err, size);
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

    return get_error_code(err, size);
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