#define F_CPU 1000000UL // compiler needs to know the clock frequency

#include <avr/io.h> // external library which describes our chip
#include <util/delay.h> // external library for time delay functions

int main (void)
{
  uint8_t magic[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f}; // 7-segment

  DDRB = 0b11111111; // all B-pins are output

  while (1) // infinite main loop
  {
    PORTB = ~magic[1];
  }  
}
