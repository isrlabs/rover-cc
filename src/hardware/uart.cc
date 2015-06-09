extern "C" {
#include <avr/io.h>
#include <util/setbaud.h>
#include <string.h>
}


#include "hardware/uart.h"


void
uart::init()
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	UCSR0A &= ~(1 << U2X0);

	UCSR0B = ((1 << TXEN0)|(1 << RXEN0));
	UCSR0C = ((1 << UCSZ01)|(1 << UCSZ00));
}


const char	CRLF[2] = {'\r', '\n'};

void
uart::writeln(const char *s)
{
	uint8_t l;
	uint8_t	i;

	l = (uint8_t)strlen(s);
	for (i = 0; i != l; i++) {
		loop_until_bit_is_set(UCSR0A, UDRE0);
		UDR0 = s[i];
	}

	for (i = 0; i < 2; i++) {
		loop_until_bit_is_set(UCSR0A, UDRE0);
		UDR0 = CRLF[i];
	}
}

