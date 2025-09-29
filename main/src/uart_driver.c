#include "uart_driver.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"

static const char *TAG = "UART_DRIVER";

void uart_init(const my_uart_config_t *config) {
    uart_config_t uart_conf = {
        .baud_rate = config->baud_rate,
        .data_bits = config->data_bits,
        .parity    = config->parity,
        .stop_bits = config->stop_bits,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB
    };

    esp_err_t ret = uart_param_config(config->port, &uart_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "uart_param_config failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = uart_set_pin(config->port, config->tx_pin, config->rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "uart_set_pin failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = uart_driver_install(config->port, 1024, 0, 0, NULL, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "uart_driver_install failed: %s", esp_err_to_name(ret));
    }
}

void uart_send(uart_port_t port, const char *data) {
    uart_write_bytes(port, data, strlen(data));
}

int uart_receive(uart_port_t port, char *buffer, uint32_t timeout_ms) {
    return uart_read_bytes(port, (uint8_t *)buffer, 1, pdMS_TO_TICKS(timeout_ms));
}
