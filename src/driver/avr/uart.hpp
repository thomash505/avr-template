//
// uart.hpp
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

#ifndef DRIVER_AVR_UART_HPP
#define DRIVER_AVR_UART_HPP

extern "C"
{
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <inttypes.h>
}

#include "utility/CircularBuffer.hpp"
#include "utility/scheduler/task.h"

namespace avr {

// MCU tags
struct MCU_ATmega32;
struct MCU_ATmega644P;
struct MCU_ATmega2560;
struct MCU_ATmega328P;

#if defined(__AVR_ATmega32__)
typedef MCU_ATmega32 MCU_T;
#elif defined(__AVR_ATmega644P__)
typedef MCU_ATmega644P MCU_T;
#elif defined(__AVR_ATmega2560__)
typedef MCU_ATmega2560 MCU_T;
#elif defined(__AVR_ATmega328P__)
typedef MCU_ATmega328P MCU_T;
#endif


template <typename MCU = MCU_T>
struct UARTTraits
{
	enum {RX = 32, TX = 64};
	enum {BAUDTOL = 2};
};

template <typename MCU = MCU_T>
class UARTBase
{
public:
	// ...
protected:
	typedef volatile uint8_t & sfr8_t;
	typedef MCU MCUType;
	
	UARTBase(sfr8_t _ubrrh, sfr8_t _ubrrl, sfr8_t _ucsra, sfr8_t _ucsrb, sfr8_t _ucsrc, sfr8_t _udr, uint8_t _u2xMaskA, uint8_t _enableMaskB, uint8_t _enableMaskC, uint8_t _udreMaskB)
		: _ubrrh(_ubrrh), _ubrrl(_ubrrl), _ucsra(_ucsra), _ucsrb(_ucsrb), _ucsrc(_ucsrc), _udr(_udr), _u2xMaskA(_u2xMaskA), _enableMaskB(_enableMaskB), _enableMaskC(_enableMaskC), _udreMaskB(_udreMaskB)
	{ }
	
	enum {BAUDTOL = UARTTraits<MCU>::BAUDTOL};		// Baud rate tolerance (%)
	
	void _enable(uint32_t baudRate)
	{
		bool u2x = false;
		
		uint32_t ubrr = (F_CPU + baudRate*8UL) / (baudRate*16UL) - 1;
		
		if ((100*F_CPU > (16*(ubrr + 1)) * (100*baudRate + baudRate*BAUDTOL)) ||
			(100*F_CPU < (16*(ubrr + 1)) * (100*baudRate - baudRate*BAUDTOL)))
		{
			u2x = true;
			ubrr = (F_CPU + baudRate*4UL) / (baudRate*8UL) - 1;
		}
		
		_ubrrh = (uint8_t)(ubrr << 8);
		_ubrrl = (uint8_t)ubrr;
		_ucsra = u2x ? _u2xMaskA : 0;
		_ucsrb |= _enableMaskB;
		_ucsrc |= _enableMaskC;
	}
	
	void _disable()
	{
		_ucsrb &= ~_enableMaskB;
		_ucsrc &= ~_enableMaskC;
	}
	
	bool _available()
	{
		return !_rxBuffer.isEmpty();
	}

	uint8_t _read()
	{
		return _rxBuffer.dequeue();
	}

	void _recvISR()
	{
//		_rxBuffer.queue(_udr);
		// TEST: No effect
		uint8_t c = _udr;
		_rxBuffer.queue(c);
	}
	
	void _write(uint8_t c)
	{
		// Block until we have room in the TX buffer
		while (_txBuffer.isFull()) { task_yield(); }
		
		// Queue character into TX buffer
		_txBuffer.queue(c);
		
		// Enable TX ready interrupt
		_ucsrb |= _udreMaskB;
	}
	
	void _xmitISR()
	{
		if (_txBuffer.isEmpty())
		{
			// Disable TX ready interrupt so we don't immediately retrigger
			_ucsrb &= ~_udreMaskB;
		}
		else
		{
			// Send next byte from TX buffer
			_udr = _txBuffer.dequeue();
		}
	}

	void _flush()
	{
		while (_txBuffer.isEmpty());
	}
private:
	enum {RXSIZE = UARTTraits<MCU>::RX, TXSIZE = UARTTraits<MCU>::TX};
    utility::CircularBuffer<RXSIZE,uint8_t> _rxBuffer;
    utility::CircularBuffer<TXSIZE,uint8_t> _txBuffer;
	
	// Special function registers
	sfr8_t _ubrrh, _ubrrl;
	sfr8_t _ucsra, _ucsrb, _ucsrc;
	sfr8_t _udr;
	
	// Feature masks
	const uint8_t _u2xMaskA;
	const uint8_t _enableMaskB, _enableMaskC;
	const uint8_t _udreMaskB;
	// ...
};

// Forward declaration
template <int CH = 0>
class UART;

// Helper class to hold a UART instance per channel
template <int CH, typename MCU = MCU_T>
struct UARTInstance;

// Partial specialisation for all MCUs which define UBRRH or UBRR0H
#if defined(UBRRH)
template <typename MCU>
struct UARTInstance<0,MCU>
{
	static UART<0> & instance()
	{
		static UART<0> _instance(UBRRH, UBRRL, UCSRA, UCSRB, UCSRC, UDR, (1<<U2X), (1<<TXEN) | (1<<RXEN) | (1 << RXCIE), (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0), (1<<UDRIE));
		return _instance;
	}
};
#elif defined(UBRR0H)
template <typename MCU>
struct UARTInstance<0,MCU>
{
	static UART<0> & instance()
	{
		static UART<0> _instance(UBRR0H, UBRR0L, UCSR0A, UCSR0B, UCSR0C, UDR0, (1<<U2X0), (1<<TXEN0) | (1<<RXEN0) | (1 << RXCIE0), (1<<UCSZ01) | (1<<UCSZ00), (1<<UDRIE0));
		return _instance;
	}
};
#endif

// Partial specialisation for all MCUs which define UBRR1H
#if defined(UBRR1H)
template <typename MCU>
struct UARTInstance<1,MCU>
{
	static UART<1> & instance()
	{
		static UART<1> _instance(UBRR1H, UBRR1L, UCSR1A, UCSR1B, UCSR1C, UDR1, (1<<U2X1), (1<<TXEN1) | (1<<RXEN1) | (1 << RXCIE1), (1<<UCSZ11) | (1<<UCSZ10), (1<<UDRIE1));
		return _instance;
	}
};
#endif

// Partial specialisation for all MCUs which define UBRR2H
#if defined(UBRR2H)
template <typename MCU>
struct UARTInstance<2,MCU>
{
	static UART<2> & instance()
	{
		static UART<2> _instance(UBRR2H, UBRR2L, UCSR2A, UCSR2B, UCSR2C, UDR2, (1<<U2X2), (1<<TXEN2) | (1<<RXEN2) | (1 << RXCIE2), (1<<UCSZ21) | (1<<UCSZ20), (1<<UDRIE2));
		return _instance;
	}
};
#endif

// Partial specialisation for all MCUs which define UBRR3H
#if defined(UBRR3H)
template <typename MCU>
struct UARTInstance<3,MCU>
{
	static UART<3> & instance()
	{
		static UART<3> _instance(UBRR3H, UBRR3L, UCSR3A, UCSR3B, UCSR3C, UDR3, (1<<U2X3), (1<<TXEN3) | (1<<RXEN3) | (1 << RXCIE3), (1<<UCSZ31) | (1<<UCSZ30), (1<<UDRIE3));
		return _instance;
	}
};
#endif

// Complete specialisations on MCU, CH
// ...

// Wrapper for accessing UART objects
template <int CH>
class UART : public UARTBase<MCU_T>
{
public:
	inline static void enable(uint32_t baudRate)	{_instance()._enable(baudRate);}
	inline static void disable()					{_instance()._disable();}
	inline static bool available()					{return _instance()._available();}
	inline static uint8_t read()					{return _instance()._read();}
	inline static void write(uint8_t c)				{_instance()._write(c);}
	inline static void write(const char *str)
	{
		for (int i = 0; str[i] != '\0'; ++i)
		{
			_instance()._write(str[i]);
		}
	}

	inline static void write(const uint8_t* buf, int n)
	{
		for (int i = 0; i < n; ++i)
		{
			_instance()._write(buf[i]);
		}
	}

	inline static void flush()						{_instance()._flush();}
	inline static void RXC_ISR()					{_instance()._recvISR();}
	inline static void UDRE_ISR()					{_instance()._xmitISR();}
private:
	friend class UARTInstance<CH>;
//	typedef volatile uint8_t & sfr8_t; // temporary
	UART(sfr8_t _ubrrh, sfr8_t _ubrrl, sfr8_t _ucsra, sfr8_t _ucsrb, sfr8_t _ucsrc, sfr8_t _udr, uint8_t _u2xMaskA, uint8_t _enableMaskB, uint8_t _enableMaskC, uint8_t _udreMaskB)
		: UARTBase(_ubrrh, _ubrrl, _ucsra, _ucsrb, _ucsrc, _udr, _u2xMaskA, _enableMaskB, _enableMaskC, _udreMaskB)
	{ }
	
	inline static UART & _instance()				{return UARTInstance<CH,MCUType>::instance();}
};


// RXC interrupt vector for UART0
#if defined(USART_RXC_vect)
#define UART0_RXC_IV USART_RXC_vect
#elif defined(USART0_RX_vect)
#define UART0_RXC_IV USART0_RX_vect
#elif defined(USART_RX_vect)
#define UART0_RXC_IV USART_RX_vect
#endif

// UDRE interrupt vector for UART0
#if defined(USART_UDRE_vect)
#define UART0_UDRE_IV USART_UDRE_vect
#elif defined(USART0_UDRE_vect)
#define UART0_UDRE_IV USART0_UDRE_vect
#endif

// RXC interrupt vector for UART1
#if defined(USART1_RX_vect)
#define UART1_RXC_IV USART1_RX_vect
#endif

// UDRE interrupt vector for UART1
#if defined(USART1_UDRE_vect)
#define UART1_UDRE_IV USART1_UDRE_vect
#endif

// RXC interrupt vector for UART2
#if defined(USART2_RX_vect)
#define UART2_RXC_IV USART2_RX_vect
#endif

// UDRE interrupt vector for UART2
#if defined(USART2_UDRE_vect)
#define UART2_UDRE_IV USART2_UDRE_vect
#endif


}

#endif // DRIVER_AVR_UART_HPP
