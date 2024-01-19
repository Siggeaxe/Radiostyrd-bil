# define F_CPU 1000000UL
# include <avr/io.h>
# include <stdio.h>
# include <util/delay.h>
# include <avr/interrupt.h>

  uint8_t M1F = 0b00000101;
  uint8_t M1B = 0b00001001;
  uint8_t M2F = 0b00010010;
  uint8_t M2B = 0b00100010;
  uint8_t MO = 0b00000011;

ISR(TIMER0_OVF_vect){
  //Byt mellan av och på för motorn
}

void init ()
{
DDRC = 0b00111111 ;
// 0-1,2EN; 1-3,4EN; 2-1A; 3-2A; 4-3A; 5-4A;
// setting up TIMER0
TCCR0A = (1 << COM0A1 ) | (1 << COM0A0 ) | (0 << COM0B1 ) | (0 << COM0B0 )
          | (1 << WGM01 ) | (1 << WGM00 );
TCCR0B = (1 << WGM02 ) | (0 << CS02) | (0 << CS01) | (1 << CS00 );
TIMSK0 = (0 << OCIE0B ) | (0 << OCIE0A ) | (1 << TOIE0 );
sei ();
}

int main(void)
{
  init();

  while(1)
  {
    OCR0A = 0xff;

    // PORTC = M1F;
    // _delay_ms(2000);
    // PORTC = M1B;
    // _delay_ms(2000);
    // PORTC = M2F;
    // _delay_ms(2000);
    // PORTC = M2B;
    // _delay_ms(2000);
    // PORTC = (M1F | M2F);
    // _delay_ms(5000);
  }
}