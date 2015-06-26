#ifndef __HARDWARE_MAST_H
#define __HARDWARE_MAST_H


namespace mast {

	struct reading {
		uint8_t		angle;
		int16_t		distance;
	};

	// init performs the initial hardware set up.
	void	init(void);

	// collision_indicator turns on or off the collision indication
	// lamp.
	void	collision_indicator(bool on);

	// self_test causes the mast to step through each of its
	// positions to ensure the mast servo is working.
	// TODO: add USR calibration as appropriate.
	void	self_test(void);

	// next steps the mast to the next position.
	void	next(void);

	// This returns the current mast angle; it is a value between 0
	// and 180.
	uint8_t	mast_angle(void);

}


#endif
