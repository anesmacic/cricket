#include <avr/io.h>              
#include <stdlib.h>
#include <ctype.h>  
#include <avr/interrupt.h>
#define F_CPU 16000000UL  
#include <util/delay.h>                     

volatile uint32_t CURRENT_TRIGGER_COUNT = 0;
volatile uint32_t REQUESTED_TRIGGER_COUNT = 26;
volatile uint32_t TRIGGER_FREQUENCY = 0;
volatile uint16_t TRIGGER_DUTY_CYCLE = 0;
volatile uint8_t IS_ON = 0;
volatile int isAvailable = 0;
volatile int num = 0;
volatile int digits_received = 0;

#define CRYSTAL_OSCILLATOR_FREQUENCY 16000000UL

volatile int n = 0;
volatile uint8_t buffer[7] = {0};
volatile uint8_t bufferIndex = 0;

void USART_init() {
  UBRR0H = 0;
  UBRR0L = 103;
  UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}


void presentTriggers(int PERIOD, int ONTIME){
  IS_ON = 1;
  CURRENT_TRIGGER_COUNT = 0;
  TCCR1A = 0;
  TCCR1B = 0;

  TCCR1A=0b01010011; // fast pwm mode(WGM10 and WGM11 are 1)
  TCCR1B=0b00011001; // no prescaler and WGM12 and WGM13 are 1

  OCR1A = PERIOD;
  OCR1B = ONTIME;
}


int main(void) {


  sei();

  isAvailable = 1;
  
  USART_init();
  while (1) {}                   
  
  return 0;    
}


ISR(USART_RX_vect) {
  uint8_t receivedByte = UDR0;
  if (bufferIndex == 0) {
    // Expecting the message to start with 'f'
    if (receivedByte == 'f') {
      buffer[bufferIndex] = receivedByte;
      bufferIndex++;
    }
  } else if (bufferIndex < 6) {
    // Expecting the next 5 bytes to be digits
    if (isdigit(receivedByte)) {
      buffer[bufferIndex] = receivedByte;
      bufferIndex++;
    }
  } else {
    // Ignore any additional bytes received
  }
  if (bufferIndex == 6) {
    // Convert the message to an integer and save to 'n'
    char* endptr;
    n = strtol((char*)buffer + 1, &endptr, 10);
    bufferIndex = 0;

    if (n == 0){ // stop trigger
      TCCR1A = 0;
      TCCR1B = 0;
      PORTB &= ~(1 << 1);  
      DDRB  &= ~(1 << 1); 
    } else{
      DDRB |= 2; 

      presentTriggers(n,n/2);
    }
  } else {
  PORTB &= ~(1 << 1); // Reset PB1 pin to low
}
}

