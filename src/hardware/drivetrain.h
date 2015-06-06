// Note: the caller needs to ensure Timer1 has been initialised.

#ifndef __HARDWARE_DRIVETRAIN_H
#define __HARDWARE_DRIVETRAIN_H


void	drivetrain_init(void);
void	drivetrain_forward(void);
void	drivetrain_backward(void);
void	drivetrain_stop(void);
void	drivetrain_rotate_left(void);
void	drivetrain_rotate_right(void);


#endif
