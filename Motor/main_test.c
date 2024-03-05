#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
//#include "i2cmaster.h"
//#include "twimaster.c"
//#include <mpu.inc>
#include "uart.c"
#include "uart.h"

#define UART_BAUD_RATE 9600      
#define WORD_SIZE 3
#define BUFFER_SIZE 32

void init(void){
uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
sei();

}

int main(void){
    init();

while(1){
    uart_puts("RC BIl\n");
    // uart_putc('C'); uart_putc('O'); uart_putc('O'); uart_putc('L');
    // uart_putc('\n'); uart_putc('\r');
    _delay_ms(100);
}
}