#ifndef __HARDWARE_PWM_H
#define __HARDWARE_PWM_H


#include <stdint.h>


namespace pwm {

	void		connect(uint8_t which, uint8_t pin);
	void		trim(uint8_t which, int16_t trim);
	void		set_limits(uint8_t which, uint16_t min, uint16_t max);
	void		set_servo(uint8_t which, uint32_t us);
	uint16_t	get_servo(uint8_t which);

}


#endif
