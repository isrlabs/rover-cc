// Note: the caller needs to ensure Timer1 has been initialised.

#ifndef __HARDWARE_DRIVETRAIN_H
#define __HARDWARE_DRIVETRAIN_H

namespace drivetrain {

	void	init(void);
	void	forward(void);
	void	backward(void);
	void	stop(void);
	void	rotate_left(void);
	void	rotate_right(void);

}

#endif
