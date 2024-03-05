#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "uart.h"
// #include <stdlib.h>

#define UART_BAUD_RATE 9600      
#define WORD_SIZE 4 // Message = 1 word
#define BUFFER_SIZE 32


int main(void)
{
    DDRB = 0x00000001;  // LED
    unsigned int c; // Recieved Char
    char buffer[BUFFER_SIZE];
    char word[WORD_SIZE];
    uint8_t k = 0;  // For position in buffer

    uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
    sei();

    for(;;)
    {
        c = uart_getc();
        if (c & UART_NO_DATA){} // no data available from UART
        else // new data available from UART
        {
            if (c & UART_FRAME_ERROR){uart_puts_P("UART Frame Error: ");} // Framing Error detected, i.e no stop bit detected
            if ( c & UART_OVERRUN_ERROR ){uart_puts_P("UART Overrun Error: ");} // Overrun Error, i.e. character already present in the UART UDR register was not read fast enough
            if ( c & UART_BUFFER_OVERFLOW ){uart_puts_P("Buffer overflow error: ");} // Overflow error, i.e. We are not reading the receive buffer fast enough

            // Recieved message
            if (c == '\r') {
              if (k >= WORD_SIZE) // Putts characters in buffer into a word with size: WORD_SIZE
              {
                for (uint8_t i = 0; i < WORD_SIZE; i++){
                  word[i] = buffer[k - i];
                }

                // Blink LED when message recieved
                PORTB = 0b00000001;
                _delay_ms(10);
                PORTB = 0b00000000;
                _delay_ms(10);
              }
            }
            else if (c != '\n') // Adds recieved character into buffer
            {
              k++;
              buffer[k] = c;
              if (k > BUFFER_SIZE)
              {
                k = 0;
              }
            }

            // Optional: Sends back recieved message
            if (c == '\r')
            {
              uart_puts("Recieved:");
              for (uint8_t i = 0; i < WORD_SIZE; i++)
              {
                uart_putc(word[WORD_SIZE - i - 1]);
              }
            }
        }
    }
}
