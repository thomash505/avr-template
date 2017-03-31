//
// settings.h
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

#ifndef SETTINGS_H
#define SETTINGS_H


// Define structures for each setting group
#define X_SETTINGS(name, fields)		\
	typedef struct						\
	{									\
		fields							\
		/* Could add CRC16 here */		\
	} name##_T;
#define X_FLOAT(x,_) float x;
#define X_INT(x,_) int x;
#define X_STRUCT(type,x,...) type x;
#include "settings_def.h"


// Allow global access from modules which include this header
#define X_SETTINGS(name, fields)		\
	extern name##_T name;				\
	extern name##_T /*EEMEM*/ eeprom_##name;
#define X_FLOAT(x,_)
#define X_INT(x,_)
#define X_STRUCT(type,x,...)
#include "settings_def.h"

#ifdef __cplusplus
extern "C" {
#endif

void settings_save(void);
void settings_load(void);

#ifdef __cplusplus
}
#endif

#endif
