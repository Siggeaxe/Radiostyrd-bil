#define F_CPU 1000000UL
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "lcd.h"
#include "lcd.c"

#include <nokia5110.h> // display routines for the graphics LCD
#include "nokia5110.c"

void init()
{
  NOKIA_init(0);

  lcd_init(LCD_DISP_ON);
  lcd_clrscr();
}

int main(void)
{
  char buffer[40];

  uint8_t i=0;
  init();
    
  while (1)
  {
    sprintf(buffer, "VOP 0x%02x", i);
    
    NOKIA_setVop(i);               // set contrast voltage
    NOKIA_print(0, 0, buffer, 0);  // print message in Nokia framebuffer
    NOKIA_update();                // transfer framebuffer to the Nokia display

    lcd_gotoxy(0, 0);
    lcd_puts(buffer);              // print message also on text-LCD

    i++;

    _delay_ms(100);
  }
}
