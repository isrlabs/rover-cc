extern "C" {
#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>

#include <stdint.h>
#include <string.h>
}

#include "hardware/hardware.h"
#include "hardware/drivetrain.h"
#include "hardware/strobe.h"

#define LED1		PH4
#define LED2		PH5

// Number of ticks to before a transition is required.
#define DT_TRIGGER_TCK	9
#define DT_ROT_TRIGGER	6

#define SEROUT(x)	serial_write((char *)(x "\r\n"))


/*
 * serial_init sets up the serial port for 9600 baud communications.
 * BAUD must be defined prior to calling this.
 */
void
serial_init(void)
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	UCSR0A &= ~(1 << U2X0);

	UCSR0B = ((1 << TXEN0)|(1 << RXEN0));
	UCSR0C = ((1 << UCSZ01)|(1 << UCSZ00));
}


/*
 * serial_write writes a sequence of characters to the serial port.
 */
void
serial_write(char *s)
{
	uint8_t l;
	uint8_t	i;

	l = (uint8_t)strlen(s);
	for (i = 0; i != l; i++) {
		loop_until_bit_is_set(UCSR0A, UDRE0);
		UDR0 = s[i];
	}
}


enum DT_STATE {
	DT_FORWARD,
	DT_STOP1,
	DT_BACK,
	DT_STOP2,
	DT_TURN,
	DT_STOP3
} state;

static bool
dt_transition(bool avoiding)
{
	static uint8_t state_ticks = 0;

	if (!avoiding) {
		return false;
	}
	state_ticks++;

	switch (state) {
	case DT_FORWARD:
		if (state_ticks < DT_TRIGGER_TCK) {
			return avoiding;
		}
		SEROUT("DT_FORWARD -> DT_STOP1");
		drivetrain_stop();
		state = DT_STOP1;
		break;
	case DT_STOP1:
		if (state_ticks < DT_TRIGGER_TCK) {
			return avoiding;
		}
		SEROUT("DT_STOP1 -> DT_BACK");
		drivetrain_backward();
		state = DT_BACK;
		break;
	case DT_BACK:
		if (state_ticks < DT_TRIGGER_TCK) {
			return avoiding;
		}
		SEROUT("DT_BACK -> DT_STOP2");
		drivetrain_stop();
		state = DT_STOP2;
		break;
	case DT_STOP2:
		if (state_ticks < DT_TRIGGER_TCK) {
			return avoiding;
		}
		SEROUT("DT_STOP2 -> DT_TURN");
		drivetrain_rotate_left();
		state = DT_TURN;
		break;
	case DT_TURN:
		if (state_ticks < DT_ROT_TRIGGER) {
			return avoiding;
		}
		SEROUT("DT_TURN -> DT_STOP3");
		drivetrain_stop();
		state = DT_STOP3;
		break;
	case DT_STOP3:
		if (state_ticks < DT_TRIGGER_TCK) {
			return avoiding;
		}
		SEROUT("DT_STOP2 -> DT_FORWARD");
		strobe_clear_alarm();
		drivetrain_forward();
		state = DT_FORWARD;
		avoiding = false;
		break;
	default:
		PORTH |= _BV(LED1) | _BV(LED2);
		while (true) {}
	}

	if (avoiding) {
		SEROUT("DT AVOID");
	}
	state_ticks = 0;
	return avoiding;
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
	bool	avoiding = false;

	hardware_init();
	drivetrain_init();
	drivetrain_stop();
	strobe_init();
	serial_init();

	// Activate the beacon LEDs.
	DDRH |= _BV(LED1) | _BV(LED2);

	SEROUT("BOOT OK");
	_delay_ms(1000);

	// Start moving.
	state = DT_FORWARD;
	drivetrain_forward();

	while (1) {
		_delay_ms(100);
		collision_indicator(false);
		avoiding = dt_transition(avoiding);
		beacon();
		if (!avoiding) {
			SEROUT("STROBE");
			strobe_activate();
			if (strobe_detect()) {
				collision_indicator(true);
				SEROUT("PROXIMITY ALARM");
				strobe_clear_alarm();
				avoiding = dt_transition(true);
			}
		}
		strobe_clear_alarm();
	}

	return 0;
}
