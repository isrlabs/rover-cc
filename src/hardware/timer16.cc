extern "C" {
#include <avr/interrupt.h>
#include <hardware/timer16.h>
#include <stdlib.h>
}


#define ADDRESS8(x)  *reinterpret_cast<volatile uint8_t *>(x)
#define ADDRESS16(x) *reinterpret_cast<volatile uint16_t *>(x)

namespace {
	// 16-bit timer offsets. For memory locations, see page 399
	// (section 33, Register Summary) of the ATmega2560 datasheet.
	constexpr uint8_t	TIMER1_BASE = 0x80;
	constexpr uint8_t	TCNT_OFFSET = 4;

	constexpr uint8_t	OCFnA = (1 << 2);
	constexpr uint8_t	OCFnB = (1 << 3);
	constexpr uint8_t	TIMER1_IFR = 0x36;
	constexpr uint8_t	TIMER3_IFR = 0x38;

	// CS10, CS11, and CS12 control the prescaler. See page
	// of the datasheet.
	constexpr uint8_t	CS0 = (1 << 1);
	constexpr uint8_t	CS1 = (1 << 2);
	constexpr uint8_t	CS2 = (1 << 3);

	constexpr uint8_t	TIFR_BASE = 0x35;

	uint8_t
	timer_number(timer16::select t)
	{
		switch (t) {
		case timer16::TIMER1:
			return 1;
		case timer16::TIMER3:
			return 3;
		}
		return 0;
	}

	void
	default_handler(void)
	{
		return;
	}
}



Timer16::Timer16(const enum timer16::select base) :
	// The base index is used to determine the base memory address
	// for a given timer.
	baseptr(TIMER1_BASE + ((static_cast<uint8_t>(base) - 1) << 4)),
	timer(timer_number(base))
{
	// Set the timer to normal mode.
	ADDRESS8(this->baseptr) = 0;

	// Set the prescaler.
	ADDRESS8(this->baseptr+1) |= (1 << CS1);

	// Clear the counter.
	ADDRESS16(this->baseptr+TCNT_OFFSET) = 0;

	(this->handler) = default_handler;
}


void
Timer16::set_mode(const enum timer16::mode mode)
{
	ADDRESS8(this->baseptr) = 0;

	switch (mode) {
	case timer16::NORMAL:
		// Nothing needs to be done in normal mode.
		break;
	case timer16::CTC:
		ADDRESS8(this->baseptr) = 0;
		ADDRESS8(this->baseptr + 1) = static_cast<uint8_t>(mode);
		break;
	}
}


void
Timer16::set_prescale(const enum timer16::prescale prescaler)
{
	uint8_t	scale;

	switch (prescaler) {
	case timer16::PRESCALE0:
		scale = CS0;
		break;
	case timer16::PRESCALE8:
		scale = CS1;
		break;
	}
	ADDRESS8(this->baseptr + 1) = scale;
}


void
Timer16::clear_interrupts(void)
{
	ADDRESS8(TIFR_BASE + this->timer) |= OCFnA;
}


void
Timer16::reset(void)
{
	// Clear the counter.
	ADDRESS16(this->baseptr+TCNT_OFFSET) = 0;

	this->clear_interrupts();
}


ISR(TIMER1_COMPA_vect)
{
	if (NULL != timer16::Timer1.handler) {
		timer16::Timer1.handler();
	}
}


ISR(TIMER3_COMPA_vect)
{
	if (NULL != timer16::Timer3.handler) {
		timer16::Timer3.handler();
	}
}
