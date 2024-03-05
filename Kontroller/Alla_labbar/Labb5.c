#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

uint8_t font[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
volatile uint8_t framebuffer[4];     // use to buffer contents of display

ISR(TIMER0_OVF_vect) // the code for the interrupt service routine
{
  static uint8_t current_digit = 0;
  PORTD = 0xff;                  // all segments off
  current_digit += 1;            // step one digit
  current_digit %= 4;            //    and keep between 0-3
  PORTB = (0b00010000 << current_digit);
  PORTD = framebuffer[current_digit];
}

void display(uint16_t value)  // print a 4-digit number
{
  uint16_t A = value;
  uint8_t  i;
  for (i = 0; i < 4; i++)
  {
    framebuffer[3-i] = ~font[A % 10];
    A = A / 10;
  }
}

void init(void)
{
  DDRB = 0b11110000;    // upper B-pins as output
  DDRD = 0b11111111;    // all D-pins as output
  
  ADMUX  = ( 0 << REFS1 ) | ( 1 << REFS0 )
         | ( 0 << ADLAR )
         | ( 0 << MUX3 )  | ( 1 << MUX2 )
         | ( 0 << MUX1 )  | ( 1 << MUX0 );
  ADCSRA = ( 1 << ADEN )  | ( 1 << ADSC )
         | ( 0 << ADATE ) | ( 0 << ADIF )  | ( 0 << ADIE )
         | ( 0 << ADPS2 ) | ( 1 << ADPS1 ) | ( 0 << ADPS0 );  

  // setting up TIMER0
  TCCR0A = (0 << COM0A1) | (0 << COM0A0) | (0 << COM0B1) | (0 << COM0B0)
         | (0 << WGM01)  | (0 << WGM00);
  TCCR0B = (0 << WGM02)  | (0 << CS02)   | (1 << CS01)   | (0 << CS00);
  TIMSK0 = (0 << OCIE0B) | (0 << OCIE0A) | (1 << TOIE0);
  
  // setting up TIMER1
  TCCR1A = ( 1 << COM1A1 ) | ( 0 << COM1A0 )
         | ( 0 << COM1B1 ) | ( 0 << COM1B0 )
         | ( 0 << WGM11 )  | ( 1 << WGM10 );
  TCCR1B = ( 0 << WGM13 )  | ( 1 << WGM12 )
         | ( 0 << CS12 )   | ( 0 << CS11 )   | ( 1 << CS10 );
  TIMSK1 = 0;
  OCR1A  = 0;
  
  sei();  // don't forget to enable interrupts
}

int main(void)
{
  init();
  while (1)
  {
    OCR1A ++;
    OCR1A = OCR1A & 0x00ff;  // keep only the lower 8 bits
    display(OCR1A);
    _delay_ms(20);
  }
}
