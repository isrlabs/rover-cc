#ifndef __HARDWARE_PWM_H
#define __HARDWARE_PWM_H


#include <stdint.h>


void	pwm_connect(uint8_t which, uint8_t pin);
void	pwm_trim(uint8_t which, uint16_t trim);
void	pwm_set_servo(uint8_t which, uint32_t us);

#endif
