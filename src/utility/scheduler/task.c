//
// task.c
//
// Copyright (C) 2017-2020 Thomas Hudson <thomash505@gmail.com>
//
// This is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// This is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// this software. If not, see <http://www.gnu.org/licenses/>.
//

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#include "task.h"

static void task_execute(TASK_T *exec, const uint8_t);
static TASK_T *task_start = NULL;

void task_init(){}

void task_yield (void)
{
	uint8_t highest_priority = 0;
    TASK_T* exec = NULL;

    for (TASK_T *current = task_start; current != NULL; current = current->next)
	{
        // Loop through all tasks
        if ((int16_t) (systime_now() - current->time) >= 0)
        {
            uint8_t priority = current->priority + systime_now() - current->time;

			if (current->count && (priority >= highest_priority))
            {
                exec = current;
				highest_priority = priority;
            }
		}
    }

	task_execute(exec, highest_priority);
}

void task_execute(TASK_T* exec, const uint8_t priority)
{
    // If the task is scheduled to execute
	if (exec != NULL)
    {
        // Set the conditions for the next call
        exec->time = systime_now() + exec->interval;

        // Save scheduler information
        int16_t count = exec->count;
        exec->count = 0;

        // Execute task
        exec->func(exec->args);

        // Restore scheduler information
        exec->count = count;

        // Update task counter
        if (exec->count > 0)
            exec->count--;
    }
}

void task_enqueue(TASK_T *task, void* args)
{
    task->args = args;
    task->time = systime_now();
    task->next = task_start;
    task_start = task;
}

bool task_dequeue(TASK_T *task)
{
	if (task == task_start)
	{
		task_start = task_start->next;
		return true;
	}

	TASK_T* prev = task_start;
	for (TASK_T* current = task_start->next; current != NULL; current = current->next)
	{
		if (current == task)
		{
			// Task found, remove from the list
			prev->next = current->next;
			return true;
		}
	}

	return false;
}

void task_print()
{
	printf_P(PSTR("systime: %lu\n"), systime_now());
    for (TASK_T* current = task_start; current != NULL; current = current->next)
    {
		printf_P(PSTR("count: %i, interval: %lu, time: %lu, priority: %u\n"), current->count, current->interval, current->time, current->priority);
    }
}
