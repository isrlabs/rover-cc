extern "C" {
#include <stdint.h>
}


namespace timer16 {
	enum select {
		TIMER1 = 1,
		TIMER3 = 2
	};

	enum mode {
		NORMAL = 0,
		CTC = (1 << 4)
	};

	enum prescale {
		PRESCALE0 = 1,
		PRESCALE8 = 2
	};
}


class Timer16 {
	public:
	// The default constructor sets up the timer in normal
	// mode with a prescaler of 8.
	Timer16(const enum timer16::select);

	void set_mode(const enum timer16::mode);
	void set_prescale(const enum timer16::prescale);
	void clear_interrupts(void);
	void reset(void);

	void (handler)(void);

	private:
	const uint8_t	baseptr;
	const uint8_t	timer;
};


namespace timer16 {
	Timer16 Timer1(TIMER1);
	Timer16 Timer3(TIMER3);

	void
	init(void)
	{
		Timer1.reset();

		Timer3.set_mode(CTC);
		Timer3.reset();
	}
}
