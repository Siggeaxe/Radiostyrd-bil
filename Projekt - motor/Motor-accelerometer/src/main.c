#define F_CPU 1000000UL
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "MPU6050_res_define.h"							/* Include MPU6050 register define file */
#include "I2C_Master_H_file.h"	
//#include <mpu6050.h>
//#include "i2cmaster.h"

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

}

void MPU6050_Init()										/* Gyro initialization function */
{
	_delay_ms(150);										/* Power up time >100ms */
	I2C_Start_Wait(0xD0);								/* Start with device write address */
	I2C_Write(SMPLRT_DIV);								/* Write to sample rate register */
	I2C_Write(0x07);									/* 1KHz sample rate */
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(PWR_MGMT_1);								/* Write to power management register */
	I2C_Write(0x01);									/* X axis gyroscope reference frequency */
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(CONFIG);									/* Write to Configuration register */
	I2C_Write(0x00);									/* Fs = 8KHz */
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(GYRO_CONFIG);								/* Write to Gyro configuration register */
	I2C_Write(0x18);									/* Full scale range +/- 2000 degree/C */
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(INT_ENABLE);								/* Write to interrupt enable register */
	I2C_Write(0x01);
	I2C_Stop();
}

void MPU_Start_Loc()
{
	I2C_Start_Wait(0xD0);								/* I2C start with device write address */
	I2C_Write(ACCEL_XOUT_H);							/* Write start location address from where to read */ 
	I2C_Repeated_Start(0xD1);							/* I2C start with device read address */
}

void Read_RawValue()
{
	MPU_Start_Loc();									/* Read Gyro values */
	Acc_x = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Acc_y = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Acc_z = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Temperature = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Gyro_x = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Gyro_y = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Gyro_z = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Nack());
	I2C_Stop();
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
  I2C_Init();											/* Initialize I2C */
	MPU6050_Init();	
  float Xa,Ya,Za,t;
	float Xg=0,Yg=0,Zg=0;
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

    Read_RawValue();

		Xa = Acc_x/16384.0;								/* Divide raw value by sensitivity scale factor to get real values */
		Ya = Acc_y/16384.0;
		Za = Acc_z/16384.0;
		
		Xg = Gyro_x/16.4;
		Yg = Gyro_y/16.4;
		Zg = Gyro_z/16.4;
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