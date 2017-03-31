//
// uart.h
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

#ifndef DRIVER_AVR_UART_H
#define DRIVER_AVR_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>
#include <stdio.h>
#include <ctype.h> 

//#define UART_BAUD_RATE 115200
//
////#define UART_DOUBLESPEED
//
//#ifdef UART_DOUBLESPEED
//#define UART_CALC_BAUDRATE(baudRate) ((uint32_t)((F_CPU) + ((uint32_t)baudRate * 4UL)) / ((uint32_t)(baudRate) * 8UL) - 1)
//#else
//#define UART_CALC_BAUDRATE(baudRate) ((uint32_t)((F_CPU) + ((uint32_t)baudRate * 8UL)) / ((uint32_t)(baudRate) * 16UL) - 1)
//#endif
//
//#if defined(__AVR_ATmega32__)
//	#define UART_BAUD_HIGH		UBRRH
//	#define UART_BAUD_LOW		UBRRL
//	#define UART_STATUS			UCSRA
//	#define UART_TXREADY		UDRE
//	#define UART_TX_IRQ			UDRIE
//	#define UART_RXREADY		RXC
//	#define UART_DOUBLE			U2X
//	#define UART_CTRL			UCSRB
//	#define UART_CTRL_DATA		((1<<TXEN) | (1<<RXEN) | (1 << RXCIE))
//	#define UART_CTRL2			UCSRC
//	#define UART_CTRL2_DATA		((1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0))
//	#define UART_DATA			UDR
//	#define UART_VECT_RXC		USART_RXC_vect
//	#define UART_VECT_UDRE		USART_UDRE_vect
//#elif defined(__AVR_ATmega644P__)
//	#ifndef UART_USE_SECOND
//		// Use UART0
//		#define UART_BAUD_HIGH	UBRR0H
//		#define UART_BAUD_LOW	UBRR0L
//		#define UART_STATUS	    UCSR0A
//		#define UART_TXREADY	UDRE0
//		#define UART_TX_IRQ		UDRIE0
//		#define UART_RXREADY	RXC0
//		#define UART_DOUBLE	    U2X0
//		#define UART_CTRL	    UCSR0B
//		#define UART_CTRL_DATA	((1<<TXEN0) | (1<<RXEN0) | (1 << RXCIE0))
//		#define UART_CTRL2	    UCSR0C
//		#define UART_CTRL2_DATA	( (1<<UCSZ01) | (1<<UCSZ00))
//		#define UART_DATA	    UDR0
//		#define UART_VECT_RXC	USART0_RXC_vect
//		#define UART_VECT_UDRE	USART0_UDRE_vect
//	#else
//		// Use UART1
//		#define UART_BAUD_HIGH	UBRR1H
//		#define UART_BAUD_LOW	UBRR1L
//		#define UART_STATUS	    UCSR1A
//		#define UART_TXREADY	UDRE1
//		#define UART_TX_IRQ		UDRIE1	
//		#define UART_RXREADY	RXC1
//		#define UART_DOUBLE	    U2X1
//		#define UART_CTRL	    UCSR1B
//		#define UART_CTRL_DATA	((1<<TXEN1) | (1<<RXEN1) | (1 << RXCIE1))
//		#define UART_CTRL2	    UCSR1C
//		#define UART_CTRL2_DATA	( (1<<UCSZ11) | (1<<UCSZ10))
//		#define UART_DATA	    UDR1
//		#define UART_VECT_RXC	USART1_RXC_vect
//		#define UART_VECT_UDRE	USART1_UDRE_vect
//	#endif
//#else
//	#error "UART not supported for this MCU"
//#endif
	
void uart_putc(char c);
int uart_printchar(char c, FILE *stream);
void uart_puts (char *s);
void uart_init(void);

int uart_avail(void);
char uart_getc(void);

#ifdef __cplusplus
}
#endif

#endif // DRIVER_AVR_UART_H
