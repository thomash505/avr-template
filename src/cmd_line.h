//
// cmd_line.h
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

#ifndef CMD_LINE_H
#define CMD_LINE_H

	#include <avr/pgmspace.h>

	#ifdef __cplusplus
	extern "C" {
	#endif

	typedef struct {
		const char * cmd;
		void (*func)(int argc, char *argv[]);
		const char * help;
	} CMD_T;

	void cmd_process(char *string);
	void cmd_about(int argc, char *argv[]);

	typedef struct
	{
		const char * name;
		void * ptr;
		const char * fmt;
	} SETTINGS_TABLE_ENTRY_T;

	#ifdef __cplusplus
	}
	#endif

#endif
