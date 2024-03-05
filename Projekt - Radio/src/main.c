#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <uart.h>
#include <i2cmaster.h>
#include <twimaster.c>
#include <mpu.inc>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>        

#define UART_BAUD_RATE 9600      
#define AVG 8

volatile int32_t accelX=0, accelY=0, accelZ=0;
volatile int16_t Ax=0, Ay=0, right = 0, left = 0;

void get_data(void){
  accelX = (MPU6050_readSensor16(MPU6050_ACCEL_XOUT_L,MPU6050_ACCEL_XOUT_H) + (AVG-1)*accelX)/AVG;
  accelY = (MPU6050_readSensor16(MPU6050_ACCEL_YOUT_L,MPU6050_ACCEL_YOUT_H) + (AVG-1)*accelY)/AVG;
  accelZ = (MPU6050_readSensor16(MPU6050_ACCEL_ZOUT_L,MPU6050_ACCEL_ZOUT_H) + (AVG-1)*accelZ)/AVG;
}

void scaled_data(){
  Ax = accelX/64;
  Ay = accelY/64;

  right = (Ax + Ay);
  left = (Ax - Ay);
}

void init()
{
  // DDRB = 0x00000001;  // LED
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
  i2c_init();
  MPU6050_writeSensor(MPU6050_PWR_MGMT_1, 0);
  
  cli();
  //reset watchdog
  wdt_reset();
  //set up WDT interrupt
  WDTCSR = (1<<WDCE) | (1<<WDE) |(0<<WDIE) | (0<<WDP3)
          | (0<<WDP2) | (1<<WDP1) | (1<<WDP0);
  //Enable global interrupts
  sei();
}

int main(void)
{
  uint8_t dir = 0, tempL = 0, tempR = 0;

  init();

  for(;;)
  {
    wdt_reset();
    get_data();
    scaled_data();

    // Left, right  
    if (abs(right)>255){
      right = 255*((right > 0) - (right < 0));
    }
    if (abs(left)>255){
      left = 255*((left > 0) - (left < 0));
    }

    // Direction
    if (right>=0 && left>=0){
      dir = 1;
    }
    else if (right>=0 && left < 0){
      dir = 2;
    }
    else if(right<0 && left>=0){
      dir = 4;
    }
    else{
      dir = 8;
    }

    tempL = abs(left);
    tempR = abs(right);

    // Code the messages
    tempL = (tempL >> 2) | 0b10000000;
    tempR = (tempR >> 2) | 0b01000000;
    dir = dir | 0b11000000;

    // Send through UART
    uart_putc(tempR);
    uart_putc(tempL);
    uart_putc(dir);
    uart_putc('\r');

    _delay_ms(5);
  }

  return 0;
}
