//
// time.c
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

#include "utility/time.h"
#include <avr/interrupt.h>
#include <util/delay.h>

static systime_t systime_count = 0;

ISR (TIMER0_COMPA_vect)
{
    systime_count++;
}

void systime_init (void)
{
    // Initialise Timer 0
    TCCR0A = (1 << WGM01);										// CTC timer mode
	TCCR0B = (CS00_VALUE << CS00);
    TIMSK0 |= (1 << OCIE0A);									// Enable CTC interrupt

    OCR0A = 250;
}

systime_t systime_now (void)
{
    return systime_count;
}

systime_t systime_millis(void)
{
    return systime2ms(systime_now());
}

void systime_sleep_ms(int ms)
{
    for(int i = 0; i < ms; ++i)
    {
        _delay_ms(1);
    }
}
