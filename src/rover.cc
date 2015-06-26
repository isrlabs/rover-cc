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
	mast::self_test();

	drivetrain::init();
	drivetrain::stop();

	// Activate the beacon LEDs.
	DDRH |= _BV(LED1) | _BV(LED2);
	PORTH &= ~(_BV(LED1)|_BV(LED2));

	SEROUT("READY");
	mast::collision_indicator(false);

	drivetrain::forward();

	while (1) {
		beacon();
		mast_step();
		_delay_ms(10);
	}

	return 0;
}
