extern "C" {
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
}

#include "hardware/pwm.h"
#include "hardware/mast.h"
#include "hardware/uart.h"


#define MAST_SERVO	0
#define MAST_PIN	PB4

#define MAST_MIN	750
#define MAST_CENTRE	1550
#define MAST_MAX	2250


#define CF_PIN		PH3
#define CF_PORT		PORTH
#define CF_DDR		DDRH

#define FIRE_LENGTH	3
#define FIRE_WAIT	750
#define FIRE_UPDATE	115
#define FIRE_MAX	18500
#define FIRE_REFRESH	200;

#define PING_DDR	DDRE
#define PING_PORT	PORTE
#define PING_PIN	PE4

#define MICROSEC_PER_CM	53


#define MAST_POSITIONS	5
static uint16_t	mast_position[] = {800, 1125, 1550, 1925, 2250};
static bool	mast_left = true;


static volatile struct {
	mast::E_PING_STATE	st;
	uint16_t	read_start;
	uint16_t	read_stop;
	uint8_t		angle;
	uint16_t	distance;
} USR;


void
mast::init()
{
	DDRB |= _BV(MAST_PIN);

	// Connect the mast positioning servo.
	pwm::connect(MAST_SERVO, MAST_PIN);
	pwm::set_limits(MAST_SERVO, MAST_MIN, MAST_MAX);
	pwm::set_servo(MAST_SERVO, MAST_CENTRE);

	// Set up collision indicator.
	CF_DDR |= _BV(CF_PIN);

	PING_DDR |= _BV(PING_PIN);
	PING_PORT &= ~_BV(PING_PIN);
	USR.st = PING_STOPPED;
}


void
mast::self_test()
{
	uint8_t	i;
	for (i = 0; i < 5; i++) {
		CF_PORT ^= ~(_BV(CF_PIN));
		pwm::set_servo(MAST_SERVO, mast_position[i]);
		_delay_ms(500);
	}

	pwm::set_servo(MAST_SERVO, mast_position[1]);
}


void
mast::collision_indicator(bool on)
{
	if (on) {
		CF_PORT |= _BV(CF_PIN);
	}
	else {
		CF_PORT &= ~_BV(CF_PIN);
	}
}


void
mast::next()
{
	static uint8_t	position = 0;

	// pwm::set_servo(MAST_SERVO, mast_position[position]);
	if (mast_left) {
		if (position == MAST_POSITIONS - 1) {
			mast_left = false;
		}
		else {
			position++;
		}
	}
	else {
		if (position == 0) {
			mast_left = true;
		}
		else {
			position--;
		}
	}

	if (USR.st == mast::PING_STOPPED) {
		USR.st = mast::PING_FIRE_START;
		TCNT3 = 0;
		PRR1 &= ~_BV(PRTIM3);
		OCR3A = FIRE_WAIT;
	}
}


struct mast::USR_reading
mast::last_distance()
{
	struct mast::USR_reading reading;

	reading.distance = USR.distance;
	reading.angle = USR.angle;
	return reading;
}


static uint8_t
_mast_angle(void)
{
	uint32_t	num, den;

	num = (pwm::get_servo(MAST_SERVO) - MAST_MIN) * 1000;
	den = (uint16_t)(MAST_MAX - MAST_MIN) * 1000;
	return uint8_t(((num / den) * 180) / 1000);
}

uint8_t
mast_angle(void)
{
	return (uint8_t)_mast_angle();
}


mast::E_PING_STATE
mast::state()
{
	return USR.st;
}


ISR(TIMER3_COMPA_vect)
{
	CF_PORT |= _BV(CF_PIN);
	switch (USR.st) {
	case mast::PING_FIRE_START:
		// To initiate a pulse, set the ping pin as an output
		// pin and set it high. Then, transition to the next
		// state and update after FIRE_LENGTH microseconds.
		PING_DDR |= _BV(PING_PIN);
		PING_PORT |= _BV(PING_PIN);
		USR.st = mast::PING_FIRE_STOP;
		OCR3A = FIRE_LENGTH;
		TCNT3 = 0;
		CF_PORT |= _BV(CF_PIN);
		break;
	case mast::PING_FIRE_STOP:
		PING_PORT &= ~_BV(PING_PIN);
		CF_PORT &= ~_BV(CF_PIN);

		// Reset TCNT3.
		TCNT3 = 0;
		OCR3A = FIRE_WAIT;

		USR.st = mast::PING_READ_IN;
		break;
	case mast::PING_READ_IN:
		if (bit_is_set(PING_DDR, PING_PIN)) {
			mast::collision_indicator(true);
			PING_DDR &= _BV(PING_PIN);
			PING_PORT |= _BV(PING_PIN);
		}

		if (bit_is_set(PING_PORT, PING_PIN)) {
			TCNT3 = 0;
			OCR3A = FIRE_UPDATE;
			USR.st = mast::PING_READ_OUT;
			USR.read_start = TCNT3;
		}
		else if (TCNT3 < FIRE_MAX) {
			OCR3A = TCNT3+FIRE_LENGTH;
		}
		else {
			USR.st = mast::PING_STOPPED;
		}
		break;
	case mast::PING_READ_OUT:
		mast::collision_indicator(false);
		if (bit_is_clear(PING_PORT, PING_PIN)) {
			USR.read_stop = TCNT3;
			TCNT3 = 0;
			OCR3A = FIRE_REFRESH;
			USR.st = mast::PING_STOPPED;
		}
		else {
			OCR3A = TCNT3+FIRE_LENGTH;
		}
		break;
	default:
		mast::collision_indicator(false);
		CF_PORT &= ~_BV(CF_PIN);
		PING_PORT &= ~_BV(PING_PIN);

		// Reset TCNT3.
		TCNT3 = 0;
		OCR3A = FIRE_REFRESH;

		// Disable the timer.
		PRR1 |= _BV(PRTIM3);

		// Drop any existing interrupts on the timer.
		TIFR3 |= _BV(OCF3A);

		USR.st = mast::PING_STOPPED;
	}
}
