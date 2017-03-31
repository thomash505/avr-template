//
// task.h
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

#ifndef TASK_H
	#define	TASK_H

    #include <stdint.h>
    #include <stdbool.h>
    #include "utility/time.h"
	
	/**
	 *	Task Scheduler Header File.
	 *	@file		task.h
	 * 	@author		Thomas Hudson
	 *	@version	1.0
	 *	@date		8/9/2012
	 *
	 * 	This header file defines an interface to the "task.c" module.
	 *			
     */

	/**
	 * 	Typedef containing a single task.
	 *
	 * 	This typedef holds all necessary information to call an
	 *	task at a specified time.
	 */

	#ifdef __cplusplus
	extern "C" {
	#endif

	#define TASK_PRIORITY_MAX						255
	#define TASK_PRIORITY_MIN						1
	#define TASK_PRIORITY_DEFAULT					127
	 
	typedef volatile struct TASK_S
	{
		volatile struct TASK_S *next;									//!< Pointer to the next task
        void (*func) (void* args);								//!< Pointer to a function that is executed when the task is scheduled
        void* args;                                             //!< Arguments to pass to thread function
        systime_t interval;										//!< Holds the number of ticks between task calls
		uint8_t priority;										//!< Holds the priority
        systime_t time;									//!< Holds the time for the next task call
		volatile int8_t count;									//!< Holds the number of times to execute task
	} TASK_T;
	
	/***** Function Prototypes *****/
    void task_init(void);										//!< Initialises and starts the SysTick timer
    void task_yield(void);										//!< Scans through all tasks and calls appropriate functions
	void task_enqueue(TASK_T*, void*);                          //!< Enqueues the specified task to the task queue
	bool task_dequeue(TASK_T*);									//!< Dequeues the specified task from the task queue
    void task_print(void);

	#define TASK_CREATE(NAME, FUNC, PRIORITY, 					\
                        INTERVAL, COUNT)                        \
	static TASK_T NAME 											\
		= 	{													\
				.func = FUNC,									\
				.time = 0,										\
				.priority = PRIORITY,							\
				.interval = INTERVAL,							\
				.count = COUNT,									\
			}

	#ifdef __cplusplus
	}
	#endif
	
#endif
