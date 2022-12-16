#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include <string.h>
#include <stdio.h>

// Uart peripheral defines
#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE
#define UART_TX_PIN 0
#define UART_RX_PIN 1

// Format of string that will be used
#define SAMPLE_STRING "[00,00][00.00][0]"

// Buffer size = strlen + 1 char ('\0')
const uint8_t BUFFER_SIZE = strlen(SAMPLE_STRING) + 1;

// Init the Uart buffer
char UartBuffer[] = SAMPLE_STRING;
// Counter of Uart buffer
uint8_t counter = 0;

// RX interrupt handler
void on_uart_rx()
{
    char ch = uart_getc(UART_ID);
    if ((ch != '\0') && (counter <= (BUFFER_SIZE - 1)))
    {
        UartBuffer[counter++] = ch;
    }
    else
    {
        counter = 0;
        printf("Content received from Pi 4: %s\n", UartBuffer);
        memset(UartBuffer, 0, BUFFER_SIZE);
    }
}

void uart_init_func()
{
    /* Uart Setup */
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    int __unused actual = uart_set_baudrate(UART_ID, BAUD_RATE);
    uart_set_hw_flow(UART_ID, false, false);
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);
    uart_set_fifo_enabled(UART_ID, false);

    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);
    uart_set_irq_enables(UART_ID, true, false);
}

void led_init(uint ledNumber)
{
    gpio_init(ledNumber);
    gpio_set_dir(ledNumber, GPIO_OUT);
}

int main()
{
    /* GPIO Setup */
    stdio_init_all();
    const uint LED_PIN = 25;
    led_init(LED_PIN);

    /* UART Setup */
    uart_init_func();

    printf("Application Init!\n");

    while (1)
    {
        gpio_put(LED_PIN, 0);
        sleep_ms(100);
        gpio_put(LED_PIN, 1);
        sleep_ms(100);
    }
}
