/*
 * 20170713_m328_fad.c
 *
 * Created: 2017-07-13 20:41:44
 * Author : uwezi
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/atomic.h>
#include "ws2812.h"
#include "i2cmaster.h"

//static const uint8_t sinetable[] PROGMEM =
//{
  //#include "sinetable_full_8bit_256t.txt"
//};

const char coords[] = "N5951210E01738385";
uint8_t coords_index=0;

// assignment between numbers and place in WS2812 chain
// N=10 E=11                0 1 2 3 4 5 6 7 8 9 10 11
const int8_t values[12] = {10,4,3,2,1,0,5,6,7,8,9,11};

const int16_t faces[12][4] =
{
  {   0,   0,  100,  11 }, 

  {  85,   0,   53,  10 },
  {  26,  81,   53,  6 },
  { -69,  50,   53,  7 },
  { -69, -50,   53,  8 },
  {  26, -81,   53,  9 },

  {  69,  50,  -53, 4 },
  { -26,  81,  -53, 5 },
  { -85,   0,  -53, 1 },
  { -26, -81,  -53, 2 },
  {  69, -50,  -53, 3 },

  {   0,   0, -100, 0 }
};

volatile uint32_t rtc=0;
volatile int16_t dataavg[3];
#define  AVG 8
char buffer[100];

#include "mpu.inc"

volatile int32_t gyroX=0, gyroY=0, gyroZ=0, accelX=0, accelY=0, accelZ=0, Temp=0;


ISR(TIMER0_OVF_vect)
{
  uint8_t i, pos_idx, coord_idx=0;
  int16_t sc_prod[12];
  int16_t max_sc;
  uint32_t absrot;
  uint8_t coord;
  static uint8_t found=0;
  static uint8_t pos_rnd=0;
  static uint8_t finished=0;
  static int16_t timer=0;
  
  rtc++;
  
  Temp   = (MPU6050_readSensor16(MPU6050_TEMP_OUT_L,  MPU6050_TEMP_OUT_H) + (AVG-1)*Temp)/AVG;
  gyroX  = (MPU6050_readSensor16(MPU6050_GYRO_XOUT_L, MPU6050_GYRO_XOUT_H) + (AVG-1)*gyroX)/AVG;
  gyroY  = (MPU6050_readSensor16(MPU6050_GYRO_YOUT_L, MPU6050_GYRO_YOUT_H) + (AVG-1)*gyroY)/AVG;
  gyroZ  = (MPU6050_readSensor16(MPU6050_GYRO_ZOUT_L, MPU6050_GYRO_ZOUT_H) + (AVG-1)*gyroZ)/AVG;
  accelX = (MPU6050_readSensor16(MPU6050_ACCEL_XOUT_L,MPU6050_ACCEL_XOUT_H) + (AVG-1)*accelX)/AVG;
  accelY = (MPU6050_readSensor16(MPU6050_ACCEL_YOUT_L,MPU6050_ACCEL_YOUT_H) + (AVG-1)*accelY)/AVG;
  accelZ = (MPU6050_readSensor16(MPU6050_ACCEL_ZOUT_L,MPU6050_ACCEL_ZOUT_H) + (AVG-1)*accelZ)/AVG;
  
  max_sc  = 0;
  pos_idx = 0;

  coord = (uint8_t)coords[coords_index];
  switch(coord)
  {
    case 'N' : 
      coord = 10;
      break;
    case 'E' :
      coord = 11;
      break;
    default:
      coord -= '0';  
  }    

  for (i=0;i<12;i++)
  {
    sc_prod[i] = faces[i][0]*accelX/128 + faces[i][1]*accelY/128 + faces[i][2]*accelZ/128;
    if (sc_prod[i]>max_sc)
    {
      max_sc  = sc_prod[i];
      pos_idx = faces[i][3];
    }
    if (values[i]==coord)
    {
      coord_idx = i;
    }
  } 
     
  absrot = abs(gyroX)+abs(gyroY)+abs(gyroZ);   
     
  WS2812_clear();

  if (finished)
  {
    timer++;
    switch((timer/10)%3)
    {
      case 0:
        WS2812_write_RGB(coord_idx*3+0, 255, 255, 255);
        WS2812_write_RGB(coord_idx*3+1, 0, 0, 0);
        WS2812_write_RGB(coord_idx*3+2, 0, 0, 0);
        break;
      case 1:
        WS2812_write_RGB(coord_idx*3+0, 0, 0, 0);
        WS2812_write_RGB(coord_idx*3+1, 255, 255, 255);
        WS2812_write_RGB(coord_idx*3+2, 0, 0, 0);
        break;
      case 2:
        WS2812_write_RGB(coord_idx*3+0, 0, 0, 0);
        WS2812_write_RGB(coord_idx*3+1, 0, 0, 0);
        WS2812_write_RGB(coord_idx*3+2, 255, 255, 255);
        break;
    }
    if (timer>150)
    {
      finished = 0;
      coords_index = 0;
    }
  }
  else
  {
    // where should I go? RED
    WS2812_write_RGB(coord_idx*3+0, 255, 0, 0);
    WS2812_write_RGB(coord_idx*3+1, 255, 0, 0);
    WS2812_write_RGB(coord_idx*3+2, 255, 0, 0);
    if (found == 1)
    {
      timer --;
      if (timer <= 0)
      {
        found = 0;
        timer = 0;
        if (coords_index < (strlen(coords)-1))
        {
          coords_index++;
        } 
        else // all found!
        {
          
          finished = 1;
        }
      }      
      else
      {
        WS2812_write_RGB(pos_idx*3+0, 0, 255, 0);
        WS2812_write_RGB(pos_idx*3+1, 0, 255, 0);
        WS2812_write_RGB(pos_idx*3+2, 0, 255, 0);
      }    
    } 
    else  // haven't found it previously
    {
      if (absrot < 2000)
      {
        if (pos_idx == coord_idx) // YEAH! found it 
        {
          found = 1;
          timer = 20;
        }
        else
        {
          timer ++;
          found = 0;
          if (timer >= 200)  // long time no see
          {
            if (timer >= 2500)
            {
              timer = 240;
            }
            pos_idx = (timer / 20)%12;
            if (timer%20 == 0)
            {
              pos_rnd = rand()%12;
            }
            WS2812_clear();
            WS2812_write_RGB(pos_rnd*3+0, 0, 0, 255);
            WS2812_write_RGB(pos_rnd*3+1, 0, 0, 255);
            WS2812_write_RGB(pos_rnd*3+2, 0, 0, 255);
          }
        }
      }
      else  // it's shaking!
      {
        if (timer >= 240)
        {
          timer = 0;
          coords_index = 0;   // restart
        }
      }
    }
  }       
  WS2812_update();
}

void init_timer0(void)
{
  //   normal operation
  //   clk/1024 prescaler (14.4 kHz)
  //   interrupt on Timer 0 overflow
  //  => 56.25 interrupts per second
  
  TCCR0A = (0 << COM0A1) | (0 << COM0A0) | (0 << COM0B1) | (0 << COM0B0)
  | (0 << WGM01)  | (0 << WGM00);
  TCCR0B = (0 << WGM02)
  | (1 << CS02)   | (0 << CS01)   | (1 << CS00);
  TIMSK0 = (0 << OCIE0B) | (0 << OCIE0A) | (1 << TOIE0);
}

void init(void)
{
  DDRB = 0b00000111;

  sei();
  i2c_init();
  MPU6050_writeSensor(MPU6050_PWR_MGMT_1, 0);

  init_timer0();
}


int main(void)
{

  DDRB = (1 << PB1) | (1 << PB0);
  WS2812_init(36, 3);
  init();

  coords_index = 0;
  while (1) 
  {

    _delay_ms(3000);   
 
  }  
}
