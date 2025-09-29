#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include "driver/uart.h"
#include "driver/gpio.h"

typedef struct {
    uart_port_t port;
    gpio_num_t tx_pin;
    gpio_num_t rx_pin;
    int baud_rate;
    uart_word_length_t data_bits;
    uart_parity_t parity;
    uart_stop_bits_t stop_bits;
} my_uart_config_t;

void uart_init(const my_uart_config_t *config);
void uart_send(uart_port_t port, const char *data);
int uart_receive(uart_port_t port, char *buffer, uint32_t timeout_ms);

#endif