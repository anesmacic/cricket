// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <avr/interrupt.h>
#include <util/delay.h>                     /* Functions to waste time */

#define F_CPU 16000000UL  

volatile uint32_t CURRENT_TRIGGER_COUNT = 0;
volatile uint32_t REQUESTED_TRIGGER_COUNT = 0;
volatile uint32_t TRIGGER_FREQUENCY = 0;
volatile uint16_t TRIGGER_DUTY_CYCLE = 0;

#define TRIGGER_ON  2; 
#define TRIGGER_OFF 0; 


#define CRYSTAL_OSCILLATOR_FREQUENCY 16000000UL
#define CRYSTAL_OSCILLATOR_PRESCALER 8

// Using Timer 1 16-bit

int main(void) {

  // -------- Inits --------- //
  DDRB |= 0b00000001;            /* Data Direction Register B:
                                   writing a one to the bit
                                   enables output. */
  sei();
  // Set timer
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << CS12) | (1 << WGM12); // prescaler
  TIMSK1 |= (1 << OCIE1A) ; 
  OCR1A = 4000;
  // ------ Event loop ------ //
  while (1) {
  }                                                  /* End event loop */
  return 0;                            /* This line is never reached */
}


ISR(TIMER1_COMPA_vect){
  if (CURRENT_TRIGGER_COUNT % 2 == 0){
    PORTB = 0b00000010;          /* Turn on first LED bit/pin in PORTB */
  } else {
    PORTB = 0b00000000;          /* Turn on first LED bit/pin in PORTB */
  }
  CURRENT_TRIGGER_COUNT++;
}
