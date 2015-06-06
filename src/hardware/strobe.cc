extern "C" {
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
}

#include "hardware/strobe.h"


#define STROBE_PIN	PE4	// strobe LED
#define STROBE_PORT	PORTE
#define STROBE_DDR	DDRE

#define STROBED_PIN	PJ1	// strobe detector
#define STROBED_PORT	PORTJ
#define STROBED_DDR	DDRJ

#define CF_PIN		PH3
#define CF_PORT		PORTH
#define CF_DDR		DDRH


// The strobe should operate on a 38kHz cycle. This is 26.32 us.
#define STROBE_CYCLE	26
#define STROBE_WAIT	5
#define STROBE_TICKS	120


static bool		strobe_active = false;
static volatile bool	strobe_pulsing = false;
static volatile bool	proximity_alert = false;


static inline void
strobe_on(void)
{
	STROBE_PORT |= _BV(STROBE_PIN);
	strobe_active = true;
}


static inline void
strobe_off(void)
{
	STROBE_PORT &= ~_BV(STROBE_PIN);
	strobe_active = false;
}


static inline void
strobe_toggle(void)
{
	if (strobe_active) {
		strobe_off();
	}
	else {
		strobe_on();
	}
}


void
collision_indicator(bool on)
{
	if (on) {
		CF_PORT |= _BV(CF_PIN);
	}
	else {
		CF_PORT &= ~_BV(CF_PIN);
	}
}


void
strobe_init(void)
{
	// Set up IR strobe LED and detector.
	STROBE_DDR = _BV(STROBE_PIN);
	STROBED_DDR &= ~_BV(STROBED_PIN);
	STROBED_PORT |= _BV(STROBED_PIN);

	// Set up collision detector.
	PCMSK1 = _BV(PCINT10);
	PCIFR |= _BV(PCIF1);

	// Set up collision indicator.
	CF_DDR |= _BV(CF_PIN);

	// Ensure the IR strobe and collision indicator are turned off.
	strobe_off();
	collision_indicator(false);
}


static volatile uint16_t	strobe_ticks = 0;


void
strobe_activate()
{
	proximity_alert = false;
	strobe_pulsing = true;
	PCICR |= _BV(PCIE1);
	PCIFR |= _BV(PCIF1);
	PRR1 &= ~_BV(PRTIM3);
	strobe_ticks = 0;
}


bool
strobe_detect()
{
	return proximity_alert;
}


void
strobe_clear_alarm()
{
	PCICR &= ~_BV(PCIE1); // Disable proximity alarm.
	PCIFR |= _BV(PCIF1);  // Clear any pending proximity alarms.
	proximity_alert = false;
}


ISR(TIMER3_COMPA_vect)
{
	if (strobe_ticks++ < STROBE_TICKS) {
		if (TCNT3 + STROBE_WAIT < STROBE_CYCLE) {
			OCR3A = STROBE_CYCLE;
			strobe_toggle();
		}
		else {
			TCNT3 = 0;
			OCR3A = TCNT3+STROBE_WAIT;
		}
	}
	else {
		strobe_pulsing = false;
		PRR1 |= _BV(PRTIM3);
		strobe_off();
		PCICR &= ~_BV(PCIE1); // Disable proximity alarm.
	}
}


ISR(PCINT1_vect)
{
	if (strobe_pulsing) {
		proximity_alert = !proximity_alert;
	}
	else {
		proximity_alert = false;
	}
}
