//
// time.h
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

#ifndef UTILITY_TIME_H
#define UTILITY_TIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#define timer_overflow_Hz   1000
#define CS00_VALUE			3			// Prescaler of 64



#define systime2ms(t)		((uint32_t) (t)/(timer_overflow_Hz/1000))
#define ms2systime(t)		((uint32_t) (t)*(timer_overflow_Hz/1000))

typedef volatile uint32_t systime_t;

void systime_init(void);
systime_t systime_now(void);
systime_t systime_millis(void);
void systime_sleep_ms(int);

#ifdef __cplusplus
}
#endif

#endif // UTILITY_TIME_H
