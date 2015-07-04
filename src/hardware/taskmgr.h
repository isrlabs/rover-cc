// Task Manager runs through each of its tasks every tick of the clock.
// This is not a real-time task manager. The package uses timer3.

#ifndef __TASK_MGR_H
#define __TASK_MGR_H

#include "hardware/timer16.h"


namespace taskmgr {
struct s_task {
	// id contains the task identifier.
	uint8_t	id;

	// run contains the code that the task should run on
	// execution. This function should track its own state
	// and decide whether any action needs to be taken.
	(void task)(void);

	// next is set up to point to the next task. The list
	// is terminated by a NULL next.
	struct s_task	*next;
};


// tasks contains the list of registered tasks.
static struct s_task	*tasks = NULL;

// register adds a task to the task list.
void	register(struct s_task *);

// init sets up the task manager and starts it.
void	init(void);
}


#endif
