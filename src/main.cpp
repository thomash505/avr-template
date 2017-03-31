//
// main.cpp
//
// Copyright (C) 2009-2012 Chris Renton <crenton@me.com>
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

#include "driver/avr/uart.hpp"
#include "driver/avr/uart.h"
#include "settings.h"
#include "cmd_line.h"
#include "utility/scheduler/task.h"
#include "application.h"

static void uart_task(void* args)
{
    // Maximum number of characters to accept on one line
    #define LINE_BUFFER_SIZE 80

    static char linebuf[LINE_BUFFER_SIZE] = "";
    static int count = 0;

    // Transfer data from uart buffer to line buffer
    while (uart_avail())
    {
        char c = uart_getc();

        if (c == '\b')			// Handle backspace
        {
            if (count > 0)
            {
                count--;
            }
        }
        else if (c != '\r')		// Ignore carriage return
        {
            if (count < LINE_BUFFER_SIZE)
            {
                linebuf[count++] = c;
            }
            else
            {
                count = 0;
                printf_P(PSTR("\nMax line length exceeded\n"));
            }
        }

        // Process command if line buffer is terminated by a line feed
        if (count > 0 && linebuf[count-1] == '\n')
        {
            linebuf[count-1] = '\0';		// terminate the string

            // process terminated command line
            cmd_process(linebuf);

            // reset linebuf
            count = 0;
            linebuf[count] = '\0';
        }
    }
}

int main(void)
{
    static TASK_T uart;

    task_init();
    uart_init();
    systime_init();

    sei();

    settings_load();    

    uart.func = uart_task;
    uart.interval = ms2systime(5);
	uart.priority = TASK_PRIORITY_MIN;
    uart.count = -1;
    task_enqueue(&uart, NULL);

    application::init();

    while(1)
    {
        task_yield();
    }

    return 0;
}
