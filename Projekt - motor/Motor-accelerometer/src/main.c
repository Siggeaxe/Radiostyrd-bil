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

  uint8_t F = 0;
  uint8_t B = 0;
  uint8_t H = 0;
  uint8_t V = 0;

  volatile uint8_t switch_mux = 0;


void init (void)
{
//DDRC = 0b00111111;
DDRB = 0b00001000;
DDRD = 0b01101000;
// 0-1,2EN; 1-3,4EN; 2-1A; 3-2A; 4-3A; 5-4A;
// setting up TIMER0
TCCR0A = (1 << COM0A1 ) | (0 << COM0A0 ) | (1 << COM0B1 ) | (0 << COM0B0 )
          | (1 << WGM01 ) | (1 << WGM00 );
TCCR0B = (0 << WGM02 ) | (0 << CS02) | (0 << CS01) | (1 << CS00 );

TCCR2A = (1 << COM2A1 ) | (0 << COM2A0 ) | (1 << COM2B1 ) | (0 << COM2B0 )
          | (1 << WGM21 ) | (1 << WGM20 );
TCCR2B = (0 << WGM22 ) | (0 << CS22) | (0 << CS21) | (1 << CS20 );

ADMUX = ( 0 << REFS1 ) | ( 1 << REFS0 )
        | ( 1 << ADLAR )
        | ( 0 << MUX3 ) | ( 1 << MUX2 )
        | ( 0 << MUX1 ) | ( 0 << MUX0 );
ADCSRA = ( 1 << ADEN ) | ( 1 << ADSC )
        | ( 0 << ADATE ) | ( 0 << ADIF ) | ( 0 << ADIE )
        | ( 0 << ADPS2 ) | ( 1 << ADPS1 ) | ( 1 << ADPS0 );

//sei();
}

void ADC_X(){
  ADMUX = ( 0 << REFS1 ) | ( 1 << REFS0 )
        | ( 1 << ADLAR )
        | ( 0 << MUX3 ) | ( 1 << MUX2 )
        | ( 0 << MUX1 ) | ( 1 << MUX0 );
}

void ADC_Y(){
  ADMUX = ( 0 << REFS1 ) | ( 1 << REFS0 )
        | ( 1 << ADLAR )
        | ( 0 << MUX3 ) | ( 1 << MUX2 )
        | ( 0 << MUX1 ) | ( 0 << MUX0 );
}

int main(void)
{
  init();
  OCR0A = 0;
  OCR0B = 0;
  OCR2A = 0;
  OCR2B = 0;

  while(1)
  {
    ADC_X();
    ADCSRA = ADCSRA | (1 << ADSC );
    while ( ADCSRA & (1 << ADSC) ) { }
    int16_t x_axis = 2*ADCH - 255;
    _delay_ms(1);

    ADC_Y();
    ADCSRA = ADCSRA | (1 << ADSC );
    while ( ADCSRA & (1 << ADSC) ) { }
    int16_t y_axis = 2*ADCH - 255;
    _delay_ms(1);

    if (x_axis>=0)
    {
      F = x_axis;
      B = 0;
    }
    else{
      F = 0;
      B = -x_axis;
    }
    if (y_axis>=0)
    {
      H = y_axis;
      V = 0;
    }
    else{
      H = 0;
      V = -y_axis;
    }

    OCR0A = (H+B)/2;
    OCR0B = (V+B)/2;

    OCR2A = (F+H)/2;
    OCR2B = (F+V)/2;
    //_delay_ms(1000);
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