extern "C" {
#include <avr/io.h>
}

#include "hardware/drivetrain.h"
#include "hardware/pwm.h"


#define MOTOR_RIGHT	0
#define MOTOR_LEFT	1
#define PORT_RIGHT	PB5
#define PORT_LEFT	PB6

#define ROTATE_FORWARD	1700
#define ROTATE_BACKWARD	1300
#define ROTATE_STOP	1500


void
drivetrain_init()
{
	DDRB |= _BV(PORT_RIGHT) | _BV(PORT_LEFT);

	pwm_connect(MOTOR_RIGHT, PORT_RIGHT);
	pwm_connect(MOTOR_LEFT, PORT_LEFT);
}


void
drivetrain_forward()
{
	pwm_set_servo(MOTOR_RIGHT, ROTATE_BACKWARD);
	pwm_set_servo(MOTOR_LEFT, ROTATE_FORWARD);
}


void
drivetrain_stop()
{
	pwm_set_servo(MOTOR_RIGHT, ROTATE_STOP);
	pwm_set_servo(MOTOR_LEFT, ROTATE_STOP);
}


void
drivetrain_backward()
{
	pwm_set_servo(MOTOR_RIGHT, ROTATE_FORWARD);
	pwm_set_servo(MOTOR_LEFT, ROTATE_BACKWARD);
}


void
drivetrain_rotate_left()
{
	pwm_set_servo(MOTOR_RIGHT, ROTATE_BACKWARD);
	pwm_set_servo(MOTOR_LEFT, ROTATE_BACKWARD);
}


void
drivetrain_rotate_right()
{
	pwm_set_servo(MOTOR_RIGHT, ROTATE_BACKWARD);
	pwm_set_servo(MOTOR_LEFT, ROTATE_FORWARD);
}

