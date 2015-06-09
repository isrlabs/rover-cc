extern "C" {
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
}

#include "hardware/hardware.h"


#define TOP		39999


static void
initTimer3(void)
{
	// Timer3 should only be started by the mast pulse sequence.
	PRR1 &= ~_BV(PRTIM3);

	TCCR3A = 0;
	TCCR3B = _BV(CS11);	// Set prescaler to 8.

	// Now, Timer3 must be prepared for use.
	TCNT3 = 0;		// Reset the timer counter.
	TIFR3 |= _BV(OCF3A);	// Drop any existing interrupts on the timer.
	TIMSK3 |= _BV(OCIE3A);	// Enable Timer3's output compare interrupt.
}


static void
initTimer1(void)
{
	// Disable powersaving mode on Timer1 to enable it.
	PRR0 &= ~_BV(PRTIM1);

	TCCR1A = 0;
	TCCR1B = _BV(CS11);	// Set prescaler to 8.

	// Now, Timer1 must be prepared for use.
	TCNT1 = 0;		// Reset the timer counter.
	TIFR1 |= _BV(OCF1A);	// Drop any existing interrupts on the timer.
	TIMSK1 |= _BV(OCIE1A);	// Enable Timer1's output compare interrupt.
}


void
hardware::init(void)
{
	initTimer1();
	initTimer3();
	sei();
}

