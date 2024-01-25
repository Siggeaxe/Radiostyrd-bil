#define F_CPU 1000000UL
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
//#include <mpu6050.h>
#include "i2cmaster.h"
#include "uart.h"

#define BAUDRATE 1200

  uint8_t M1F = 0b00000101;
  uint8_t M1B = 0b00001001;
  uint8_t M2F = 0b00010010;
  uint8_t M2B = 0b00100010;
  uint8_t MO = 0b00000011;

  uint8_t F = 0;
  uint8_t B = 0;
  uint8_t H = 0;
  uint8_t V = 0;

  float Acc_x,Acc_y,Acc_z,Temperature,Gyro_x,Gyro_y,Gyro_z;
  volatile uint8_t switch_mux = 0;


void init (void)
{
//DDRC = 0b00111111;
DDRB = 0b00001000;
DDRD = 0b01101000;
uart_init(UART_BAUD_SELECT(BAUDRATE, F_CPU));
sei();
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

_delay_ms(1000);
i2c_init();
}

// void Gyro_Init(){
//   _delay_ms(150);
//   i2c_start_wait(0xD0);
//   i2c_write(SMPLRT_DIV);
//   i2c_write(0x07);
//   i2c_stop();

//   i2c_start_wait(0xD0);
//   i2c_write(PWR_MGMT_1);
//   i2c_write(0x01);
//   i2c_stop();

//   i2c_start_wait(0xD0);
//   i2c_write(CONFIG);
//   i2c_write(0x00);
//   i2c_stop();

//   i2c_start_wait(0xD0);
//   i2c_write(GYRO_CONFIG);
//   i2c_write(0x18);
//   i2c_stop();

//   i2c_start_wait(0xD0);
//   i2c_write(INT_ENABLE);
//   i2c_write(0x01);
//   i2c_stop();
// }

// void MPU_Start_Loc(){
//   i2c_start_wait(0xD0);
//   i2c_write(ACCEL_XOUT_H);
//   i2c_rep_start(0xD1);
// }

// void Read_RawValue(){
//   MPU_Start_Loc();
//   Acc_x = (((int)i2c_readAck()<<8) | (int)i2c_readAck());
//   Acc_y = (((int)i2c_readAck()<<8) | (int)i2c_readAck());
//   Acc_z = (((int)i2c_readAck()<<8) | (int)i2c_readAck());
//   Temperature = (((int)i2c_readAck()<<8) | (int)i2c_readAck());
//   Gyro_x = (((int)i2c_readAck()<<8) | (int)i2c_readAck());
//   Gyro_y = (((int)i2c_readAck()<<8) | (int)i2c_readAck());
//   Gyro_z= (((int)i2c_readAck()<<8) | (int)i2c_readNak());
//   i2c_stop();
// }

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
  //Gyro_Init();
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

    OCR2A = (H+F)/2;
    OCR2B = (V+F)/2;
  }
}