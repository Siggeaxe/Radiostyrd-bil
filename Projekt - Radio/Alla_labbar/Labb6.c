# define F_CPU 1000000UL

# include <avr/io.h>
# include <util/delay.h>
# include <stdio.h>
# include <avr/interrupt.h>
# include "lcd.h"
# include "uart.h"
# define BAUDRATE 1200

void init(void)
{
    lcd_init( LCD_DISP_ON );
    lcd_clrscr();

    uart_init(UART_BAUD_SELECT(BAUDRATE, F_CPU));

    PORTD |= (1 << PD2 ); // pull -up for switch 1

    sei (); // allow interrupts globally
}

int main(void)
{
    uint8_t i=0;
    uint16_t c;
    char buffer [40];

    init();

    while (1)
    {
        while (PIND & 0b00000100 ); // wait for S1
        i = i + 1;

        sprintf (buffer , " message %3u", i);
        lcd_gotoxy (0 ,0);
        lcd_puts ( buffer );
        uart_puts ( buffer );

        _delay_ms (50); // some time to get 1st char out

        lcd_gotoxy (0 ,1);
        do
        {
            c = uart_getc (); // fetch a char from the queue
            if (!(c & UART_NO_DATA ))
            {
            lcd_putc(c & 0xff );
            }
        } while (!(c & UART_NO_DATA ));
    }
}