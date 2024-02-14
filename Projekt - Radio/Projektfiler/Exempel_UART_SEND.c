#define F_CPU 1000000UL
#include <avr/io.h> // external library which describes our chip
#include <util/delay.h> // external library for time delay functions
#include <uart.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define BAUDRATE 1200 // Baud rate i arduino serial motirot: 1200 -> 9600 (ggr 8)

void init(void)
{
uart_init(UART_BAUD_SELECT(BAUDRATE, F_CPU ));
sei(); // allow interrupts globally
}

int main(void) 
{
  uint8_t i = 0;
  char buffer[40];

  init();

  while (1) {
      _delay_ms(1000);
      i = i + 1;

      sprintf(buffer, "Meddelande %3u\n\r", i);
      uart_puts(buffer);
  }

  return 0;
}