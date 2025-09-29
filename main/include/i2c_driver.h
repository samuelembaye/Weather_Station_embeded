#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include "driver/i2c.h"
#include "esp_err.h"

typedef enum {
    HTU21_TEMPERATURE,
    HTU21_HUMIDITY
} weather;

/**
 * @brief Simplified I2C initialization
 *
 * This function wraps the ESP-IDF I2C master configuration using basic parameters.
 *
 * @param port         I2C port number (e.g., I2C_NUM_0)
 * @param sda_pin      GPIO number for SDA
 * @param scl_pin      GPIO number for SCL
 * @param clk_speed_hz Clock speed in Hz (e.g., 100000)
 * @param pullup_en    true to enable internal pull-ups on SDA/SCL
 *
 * @return ESP_OK on success, or ESP_FAIL on error
 */
esp_err_t i2c_master_simple_init(i2c_port_t port, gpio_num_t sda_pin, gpio_num_t scl_pin,
                                 uint32_t clk_speed_hz, bool pullup_en);

esp_err_t i2c_write(i2c_port_t port, uint8_t addr, const uint8_t *data, size_t len);
esp_err_t i2c_read(i2c_port_t port, uint8_t addr, uint8_t *buffer, size_t len);


/**
 * @brief Read temperature or humidity from HTU21 sensor
 *
 * @param port     I2C port to use (e.g., I2C_NUM_0)
 * @param weather  Type of weather data to read: temperature or humidity
 * @param result   Output pointer to float for the measurement value
 *
 * @return ESP_OK on success, otherwise error code
 */
esp_err_t i2c_read_weather_htu21(i2c_port_t port, weather weather, float *result);




#endif // I2C_DRIVER_H
