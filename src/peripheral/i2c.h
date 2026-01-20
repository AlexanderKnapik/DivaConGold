/*****************************************************************************/
/*                   I2C Wrapper and Common Setup Functions                  */
/*****************************************************************************/
#ifndef I2C_H_
#define I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common/error.h"

#include <stdint.h>

/*
 * Re-typedef the struct to make it an opaque type so that the pico
 * "hardware/i2c.h" doesn't have to be included as well in order to get the
 * type definition.
 */
typedef struct i2c_inst i2c_inst_t;

struct i2c_config {
    i2c_inst_t *instance;
    uint32_t speed_hz;
    uint8_t sda_pin;
    uint8_t scl_pin;
};

/**
 * @brief Initialise the I2C interface and GPIO pins.
 *
 * @param [in] config The I2C initialisation configuration.
 * @return The initialised I2C instance.
 * @retval NULL I2C instance could not be initialised.
 */
i2c_inst_t *i2c_open(const struct i2c_config *config);

/**
 * @brief De-initialise the I2C interface.
 *
 * Does not de-initialise the GPIO. If the pins are shared with other
 * interfaces, it is the responsibility of the other interface to
 * initialise the GPIO pins as necessary.
 *
 * @param [in] i2c The I2C instance to de-initialise.
 * @return Error code.
 * @retval E_SUCCESS Success.
 * @retval E_NULL_POINTER Null i2c.
 */
enum error i2c_close(i2c_inst_t *i2c);

/**
 * @brief Write data to the I2C instance with a timeout.
 *
 * Essentially a wrapper function for standardised error codes. See
 * i2c_write_timeout_us() for more details.
 *
 * Writes in blocking mode.
 *
 * @param [in] i2c The I2C instance to write to.
 * @param [in] address The I2C address to write to.
 * @param [in] data The data to write.
 * @param [in] size The size of the data to write.
 * @param [in] timeout_ms The timeout period in milliseconds.
 * @return Error code.
 * @retval E_SUCCESS Success.
 * @retval E_IO The write was only partially successful.
 * @retval E_INVALID_INPUT Invalid zero size.
 * @retval E_NULL_POINTER Null i2c or data.
 * @retval E_TIMEOUT The write timed out.
 */
enum error i2c_write(i2c_inst_t *i2c, uint8_t address, const uint8_t *data, uint16_t size,
                     uint16_t timeout_ms);

/**
 * @brief Read data from the I2C instance with a timeout.
 *
 * Essentially a wrapper function for standardised error codes. See
 * i2c_read_timeout_us() for more details.
 *
 * Reads in blocking mode.
 *
 * @param [in] i2c The I2C instance to read from.
 * @param [in] address The I2C address to read from.
 * @param [out] data Pointer to store the read data.
 * @param [in] size The amount of bytes to read.
 * @param [in] timeout_ms The timeout period in milliseconds.
 * @return Error code.
 * @retval E_SUCCESS Success.
 * @retval E_IO The read was only partially successful.
 * @retval E_INVALID_INPUT Invalid zero size.
 * @retval E_NULL_POINTER Null i2c or data.
 * @retval E_TIMEOUT The read timed out.
 */
enum error i2c_read(i2c_inst_t *i2c, uint8_t address, uint8_t *data, uint16_t size,
                    uint16_t timeout_ms);

/**
 * @brief Write data to an I2C instance and read back the response.
 *
 * Reads and writes in blocking mode. Uses the same timeout period
 * for both reading and writing, essentially being a maximum 2*timeout_ms.
 *
 * @param [in] i2c The I2C instance to transfer with.
 * @param [in] address The I2C address to transfer with.
 * @param [in] src The data to write.
 * @param [in] src_size The amount of bytes to write.
 * @param [out] dest Pointer to store the read data.
 * @param [in] dest_size The amount of bytes to read.
 * @param [in] timeout_ms The timeout period in milliseconds.
 * @return Error code.
 * @retval E_SUCCESS Success.
 * @retval E_IO The read was only partially successful.
 * @retval E_INVALID_INPUT Invalid zero size.
 * @retval E_NULL_POINTER Null i2c or data.
 * @retval E_TIMEOUT The read timed out.
 */
enum error i2c_transfer(i2c_inst_t *i2c, uint8_t address, const uint8_t *src, uint16_t src_size,
                        uint8_t *dest, uint16_t dest_size, uint16_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif // I2C_H_
