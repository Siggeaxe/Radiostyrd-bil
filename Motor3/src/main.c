#define F_CPU 1000000UL
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "i2cmaster.h"
#include "twimaster.c"
#include <mpu.inc>

#define AVG 8
volatile float x_angle=0,y_angle=0;
volatile int32_t accelX=0,accelY=0,accelZ=0;//,Temp=0,gyroX=0,gyroY=0,gyroZ=0;
volatile int16_t Ax=0, Ay=0, Az=0;
//volatile uint8_t x_1=0, y_1=0;

void get_data(void){
  //Temp   = (MPU6050_readSensor16(MPU6050_TEMP_OUT_L,  MPU6050_TEMP_OUT_H) + (AVG-1)*Temp)/AVG;
  //gyroX  = (MPU6050_readSensor16(MPU6050_GYRO_XOUT_L, MPU6050_GYRO_XOUT_H) + (AVG-1)*gyroX)/AVG;
  //gyroY  = (MPU6050_readSensor16(MPU6050_GYRO_YOUT_L, MPU6050_GYRO_YOUT_H) + (AVG-1)*gyroY)/AVG;
  //gyroZ  = (MPU6050_readSensor16(MPU6050_GYRO_ZOUT_L, MPU6050_GYRO_ZOUT_H) + (AVG-1)*gyroZ)/AVG;
  accelX = (MPU6050_readSensor16(MPU6050_ACCEL_XOUT_L,MPU6050_ACCEL_XOUT_H) + (AVG-1)*accelX)/AVG;
  accelY = (MPU6050_readSensor16(MPU6050_ACCEL_YOUT_L,MPU6050_ACCEL_YOUT_H) + (AVG-1)*accelY)/AVG;
  accelZ = (MPU6050_readSensor16(MPU6050_ACCEL_ZOUT_L,MPU6050_ACCEL_ZOUT_H) + (AVG-1)*accelZ)/AVG;
}

ISR(TIMER1_OVF_vect){
  //i2c_stop();
  PORTC = PORTC ^ 0b00000001;
}

void init(void){
  //PD 0,1,2,4 - framåt ports
  //PB 0,1,2,4 - Bakåt ports, Xor med forward för att altid ha en high
  //PD 3,5,6 & PB 3 - PWM portar till Enable pin på drivarkretsen
DDRB = 0b00011111;
DDRD = 0b01111111;
DDRC = 0b00000001;

// setting up TIMER0
TCCR0A = (1 << COM0A1 ) | (0 << COM0A0 ) | (1 << COM0B1 ) | (0 << COM0B0 )
          | (1 << WGM01 ) | (1 << WGM00 );
TCCR0B = (0 << WGM02 ) | (0 << CS02) | (1 << CS01) | (1 << CS00 );


TCCR1A = (0 << COM1A1 ) | (1 << COM1A0 )
          | (0 << COM1B1 ) | (1 << COM1B0 )
          | (0 << WGM11 ) | (0 << WGM10 );
TCCR1B = (0 << WGM13) | (0 << WGM12 )
          | (0 << CS12) | (1 << CS11) | (1 << CS10 );
TIMSK1 = (0 << OCIE1B ) | (0 << OCIE1A ) | (1 << TOIE1 );
//TIFR1 = (1<< TOV1);

TCCR2A = (1 << COM2A1 ) | (0 << COM2A0 ) | (1 << COM2B1 ) | (0 << COM2B0 )
          | (1 << WGM21 ) | (1 << WGM20 );
TCCR2B = (0 << WGM22 ) | (0 << CS22) | (1 << CS21) | (1 << CS20 );
i2c_init();
MPU6050_writeSensor(MPU6050_PWR_MGMT_1, 0);
sei();
}

void scaled_data(){
  Ax = accelX/128;
  Ay = accelY/128;
  Az = accelZ/128;
}

void vinkel(){
  y_angle = atan(-accelX/sqrt(accelY*accelY+accelZ*accelZ))/3.1416;
  x_angle = atan(accelY/sqrt(accelX*accelX+accelZ*accelZ))/3.1416;
}


int main(void){
  init();
  OCR0A = 0;
  OCR0B = 0;
  OCR2A = 0;
  OCR2B = 0;
  uint8_t forward = 0b00010111;
  uint8_t motor_right = 0b00010100;
  uint8_t motor_left = 0b00000011;
  int16_t right = 0;
  int16_t left = 0;

  PORTC = 0x01;
  PORTD = 0x00;
  while(1){
    get_data();
    scaled_data();
    //vinkel();

    right = (Ax + Ay);
    left = (Ax - Ay);

    if (abs(right)>255){
      right = 255*((right > 0) - (right < 0));
    }
    if (abs(left)>255){
      left = 255*((left > 0) - (left < 0));
    }
    
    if (right>=0 && left>=0){
      PORTD = forward;
      PORTB = 0x00;
    }
    else if (right>=0 && left < 0){
      PORTD = motor_right; 
      PORTB = forward^motor_right;
    }
    else if(right<0 && left>=0){
      PORTD = motor_left;
      PORTB = forward^motor_left;
    }
    else{
      PORTD = 0x00;
      PORTB = forward;     
    }

    OCR0A = abs(right);
    OCR0B = abs(left);

    OCR2A = abs(left);
    OCR2B = abs(right);
    _delay_ms(10);
  }
}