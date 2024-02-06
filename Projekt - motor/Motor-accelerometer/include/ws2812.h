#ifndef WS2812_H_
#define WS2812_H_

  typedef struct {
    uint8_t	g, r, b;
  } RGB_t;

// send 24bit GRB to WS2812
//   T0H 0.4us - T0L 0.85us
//   T1H 0.8us - T1L 0.45us
//   f_CPU 8MHz - 0.125us clock cycle -> it's getting tight!
#define T0H 0.1
#define T0L 0.9 
#define T1H 0.7  
#define T1L 0.4

#define WSPORT PORTB
#define WSPIN  PINB0

extern void WS2812_init(uint8_t count, uint8_t width);
extern void WS2812_write_RGB(uint16_t pos, uint8_t r, uint8_t g, uint8_t b);
extern void WS2812_write_RGBW(uint16_t pos, uint8_t r, uint8_t g, uint8_t b, uint8_t w);
extern void WS2812_write(uint16_t pos, uint8_t value);

extern void WS2812_clear(void);
extern void WS2812_update(void);

#endif /* WS2812_H_ */