//
// settings.c
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

#include "settings.h"
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

// Working copy of settings (RAM)
//SETTINGS_T settings;
#define X_SETTINGS(name, fields)		\
	name##_T name;
#define X_FLOAT(x,_)
#define X_INT(x,_)
#define X_STRUCT(type,x,...)
#include "settings_def.h"

// Nonvolatile copy of settings (EEPROM)
// Default values will be saved to .EEP file
#define X_SETTINGS(name, fields)		\
	name##_T EEMEM eeprom_##name = {	\
		fields							\
	};
#define X_FLOAT(x,y) .x = y,
#define X_INT(x,y) .x = y,
#define X_STRUCT(type,x,...) .x = __VA_ARGS__,
#include "settings_def.h"


void settings_save(void)
{
	#define X_SETTINGS(name, fields)	\
		eeprom_update_block(&name, &eeprom_##name, sizeof(name##_T));
	#define X_FLOAT(x,_)
	#define X_INT(x,_)
	#define X_STRUCT(type,x,...)
	#include "settings_def.h"
}

void settings_load(void)
{
	#define X_SETTINGS(name, fields)	\
		eeprom_read_block(&name, &eeprom_##name, sizeof(name##_T));
	#define X_FLOAT(x,_)
	#define X_INT(x,_)
	#define X_STRUCT(type,x,...)
	#include "settings_def.h"
}
