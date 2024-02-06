/* 
 * ws2812.c
 *
 * Created: 2015-11-15
 *  Author: uwezi
 */ 

  #define F_CPU 8000000UL

  #include <avr/io.h>
  #include <util/delay.h>
  #include <util/atomic.h>

  #include <stdlib.h>
  #include <stdio.h>
  
  #include "ws2812.h"

  uint8_t *framebuffer;
  uint8_t framesize;  
//=================================================================================
// help routines for WS2812
//
void WS2812_init(uint8_t count, uint8_t width)
{
  uint16_t size;
  size = count*width;
  framebuffer = malloc(size);
  if (framebuffer==NULL)
  {
    framesize = 0;
  } 
  else
  {
    framesize = size;
  }
}

void WS2812_write_RGB(uint16_t pos, uint8_t r, uint8_t g, uint8_t b)
{
  uint8_t *ptr;
  ptr = framebuffer + pos*3;
  *ptr = g;
  ptr++;
  *ptr = r;
  ptr++;
  *ptr = b;
}

void WS2812_write_RGBW(uint16_t pos, uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
  uint8_t *ptr;
  ptr = framebuffer + pos*4;
  *ptr = g;
  ptr++;
  *ptr = r;
  ptr++;
  *ptr = b;
  ptr++;
  *ptr = w;
}
void WS2812_write(uint16_t pos, uint8_t value)
{
  uint8_t *ptr;
  ptr = framebuffer + pos;
  *ptr = value;
}

void send_byte(uint8_t outbyte)
{
  uint8_t old_port, port_h, port_l;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {  
  old_port = WSPORT;
  port_h   = old_port |  (1 << WSPIN);
  port_l   = old_port & ~(1 << WSPIN);
  uint8_t i;
  for (i=0; i<8; i++)
  {
    if (outbyte & 0b10000000)
    {
      WSPORT = port_h; // here we set the pin to 1
      _delay_us(T1H);
      WSPORT = port_l;
      _delay_us(T1L);
    }
    else
    {
      WSPORT = port_h;
      _delay_us(T0H);
      WSPORT = port_l;
      _delay_us(T0L);
    }
    outbyte = outbyte << 1;
  }
  WSPORT = old_port;
  }  
}

void WS2812_clear(void)
{
  uint16_t i;
  uint8_t *ptr;
  ptr = framebuffer;
  for (i=0; i<framesize; i++)
  {
    *ptr = 0;
    ptr++;
  }
}

void WS2812_update(void)
{
  uint16_t i;
  uint8_t *ptr;
  ptr = framebuffer;
  for (i=0; i<framesize; i++)
  {
    send_byte(*ptr);
    ptr++;
  }
}