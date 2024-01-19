#define F_CPU 1000000UL
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

void init()
{
  DDRD  = 0b11110000;
}

uint8_t steps[8] = 
{
  0b00010000,
  0b00110000,
  0b00100000,
  0b01100000,
  0b01000000,
  0b11000000,
  0b10000000,
  0b10010000
  };

int main(void)
{
  uint8_t i=0;
  init();
  
  while (1)
  {
    PORTD = steps[i];
    _delay_ms(1);
    i++;
    i%=8;
  }    
}