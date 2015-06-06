extern "C" {
#include <avr/io.h>
#include <util/delay.h>

#include <stdint.h>
}

#include "hardware/hardware.h"
#include "hardware/drivetrain.h"

#define LED1		PH4
#define LED2		PH5

// Number of ticks to before a transition is required.
#define DT_TRIGGER_TCK	9
#define DT_ROT_TRIGGER	6


enum DT_STATE {
	DT_FORWARD,
	DT_STOP1,
	DT_TURN,
	DT_STOP2
} state;

static void
dt_transition(void)
{
	static uint8_t state_ticks = 0;

	state_ticks++;

	switch (state) {
	case DT_FORWARD:
		if (state_ticks < DT_TRIGGER_TCK) {
			return;
		}
		drivetrain_stop();
		state = DT_STOP1;
		break;
	case DT_STOP1:
		if (state_ticks < DT_TRIGGER_TCK) {
			return;
		}
		drivetrain_rotate_left();
		state = DT_TURN;
		break;
	case DT_TURN:
		if (state_ticks < DT_ROT_TRIGGER) {
			return;
		}
		drivetrain_stop();
		state = DT_STOP2;
		break;
	case DT_STOP2:
		if (state_ticks < DT_TRIGGER_TCK) {
			return;
		}
		drivetrain_forward();
		state = DT_FORWARD;
		break;
	default:
		PORTH |= _BV(LED1) | _BV(LED2);
		while (true) {}
	}

	state_ticks = 0;
}

void
beacon(void)
{
	static uint8_t	bcn_ticks = 0;

	switch (bcn_ticks) {
	case 0:
		PORTH ^= _BV(LED1);
		break;
	case 1:
		PORTH ^= _BV(LED1);
		break;
	case 3:
		PORTH ^= _BV(LED2);
		break;
	case 4:
		PORTH ^= _BV(LED2);
		break;
	case 9:
		bcn_ticks = 0;
		return;
	}

	bcn_ticks++;
}

int
main()
{
	state = DT_STOP2;

	hardware_init();
	drivetrain_init();
	drivetrain_stop();

	DDRH |= _BV(LED1) | _BV(LED2);

	_delay_ms(1000);
	while (1) {
		dt_transition();
		beacon();
		_delay_ms(100);
	}

	return 0;
}
