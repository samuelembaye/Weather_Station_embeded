#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"

#include "i2c_driver.h"
#include "uart_driver.h"
#include <string.h>   // ✅ Needed for strncmp, strlen, etc.

// I2C Config
#define I2C_PORT            I2C_NUM_0
#define SDA_PIN             GPIO_NUM_1
#define SCL_PIN             GPIO_NUM_2
#define CLK_SPEED_HZ        100000  // 100 kHz

// UART Config
#define UART_PORT  UART_NUM_1
#define TX_PIN     GPIO_NUM_43
#define RX_PIN     GPIO_NUM_44
#define BAUD_RATE  115200

static const char *TAG = "MAIN";

// Simple I2C scan helper
void i2c_scan(i2c_port_t port) {
    ESP_LOGI(TAG, "Scanning I2C bus...");
    for (uint8_t addr = 1; addr < 127; addr++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);
        esp_err_t err = i2c_master_cmd_begin(port, cmd, pdMS_TO_TICKS(100));
        i2c_cmd_link_delete(cmd);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Found device at address 0x%02X", addr);
        }
    }
}

// Weather periodic task
void weather_task(void *arg) {
    float temperature = 0.0f;
    float humidity = 0.0f;
    char buffer[128];

    while (1) {
        esp_err_t temp_ret = i2c_read_weather_htu21(I2C_PORT, HTU21_TEMPERATURE, &temperature);
        esp_err_t hum_ret = i2c_read_weather_htu21(I2C_PORT, HTU21_HUMIDITY, &humidity);

        if (temp_ret == ESP_OK && hum_ret == ESP_OK) {
            snprintf(buffer, sizeof(buffer),
                    "{\"sensor_id\":1,\"temperature\":%.2f,\"humidity\":%.2f}\r\n",
                    temperature, humidity);
            uart_send(UART_PORT, buffer);
            ESP_LOGI(TAG, "%s", buffer);
        } else if (temp_ret == ESP_OK) {
            snprintf(buffer, sizeof(buffer),
                    "{\"sensor_id\":1,\"temperature\":%.2f}\r\n", temperature);
            uart_send(UART_PORT, buffer);
            ESP_LOGI(TAG, "%s", buffer);
        } else if (hum_ret == ESP_OK) {
            snprintf(buffer, sizeof(buffer),
                    "{\"sensor_id\":1,\"humidity\":%.2f}\r\n", humidity);
            uart_send(UART_PORT, buffer);
            ESP_LOGI(TAG, "%s", buffer);
        } else {
            ESP_LOGE(TAG, "Failed to read temperature and humidity");
            uart_send(UART_PORT, "Failed to read temperature and humidity\r\n");
        }

        vTaskDelay(pdMS_TO_TICKS(30000));  // every 30s
    }
}

// Helper: send one-shot JSON on request
static void send_weather_data(void) {
    float temperature = 0.0f;
    float humidity = 0.0f;
    char buffer[128];

    esp_err_t temp_ret = i2c_read_weather_htu21(I2C_PORT, HTU21_TEMPERATURE, &temperature);
    esp_err_t hum_ret = i2c_read_weather_htu21(I2C_PORT, HTU21_HUMIDITY, &humidity);

    if (temp_ret == ESP_OK && hum_ret == ESP_OK) {
        snprintf(buffer, sizeof(buffer),
                 "{\"sensor_id\":1,\"temperature\":%.2f,\"humidity\":%.2f}\r\n",
                 temperature, humidity);
        uart_send(UART_PORT, buffer);
        ESP_LOGI(TAG, "Sent: %s", buffer);
    } else {
        ESP_LOGE(TAG, "Failed to read sensor");
    }
}

// ✅ UART line reader (local to main.c, uses uart_receive)
static int uart_receive_line(uart_port_t port, char *buf, size_t max_len, TickType_t timeout_ticks) {
    size_t i = 0;
    uint8_t ch;
    int len;
    TickType_t start = xTaskGetTickCount();

    while (i < (max_len - 1)) {
        len = uart_receive(port, (char *)&ch, 100 / portTICK_PERIOD_MS);
        if (len > 0) {
            buf[i++] = (char)ch;
            if (ch == '\n') break;
        }

        // timeout check
        if ((xTaskGetTickCount() - start) > timeout_ticks) break;
    }

    buf[i] = '\0';
    return i;
}

// UART command task
void uart_command_task(void *arg) {
    char rx_buffer[64];
    while (1) {
        int len = uart_receive_line(UART_PORT, rx_buffer, sizeof(rx_buffer), pdMS_TO_TICKS(60000));
        if (len > 0) {
            rx_buffer[len] = '\0';
            ESP_LOGI(TAG, "UART Received: %s", rx_buffer);

            if (strncmp(rx_buffer, "GET_DATA", 8) == 0) {
                ESP_LOGI(TAG, "Command GET_DATA received");
                send_weather_data();
            }
        }
    }
}

void app_main(void) {
    // Scan bus
    i2c_scan(I2C_PORT);

    // I2C Init
    if (i2c_master_simple_init(I2C_PORT, SDA_PIN, SCL_PIN, CLK_SPEED_HZ, true) != ESP_OK) {
        ESP_LOGE(TAG, "I2C init failed");
        return;
    }

    // UART Init
    my_uart_config_t uart_conf = {
        .port = UART_PORT,
        .tx_pin = TX_PIN,
        .rx_pin = RX_PIN,
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1
    };
    uart_init(&uart_conf);

    // Tasks
    xTaskCreate(weather_task, "weather_task", 4096, NULL, 5, NULL);
    xTaskCreate(uart_command_task, "uart_command_task", 4096, NULL, 5, NULL);
}
