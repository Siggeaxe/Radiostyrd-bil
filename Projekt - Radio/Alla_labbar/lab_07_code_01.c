#define F_CPU 1000000UL
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"

volatile uint16_t timer1_ticks=0;

// TIMER2 with 32768Hz crystal, 16 interrupts/s
ISR(TIMER2_OVF_vect)
{
  static uint8_t int_count = 0;
  if (int_count == 16)
  {
    int_count = 0;
    timer1_ticks = TCNT1;
    TCNT1        = 0;
  }
  int_count ++;
}

void init()
{
  lcd_init(LCD_DISP_ON);
  lcd_clrscr();
  
  PORTD = 0b00000100;  // pull up for PD2
  
  _delay_ms(1000);
  
  // run 8-bit TIMER2 on external 32768Hz xtal
  // 8 prescaler, normal operation
  // 16 overflows/s
  // enable TIMER2_OVF interrupt
  ASSR   = (0 << EXCLK) | (1 << AS2);
  TCCR2A = (0 << COM2A1) | (0 << COM2A0)
         | (0 << COM2B1) | (0 << COM2B0)
         | (0 << WGM21)  | (0 << WGM20);                   // set mode
  TCCR2B = (0 << WGM22)
         | (0 << CS22)   | (1 << CS21)   | (0 << CS20);    // set prescaler

  TIMSK2 = (0 << OCIE2B) | (0 << OCIE2A) | (1 << TOIE2);   // enable interrupt
  
  // run 16-bit TIMER on f_cpu
  // 1:64 prescaler, normal operation
  // i.e. 15625 tics per second
  TCCR1A = (0 << COM1A1) | (0 << COM1A0)
         | (0 << COM1B1) | (0 << COM1B0)
         | (0 << WGM11)  | (0 << WGM10);                   // set mode
  TCCR1B = (0 << WGM12)  | (0 << WGM13)
         | (0 << CS12)   | (1 << CS11)   | (1 << CS10);    // set prescaler  

  sei();
}

int main(void)
{
  char buffer[20];
  
  init();
  
  while (1)
  {
    sprintf(buffer, "T1 ticks: %5u",timer1_ticks);
    
    lcd_gotoxy(0,0);
    lcd_puts(buffer);
    _delay_ms(100);
  }    
}