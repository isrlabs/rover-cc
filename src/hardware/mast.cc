// The mast namespace contains functions for steering the USR mast and
// performing distance measurements. The mast consists of a USR mountedd
// to a servo that swings through a 180 degree arc such that 90 degrees
// is pointing forward. The USR should take measurements in 45 degree
// steps, which can be fed to the navigation system.

extern "C" {
#include <assert.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdio.h>
}


#include "hardware/pwm.h"
#include "hardware/mast.h"
#include "hardware/uart.h"


#ifndef NULL
#define NULL	0
#endif

#define MAST_SERVO	0
#define MAST_PIN	PB4	// Digital pin 10

#define CF_PIN		PH3
#define CF_PORT		PORTH
#define CF_DDR		DDRH	// Digital pin 6

// The following values are for the servo used to orient the mast.
#define MAST_MIN	750
#define MAST_CENTRE	1550
#define MAST_MAX	2250

// Predefine the 0, 45, 90, 135, and 180 degree orientations for the
// mast. The 0-index corresponds to a 0 degree orientation, which is
// the rightmost orientation.
#define MAST_POSITIONS	5
static uint16_t	mast_position[] = {2250, 1925, 1550, 1125, 800};
static bool	mast_left = true;	// Mast should turn to the left.


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

	pwm::set_servo(MAST_SERVO, mast_position[2]);
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

	pwm::set_servo(MAST_SERVO, mast_position[position]);
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
	uint8_t	angle = _mast_angle();
	return angle;
}
