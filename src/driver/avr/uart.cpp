//
// uart.cpp
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

#include "uart.h"

extern "C"
{
	#include <stdio.h>
	#include <avr/interrupt.h>
}

#include "uart.hpp"

// Note: This is initialised in uart_stream.c
extern FILE uart_str;

using namespace avr;

#ifdef UART0_RXC_IV
ISR(UART0_RXC_IV) {UART<0>::RXC_ISR();}
#endif

#ifdef UART0_UDRE_IV
ISR(UART0_UDRE_IV) {UART<0>::UDRE_ISR();}
#endif

#ifdef UART1_RXC_IV
ISR(UART1_RXC_IV) {UART<1>::RXC_ISR();}
#endif

#ifdef UART1_UDRE_IV
ISR(UART1_UDRE_IV) {UART<1>::UDRE_ISR();}
#endif

#ifdef UART2_RXC_IV
ISR(UART2_RXC_IV) {UART<2>::RXC_ISR();}
#endif

#ifdef UART2_UDRE_IV
ISR(UART2_UDRE_IV) {UART<2>::UDRE_ISR();}
#endif


// C linkable functions (see uart.h)

void uart_putc(char c)
{
	UART<0>::write(c);
}

int uart_printchar(char c, FILE *stream)
{
	UART<0>::write(c);
	return 0;
}

void uart_init(void)
{
    UART<0>::enable(115200);
	
	// Redirect stdout, so that printf() knows where to go
	stdout = &uart_str;
	stderr = &uart_str;		// For assert, etc.
}

int uart_avail(void)
{
	return UART<0>::available();
}

char uart_getc(void)
{
	return UART<0>::read();
}
