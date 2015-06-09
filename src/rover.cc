extern "C" {
#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <stdio.h>

#include <stdint.h>
#include <string.h>
}

#include "hardware/hardware.h"
#include "hardware/uart.h"
#include "hardware/drivetrain.h"
#include "hardware/mast.h"

#define LED1		PH4
#define LED2		PH5

// Number of ticks to before a transition is required.
#define DT_TRIGGER_TCK	99
#define DT_ROT_TRIGGER	59

#define SEROUT(x)	uart::writeln((char *)(x))


/*
 * Beacon controls the LED beacons on the front of the rover. These
 * flash in sequence and are used to visually identify the rover.
 */
void
beacon(void)
{
	static uint8_t	bcn_ticks = 0;

	switch (bcn_ticks) {
	case 0:
		PORTH ^= _BV(LED1);
		break;
	case 10:
		PORTH ^= _BV(LED1);
		break;
	case 30:
		PORTH ^= _BV(LED2);
		break;
	case 40:
		PORTH ^= _BV(LED2);
		break;
	case 90:
		bcn_ticks = 0;
		return;
	}

	bcn_ticks++;
}


#define MAST_TURN	100

static void
mast_step(void)
{
	static uint16_t	ticks = 0;

	ticks++;
	if (ticks < MAST_TURN) {
		return;
	}

	ticks = 0;
	SEROUT("MAST STEP");
	mast::next();

		mast::E_PING_STATE st;
		char	ds[64];

		memset(ds, 0, 64);
		st = mast::state();
		switch (st) {
		case mast::PING_FIRE_START:
			SEROUT("pinging...");
			break;
		case mast::PING_FIRE_STOP:
			SEROUT("stopping ping...");
			break;
		case mast::PING_READ_IN:
			SEROUT("waiting for ping in...");
			break;
		case mast::PING_READ_OUT:
			SEROUT("received response");
			break;
		default:
			SEROUT("unknown mast state");
		}


		struct mast::USR_reading reading = mast::last_distance();
		sprintf(ds, "D: %u @ %u (%u, %u)", reading.angle, reading.distance,
		    TCNT3, OCR3A);
		uart::writeln(ds);

}


int
main()
{
	hardware::init();
	uart::init();

	SEROUT("BOOT OK");
	mast::init();
	mast::collision_indicator(true);
	// Note: once hardware debug is over, add self test here.

	drivetrain::init();
	drivetrain::stop();

	// Activate the beacon LEDs.
	DDRH |= _BV(LED1) | _BV(LED2);
	PORTH &= ~(_BV(LED1)|_BV(LED2));

	SEROUT("READY");
	mast::collision_indicator(false);

	while (1) {
		beacon();
		mast_step();
		_delay_ms(10);
	}

	return 0;
}
