#include <stdio.h>
#include <esp_log.h>
#include <driver/uart.h>
#include <string.h>

#define TXD_PIN 1
#define RXD_PIN 3

const int RX_BUF_SIZE = 1024; // Buffer Size

void app_main(void)
{
  uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
  };
  uart_param_config(UART_NUM_0, &uart_config); // Passing UART port and config
  uart_set_pin(UART_NUM_0, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE); // Set UART pins (TX, RX, RTS, CTS)
  uart_driver_install(UART_NUM_0, RX_BUF_SIZE, 0, 0, NULL, 0); // UART port, Rx Buf size, queue size, UART queue, flags

  while(1)
  {
    char message[] = "ping from two";
    printf("Sending: %s\n", message);
    uart_write_bytes(UART_NUM_0, message, sizeof(message));

    char incoming_message[RX_BUF_SIZE];
    memset(incoming_message, 0, sizeof(incoming_message));
    uart_read_bytes(UART_NUM_0, (uint8_t *) incoming_message, RX_BUF_SIZE, pdMS_TO_TICKS(500)); // UART port, pointer to buffer, data length, tick to wait
    printf("Received %s\n", incoming_message);
  }
}