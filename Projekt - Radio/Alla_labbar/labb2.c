#include <avr/io.h> // external library which describes our chip
#include <util/delay.h> // external library for time delay functions

int main (void)
{
  DDRB = 0x00000011;
  while (1) // infinite main loop
  {
    PORTB = 0b00000001;
    _delay_ms(500); // wait 1/2 second
    PORTB = 0b00000010;
    _delay_ms(500); // wait 1/2 second
  }
}