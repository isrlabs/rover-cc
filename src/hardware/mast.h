#ifndef __HARDWARE_MAST_H
#define __HARDWARE_MAST_H


namespace mast {

	struct	USR_reading {
		uint8_t		angle;
		uint16_t	distance;
	};

	enum E_PING_STATE {
		PING_FIRE_START,// Initiate echo pulse.
		PING_FIRE_STOP, // Stop echo pulse.
		PING_READ_IN,	// Waiting for return pulse.
		PING_READ_OUT,	// Pulse has returned, disable timer and read dist.
		PING_STOPPED
	};

	bool		check_state(E_PING_STATE);

	void		init(void);
	void		collision_indicator(bool on);
	void		self_test(void);
	void		next(void);
	struct USR_reading	last_distance(void);
	uint8_t		mast_angle(void);
	E_PING_STATE	state(void);

}


#endif
