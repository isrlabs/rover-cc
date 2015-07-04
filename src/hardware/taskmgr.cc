#include "hardware/taskmgr.h"
#include "hardware/timer16.h"


static void
run(void)
{
	struct s_task	*task = tasks;

	if (NULL != task) {
		task->run();
		task = task->next;
	}
}

void
taskmgr::init(void)
{
	Timer16::Timer3.handler = run;
}


void
taskmgr::register(struct s_task *task)
{
	struct s_task	*cur = tasks;

	if (tasks == NULL) {
		tasks = task;
	}
	else {
		while (cur->next != NULL) {
			cur = cur->next;
		}
		cur->next = task;
		task->next = NULL;
	}
}
