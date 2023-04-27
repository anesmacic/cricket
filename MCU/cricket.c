// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <avr/interrupt.h>
#include <util/delay.h>                     /* Functions to waste time */

#define F_CPU 16000000UL  

volatile uint32_t CURRENT_TRIGGER_COUNT = 0;
volatile uint32_t REQUESTED_TRIGGER_COUNT = 5;
volatile uint32_t TRIGGER_FREQUENCY = 0;
volatile uint16_t TRIGGER_DUTY_CYCLE = 0;

#define TRIGGER_ON  2; 
#define TRIGGER_OFF 0; 


#define CRYSTAL_OSCILLATOR_FREQUENCY 16000000UL

// Using Timer 1 16-bit

int main(void) {

  // -------- Inits --------- //
  DDRB |= 0b00000001;            /* Data Direction Register B:
                                   writing a one to the bit
                                   enables output. */
  sei();

  // OCR1A GENERATES TRIGGER ON
  // OCR1B GENERATES TRIGGER OFF /


  // Set timer to CTC MODE
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << CS10) | (1 << WGM12); // NO PRESCALING. RUNNING AT 16MHZ
  // ENABLE BOTH A AND B INTERRUPTS
  TIMSK1 |= (1 << OCIE1B) ;
  TIMSK1 |= (1 << OCIE1A) ;


  OCR1A = 300;
  OCR1B = 100;
  // ------ Event loop ------ //
  while (1) {
  }                                                  /* End event loop */
  return 0;                            /* This line is never reached */
}


ISR(TIMER1_COMPA_vect){
  PORTB = 2 ;       /* Turn on first LED bit/pin in PORTB */
}


ISR(TIMER1_COMPB_vect){
  PORTB = 0;
  CURRENT_TRIGGER_COUNT++;
  if (CURRENT_TRIGGER_COUNT == REQUESTED_TRIGGER_COUNT) {
    TCCR1B = 0;
  }
}
