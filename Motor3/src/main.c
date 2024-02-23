#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "i2cmaster.h"
#include "twimaster.c"
#include <mpu.inc>
#include "uart.c"
#include "uart.h"

#define AVG 8
#define UART_BAUD_RATE 9600      

volatile float x_angle=0,y_angle=0;
volatile int32_t accelX=0,accelY=0,accelZ=0;
volatile int16_t Ax=0, Ay=0, Az=0;
volatile uint8_t k =0;

void init(void){
  //PB 1,2,4,5 - framåt ports
  //PC 1,2,4,5 - Bakåt ports, Xor med forward för att altid ha en high
  //PD 3,5,6 & PB 3 - PWM portar till Enable pin på drivarkretsen
  DDRB = 0b00111110;
  DDRD = 0b01101000;
  DDRC = 0b00110110;

  // setting up TIMER0
  TCCR0A = (1 << COM0A1 ) | (0 << COM0A0 ) | (1 << COM0B1 ) | (0 << COM0B0 )
            | (1 << WGM01 ) | (1 << WGM00 );
  TCCR0B = (0 << WGM02 ) | (0 << CS02) | (1 << CS01) | (1 << CS00 );

  TCCR2A = (1 << COM2A1 ) | (0 << COM2A0 ) | (1 << COM2B1 ) | (0 << COM2B0 )
            | (1 << WGM21 ) | (1 << WGM20 );
  TCCR2B = (0 << WGM22 ) | (0 << CS22) | (1 << CS21) | (1 << CS20 );

  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
  sei();
}

int main(void){
  init();
  OCR0A = 0;
  OCR0B = 0;
  OCR2A = 0;
  OCR2B = 0;
  uint8_t forward = 0b00110110;
  uint8_t motor_right = 0b00110000;
  uint8_t motor_left = 0b00000110;
  uint8_t right = 0;
  uint8_t left = 0;
  uint8_t direction = 0;
  uint16_t timer = 0;
  unsigned int c;

  while(1){
    c = uart_getc();

    if (c & UART_NO_DATA){  // no data available from UART (lower byte: received byte from ringbuffer, 
                            // higher byte: last receive status. Om du inte fattar holla på Peter Fleury:s UART dokummentation!!!)
      if (timer >= 500){
        right = 0;
        left = 0;
        direction = 0;
      }
      else{timer++;}
      _delay_ms(1);      
    }
    else{
      timer = 0;
      switch(c & 0b11000000){
        case 0b01000000:
          right = c<<2;
          break;
        case 0b10000000:
          left = c<<2;
          break;
        case 0b11000000:
          direction = c & 0b00000111;
          break;
      }
    }

    if (direction & 0x01){
      PORTB = forward;
      PORTC = 0x00;
    }
    else if (direction & 0x02){
      PORTB = motor_right; 
      PORTC = forward^motor_right;
    }
    else if(direction & 0x04){
      PORTB = motor_left;
      PORTC = forward^motor_left;
    }
    else{
      PORTB = 0x00;
      PORTC = forward;     
    }

    OCR0A = right;
    OCR0B = right;

    OCR2A = left;
    OCR2B = left;

  }
}