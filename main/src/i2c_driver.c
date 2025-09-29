#include "i2c_driver.h"
#include "esp_log.h"
#include "i2c_driver.h"
#include "esp_log.h"

#define HTU21_ADDR    0x40
#define CMD_READ_TEMP     0xE3
#define CMD_READ_HUMIDITY 0xE5

static const char *TAG = "I2C_DRIVER";

esp_err_t i2c_master_simple_init(i2c_port_t port, gpio_num_t sda_pin, gpio_num_t scl_pin,
                                 uint32_t clk_speed_hz, bool pullup_en) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_pin,
        .scl_io_num = scl_pin,
        .sda_pullup_en = pullup_en,
        .scl_pullup_en = pullup_en,
        .master.clk_speed = clk_speed_hz,
        .clk_flags = 0
    };

    esp_err_t ret = i2c_param_config(port, &conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Param config failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = i2c_driver_install(port, conf.mode, 0, 0, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Driver install failed: %s", esp_err_to_name(ret));
    }

    return ret;
}

esp_err_t i2c_write(i2c_port_t port, uint8_t addr, const uint8_t *data, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    if (!cmd) return ESP_FAIL;

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, data, len, true);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(port, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Write failed: %s", esp_err_to_name(ret));
    }

    return ret;
}

esp_err_t i2c_read(i2c_port_t port, uint8_t addr, uint8_t *buffer, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    if (!cmd) return ESP_FAIL;

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_READ, true);
    if (len > 1) {
        i2c_master_read(cmd, buffer, len - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, buffer + len - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(port, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Read failed: %s", esp_err_to_name(ret));
    }

    return ret;
}

esp_err_t i2c_read_weather_htu21(i2c_port_t port, weather weather_type, float *result) {
    uint8_t cmd;

    switch (weather_type) {
        case HTU21_TEMPERATURE:
            cmd = CMD_READ_TEMP;
            break;
        case HTU21_HUMIDITY:
            cmd = CMD_READ_HUMIDITY;
            break;
        default:
            ESP_LOGE(TAG, "Invalid weather type");
            return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = i2c_write(port, HTU21_ADDR, &cmd, 1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send measurement command");
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(50)); // Wait for measurement

    uint8_t data[3] = {0};
    ret = i2c_read(port, HTU21_ADDR, data, 3);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read measurement data");
        return ret;
    }

    uint16_t raw = ((uint16_t)data[0] << 8) | (data[1] & 0xFC);

    if (weather_type == HTU21_TEMPERATURE) {
        *result = -46.85 + (175.72 * raw / 65536.0);
    } else {
        *result = -6.0 + (125.0 * raw / 65536.0);
    }

    return ESP_OK;
}

