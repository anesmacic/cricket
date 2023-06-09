#include <avr/io.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define CRYSTAL_OSCILLATOR_FREQUENCY 16000000UL
#define BAUD_RATE 9600UL;

volatile int DAQ_NUMBER_OF_CYCLES = 0;
volatile int OCT_NUMBER_OF_CYCLES = 0;

volatile bool UART_COMMAND_IS_VALID = false;
volatile int UART_buffer_index = 0;
volatile unsigned char UART_message[6] = {0};

void configure_timers()
{
  PORTB &= ~(1 << PB1); 
  PORTD &= ~(1 << PD6); 

  DDRB |= (1 << PB1); 
  DDRD |= (1 << PD6); 

  TCCR0A = (1 << COM0A0) | (1 << WGM01);
  TCCR0B = (1 << CS00);
  OCR0A = DAQ_NUMBER_OF_CYCLES;

  TCCR1B = (1 << WGM12) | (1 << CS10);
  TCCR1A = (1 << COM1A0);
  OCR1A = OCT_NUMBER_OF_CYCLES;

  TCNT0 = DAQ_NUMBER_OF_CYCLES - 1;
  TCNT1 = OCT_NUMBER_OF_CYCLES - 1;
}

void stop_triggers()
{
  GTCCR = (1 << TSM) | (1 << PSRASY) | (1 << PSRSYNC);
  TCCR0A = (1 << COM0A1) | (1 << WGM01);
  TCCR0B = (1 << CS00);
  TCCR0B = (1 << FOC0A);
  TCCR1A = (1 << COM1A1);
  TCCR1B = (1 << WGM12) | (1 << CS10);
  TCCR1C = (1 << FOC1A);
  GTCCR = 0;
  PORTB &= ~(1 << PB1);
  PORTD &= ~(1 << PD6);
  DDRB &= ~(1 << PB1);
  DDRD &= ~(1 << PD6);
}



void initiate_triggers()
{
  stop_triggers();
  GTCCR = (1 << TSM) | (1 << PSRASY) | (1 << PSRSYNC);
  configure_timers();
  GTCCR = 0;
}

void initialize_USART()
{
  UBRR0H = 0;
  UBRR0L = 103; 
  UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

int main(void)
{
  sei();
  initialize_USART();
  while (1){};
  return 0;
}

ISR(USART_RX_vect)
{
  uint8_t byte = UDR0;
  if (UART_buffer_index == 0)
  {
    if (byte == 'S')
    {
      UART_COMMAND_IS_VALID = true;
      UART_buffer_index++;
    }
    else
    {
      UART_COMMAND_IS_VALID = false;
      stop_triggers();
    }
  }
  else if ((UART_buffer_index < 7) & UART_COMMAND_IS_VALID)
  {
    if ('0' <= byte && byte <= '9')
    {
      UART_message[UART_buffer_index - 1] = (byte - '0');
      UART_buffer_index++;
    }
    else
    {
      UART_buffer_index = 0;
      UART_COMMAND_IS_VALID = false;
      stop_triggers();
    }
  }
  else
  {
    if (UART_COMMAND_IS_VALID)
    {

      DAQ_NUMBER_OF_CYCLES = UART_message[0] * 100 + UART_message[1] * 10 + UART_message[2];
      OCT_NUMBER_OF_CYCLES = UART_message[3] * 100 + UART_message[4] * 10 + UART_message[5];

      char reply_message[20];
      sprintf(reply_message, "DAQ: %d, OCT: %d\n", DAQ_NUMBER_OF_CYCLES, OCT_NUMBER_OF_CYCLES);

      int i = 0;
      while (reply_message[i] != '\0')
      {
        while (!(UCSR0A & (1 << UDRE0)));
        UDR0 = reply_message[i];
        i++;
      }

      UART_buffer_index = 0;
      UART_COMMAND_IS_VALID = false;

      if ((DAQ_NUMBER_OF_CYCLES == 0) || (OCT_NUMBER_OF_CYCLES == 0))
      {
        stop_triggers();
      }
      else
      {
        initiate_triggers();
      }
    }
    else
    {
      UART_buffer_index = 0;
      stop_triggers();
    }
  }
}
