extern "C" {
#include <avr/io.h>
}

#include "hardware/drivetrain.h"
#include "hardware/pwm.h"


#define MOTOR_RIGHT	1
#define MOTOR_LEFT	2
#define PORT_RIGHT	PB5
#define PORT_LEFT	PB6

#define ROTATE_FORWARD	1700
#define ROTATE_BACKWARD	1300
#define ROTATE_STOP	1500


void
drivetrain::init()
{
	DDRB |= _BV(PORT_RIGHT) | _BV(PORT_LEFT);

	pwm::connect(MOTOR_RIGHT, PORT_RIGHT);
	pwm::connect(MOTOR_LEFT, PORT_LEFT);

	pwm::set_limits(MOTOR_RIGHT, ROTATE_BACKWARD, ROTATE_FORWARD);
	pwm::set_limits(MOTOR_LEFT, ROTATE_BACKWARD, ROTATE_FORWARD);

	drivetrain::stop();
}


void
drivetrain::forward()
{
	pwm::set_servo(MOTOR_RIGHT, ROTATE_BACKWARD);
	pwm::set_servo(MOTOR_LEFT, ROTATE_FORWARD);
}


void
drivetrain::stop()
{
	pwm::set_servo(MOTOR_RIGHT, ROTATE_STOP);
	pwm::set_servo(MOTOR_LEFT, ROTATE_STOP);
}


void
drivetrain::backward()
{
	pwm::set_servo(MOTOR_RIGHT, ROTATE_FORWARD);
	pwm::set_servo(MOTOR_LEFT, ROTATE_BACKWARD);
}


void
drivetrain::rotate_left()
{
	pwm::set_servo(MOTOR_RIGHT, ROTATE_BACKWARD);
	pwm::set_servo(MOTOR_LEFT, ROTATE_BACKWARD);
}


void
drivetrain::rotate_right()
{
	pwm::set_servo(MOTOR_RIGHT, ROTATE_BACKWARD);
	pwm::set_servo(MOTOR_LEFT, ROTATE_FORWARD);
}

