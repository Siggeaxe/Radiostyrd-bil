#define F_CPU 1000000UL
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/atomic.h>
#include "i2cmaster.h"
#include <mpu.inc>
#include "nokia5110.h"
#include "nokia5110.c"

#define AVG 8
uint8_t Forward = 0;
uint8_t Backward = 0;
uint8_t Hoger = 0;
uint8_t Vanster = 0;

  volatile int32_t Acc_x=0,Acc_y=0,Acc_z=0,Temp=0,Gyro_x=0,Gyro_y=0,Gyro_z=0;
  volatile uint8_t switch_mux_joystick = 0;

void init (void)
{
//DDRC = 0b00111111;
DDRB = 0b00001000;
DDRD = 0b01101111;
DDRC = 0b00001100;
// 0-1,2EN; 1-3,4EN; 2-1A; 3-2A; 4-3A; 5-4A;
// setting up TIMER0
TCCR0A = (1 << COM0A1 ) | (0 << COM0A0 ) | (1 << COM0B1 ) | (0 << COM0B0 )
          | (1 << WGM01 ) | (1 << WGM00 );
TCCR0B = (0 << WGM02 ) | (0 << CS02) | (0 << CS01) | (1 << CS00 );

TCCR1A = (0 << COM1A1) | (0 << COM1A0) | (0 << COM1B1) | (0 << COM1B0)
      | (0 << WGM11)  | (1 << WGM10);
TCCR0B = (0 << WGM13) | (0 << WGM12)
      | (0 << CS02)   | (0 << CS01)   | (1 << CS00);
TIMSK1 = (0 << OCIE0B) | (0 << OCIE0A) | (1 << TOIE1);

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

sei();
NOKIA_init(0);
}

void ADC_X(){
  ADMUX = ( 0 << REFS1 ) | ( 1 << REFS0 )
        | ( 1 << ADLAR )
        | ( 0 << MUX3 ) | ( 0 << MUX2 )
        | ( 0 << MUX1 ) | ( 1 << MUX0 );
}

void ADC_Y(){
  ADMUX = ( 0 << REFS1 ) | ( 1 << REFS0 )
        | ( 1 << ADLAR )
        | ( 0 << MUX3 ) | ( 0 << MUX2 )
        | ( 0 << MUX1 ) | ( 0 << MUX0 );
}

int main(void)
{
  init();
  char buffer[40];
  uint8_t i=0;
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
      Forward = x_axis;
      Backward = 0;
    }
    else{
      Forward = 0;
      Backward = -x_axis;
    }
    if (y_axis>=0)
    {
      Hoger = y_axis;
      Vanster = 0;
    }
    else{
      Hoger = 0;
      Vanster = -y_axis;
    }

    OCR0A = (Hoger+Backward)/2;
    OCR0B = (Vanster+Backward)/2;

    OCR2A = (Hoger+Forward)/2;
    OCR2B = (Vanster+Forward)/2;

    sprintf(buffer, "VOP 0x%02x", i);
    NOKIA_setVop(i); // set contrast voltage
    NOKIA_print(0, 0, buffer, 0); // print message in Nokia framebuffer
    NOKIA_update(); // transfer framebuffer to the Nokia display
    i++;
    _delay_ms(10);
  }
}