// 7-segment x4 counter 0-9999

#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
uint8_t font[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};

volatile uint16_t counter = 0;
volatile uint16_t _counter = 0;
volatile uint8_t framebuffer[4];
volatile uint8_t current_digit = 0; // which digit is active now

ISR(TIMER0_OVF_vect)
{
// the code for the interrupt service routine
PORTD = 0xff; // all segments off
current_digit += 1; // step one digit
current_digit %= 4; // and keep between 0-3
PORTB = (1 << current_digit);

framebuffer[0] = counter/1000%10;
framebuffer[1] = counter/100%10;
framebuffer[2] = counter/10%10;
framebuffer[3] = counter%10;

_counter += 1;
if (_counter > 100)
{
    _counter = 0;
    counter += 1;

    if (_counter > 999)
    {
        counter = 0;
    }
}

PORTD = ~font[framebuffer[current_digit]]; // -- to be replaced! --
}

void init(void)
{
    // we can collect all hardware initialization here
    DDRB = 0b00001111; // lower B-pins as output
    DDRD = 0b11111111; // all D-pins as output

    // setting up TIMER0
    TCCR0A = (0 << COM0A1) | (0 << COM0A0) // we'll use PWM later
    | (0 << COM0B1) | (0 << COM0B0) // we'll use PWM later
    | (0 << WGM01) | (0 << WGM00); // WGMxx sets the mode of the timer

    TCCR0B = (0 << WGM02) // WGMxx sets the mode of the timer
    | (0 << CS02) | (1 << CS01) | (0 << CS00); // clock prescaler

    // enable the TIMER0 OVERFLOW INTERRUPT
    TIMSK0 = (0 << OCIE0B) | (0 << OCIE0A) | (1 << TOIE0);

    // don't forget to enable interrupts
    sei();
}

int main(void)
{
    init();
    while (1)
    {
        // Code goes here
    }
}