//
// cmd_line.c
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

#include "cmd_line.h"

#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <ctype.h>
#include "utility/scheduler/task.h"

// Helper functions and built-in commands
int makeargv(char *string, char *argv[], int argvsize);
void cmd_help(int argc, char *argv[]);

// User supplied commands
// Settings
#include "settings.h"
#include <stdlib.h>
void cmd_save(int argc, char *argv[]);
void cmd_load(int argc, char *argv[]);
void cmd_list(int argc, char *argv[]);
void cmd_set(int argc, char *argv[]);
void cmd_top(int argc, char* argv[]) { task_print(); }

#define CMD_TABLE															\
	X(help,"Displays this command list")									\
	X(about,"Displays information about this application")					\
	X(save,"Save settings to EEPROM")										\
	X(load,"Load settings from EEPROM")										\
	X(list,"List current settings")											\
	X(set,"Configure a setting")                                            \
	X(top,"Displays all tasks")

// Store strings in program memory
#define X(name,help) 														\
	static const char cmd_##name##_cstr[] PROGMEM = #name;					\
	static const char cmd_##name##_hstr[] PROGMEM = help;
CMD_TABLE
#undef X

// Initialise table
#define X(name,help) {cmd_##name##_cstr,cmd_##name,cmd_##name##_hstr},
static CMD_T cmd_table[] =
{
	CMD_TABLE
};
#undef X

enum {NUM_CMDS = sizeof(cmd_table)/sizeof(CMD_T)};
enum {MAX_TOKENS = 10};		// Maximum number of tokens to process (command + arguments)


void cmd_process(char *string)
{
	char *argv[MAX_TOKENS];
	int argc = makeargv(string, argv, MAX_TOKENS);
	for (int i = 0; i < NUM_CMDS; i++)
	{
		if (strcmp_P(argv[0], cmd_table[i].cmd)==0)
		{
			cmd_table[i].func(argc - 1, &argv[1]);
			return;
		}
	}

	// Command not found
    if (string[0])
	{
		printf_P(PSTR("Unknown command: \"%s\"\n"),argv[0]);
		if (argc > 1)
		{
			printf_P(PSTR("    with arguments:\n"));
			for (int i = 1; i < argc; i++)
			{
				printf_P(PSTR("        %d: %s\n"),i,argv[i]);
			}
		}
		printf_P(PSTR("\nEnter \"help\" for a list of commands.\n\n"));
	}
}

#define xstr(s) str(s)
#define str(s) #s
#ifndef MAJ_VERSION
	#define MAJ_VERSION 1
#endif
#ifndef MIN_VERSION
	#define MIN_VERSION 0
#endif

#ifdef BUILD_NUM
	#define BUILD_STR "." xstr(BUILD_NUM)
#else
	#define BUILD_STR
#endif
void cmd_about(int argc, char *argv[])
{
	printf_P(PSTR(
		"\n\nAVR serial interpreter v" xstr(MAJ_VERSION) "." xstr(MIN_VERSION) BUILD_STR ". Copyright (C) 2009-2012 Chris Renton.\n"
		"Last built on " __DATE__ " at " __TIME__ ".\n"
		"\n"
	));
}

void cmd_help(int argc, char *argv[])
{
	switch (argc)
	{
	case 0:
		printf_P(PSTR(
			"Command        Description\n"
			"--------------------------\n"
		));
		for (int i = 0; i < NUM_CMDS; i++)
		{
			printf_P(PSTR("%-14S %S\n"),cmd_table[i].cmd,cmd_table[i].help);
		}
		printf_P(PSTR("\nFor more information, enter help followed by the command name\n\n"));
		break;
	case 1:
		printf_P(PSTR("To be implemented...\n\n"));
		// TODO: Scan command table, and lookup extended help string.
		break;
	default:
		printf_P(PSTR("help is expecting zero or one argument.\n\n"));
	}
}

void cmd_save(int argc, char *argv[])
{
	settings_save();
	printf_P(PSTR("Saved settings to EEPROM\n\n"));
}

void cmd_load(int argc, char *argv[])
{
	settings_load();
	printf_P(PSTR("Loaded settings from EEPROM\n\n"));
}

#include <stddef.h>
void print_memory_map(void *p, size_t n, void *addr)
{
	const size_t m = 16;		// Number of columns
	const size_t q = n/m;		// Full rows
	const size_t r = n%m;		// Elements on last row

	// Header
	printf_P(PSTR("Addr"));
	for (int j = 0; j < m; j++)
	{
		printf_P(PSTR(" %02X"), j);
	}
	printf_P(PSTR("\n"));
	// Print full rows
	int i = 0;
	for (; i < q; i++)
	{
		printf_P(PSTR("%04X"), (char *)addr + m*i);
		for (int j = 0; j < m; j++)
		{
			printf_P(PSTR(" %02X"), ((char *)p)[m*i + j]);
		}
		printf_P(PSTR("\n"));
	}
	// Print remainder
	printf_P(PSTR("%04X"), (char *)addr + m*i);
	for (int j = 0; j < r; j++)
	{
		printf_P(PSTR(" %02X"), ((char *)p)[m*i + j]);
	}
	printf_P(PSTR("\n"));
}

void cmd_list(int argc, char *argv[])
{
	#if 1
		#define PRINT_RAW_BYTES(name)										\
			printf_P(PSTR("\n"));											\
			print_memory_map(&name, sizeof(name), &eeprom_##name);			\
			printf_P(PSTR("\n"));
	#else
		#define PRINT_RAW_BYTES(name)
	#endif

	#define X_SETTINGS(name, fields)										\
		printf_P(PSTR("Settings: %S\n"), PSTR(#name));						\
		PRINT_RAW_BYTES(name)												\
		printf_P(PSTR(														\
			"Variable       Value      \n"									\
			"--------------------------\n"									\
		));																	\
		{	/* create local scope for pointer */							\
			name##_T *p = &name;											\
			fields															\
		}																	\
		printf_P(PSTR("\n"));
	#define X_FLOAT(x,_) printf_P(PSTR("%-14S %g\n"), PSTR(#x), p->x);
	#define X_INT(x,_) printf_P(PSTR("%-14S %d\n"), PSTR(#x), p->x);
	#define X_STRUCT(type,x,...) printf_P(PSTR("%-14S <struct>\n"), PSTR(#x));
	#include "settings_def.h"
}

void cmd_set(int argc, char *argv[])
{
	switch (argc)
	{
	case 2:
		#define X_SETTINGS(name, fields)									\
			{	/* create local scope for pointer */						\
				name##_T *p = &name;										\
				fields { }													\
			}
		#define X_FLOAT(x,_)												\
			if (strcmp_P(argv[0],PSTR(#x))==0)								\
			{																\
				p->x = atof(argv[1]);										\
				printf_P(PSTR("Variable %s set to %g\n\n"),argv[0],p->x);	\
			} else
		#define X_INT(x,_)													\
			if (strcmp_P(argv[0],PSTR(#x))==0)								\
			{																\
				p->x = atoi(argv[1]);										\
				printf_P(PSTR("Variable %s set to %i\n\n"),argv[0],p->x);	\
			} else
		#define X_STRUCT(type,x,...)
		#include "settings_def.h"
		break;
	default:
		printf_P(PSTR("set command expecting 2 arguments\n\n"));
	}
}

int makeargv(char *string, char *argv[], int argvsize)
{
	char *p = string;
	int argc = 0;

	for(int i = 0; i < argvsize; i++) {
		// skip leading whitespace
		while (isspace(*p))
			p++;

		if(*p != '\0')
			argv[argc++] = p;
		else
		{
			argv[argc] = 0;
			break;
		}

		// scan over arg
		while(*p != '\0' && !isspace(*p))
			p++;

		// terminate arg
		if(*p != '\0' && i < argvsize-1)
			*p++ = '\0';
	}

	return argc;
}
