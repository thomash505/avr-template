//
// spi.hpp
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

#ifndef DRIVER_AVR_SPI_HPP
#define DRIVER_AVR_SPI_HPP

#include "SPI.h"

extern "C"
{
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
}

namespace avr {

namespace SPIConfig {

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
}

template <typename MCU = SPIConfig::MCU_T>
class SPIBase
{
public:

    typedef MCU MCUType;

protected:
    typedef volatile uint8_t & sfr8_t;
    SPIBase(sfr8_t spdr,
            sfr8_t spcr,
            sfr8_t spsr,
            sfr8_t sreg,
            uint8_t spif,
            uint8_t mstr,
            uint8_t spe)
        : _spdr(spdr)
        , _spcr(spcr)
        , _spsr(spsr)
        , _sreg(sreg)
        , _spif(spif)
        , _mstr(mstr)
        , _spe(spe)
        , initialized(0)
    {}

    void begin()
    {
        uint8_t sfreg = _sreg;
        cli();
        if(!initialized)
        {
            // De-assert slave select and set as output
            _spcr = (_mstr | _spe);
        }
        ++initialized;
        _sreg = sfreg;
    }

    void end()
    {
        uint8_t sfreg = _sreg;
        cli();
        if (initialized)
            initialized--;
        if (!initialized) {
            _spcr &= ~_spe;
        }
        _sreg = sfreg;
    }

    uint8_t transfer(uint8_t data)
    {
        _spdr = data;
        asm volatile("nop");
        while (!(_spsr & _spif));
        return _spdr;
    }

    void setClockDivider(uint8_t clockDiv)
    {
        _spcr = (_spcr & ~SPI_CLOCK_MASK) | (clockDiv & SPI_CLOCK_MASK);
        _spsr = (_spsr & ~SPI_2XCLOCK_MASK) | ((clockDiv >> 2) & SPI_2XCLOCK_MASK);
    }

private:
    // Special function registers
    sfr8_t _spdr, _spcr, _spsr;
    sfr8_t _sreg;

    // Feature masks
    const uint8_t _spif;
    const uint8_t _mstr;
    const uint8_t _spe;

    // Other
    uint8_t initialized;

};

template <int CH = 0>
class SPI;

template <int CH, typename MCU = SPIConfig::MCU_T>
struct SPIInstance;

#if defined(SPDR)
template <typename MCU>
struct SPIInstance<0,MCU>
{
    static SPI<0> & instance()
    {
        static SPI<0> _instance(SPDR, SPCR, SPSR, SREG, (1 << SPIF), (1 << MSTR), (1 << SPE));
        return _instance;
    }
};

#endif


template <int CH>
class SPI : public SPIBase<SPIConfig::MCU_T>
{
public:
    inline static void open()
    {
        instance().begin();
        setSpeedFast();
    }

    inline static void close()
    {
        instance().end();
    }

    inline static void write(uint8_t c)
    {
        instance().transfer(c);
    }

    inline static void write(const void* buf,
                            uint8_t n)
    {
        for (uint8_t i = 0; i < n; ++i)
        {
            instance().transfer(reinterpret_cast<const uint8_t*>(buf)[i]);
        }
    }

    inline static void read(void* buf,
                           uint8_t n)
    {
        for (uint8_t i = 0; i < n; ++i)
        {
            reinterpret_cast<uint8_t*>(buf)[i] = instance().transfer(0x00);
        }
    }

    inline static void setSpeedFast()
    {
        instance().setClockDivider(SPI_CLOCK_DIV4);
    }

    inline static void setSpeedSlow()
    {
        instance().setClockDivider(SPI_CLOCK_DIV32);
    }

private:
    inline static SPI& instance()
    {
        return SPIInstance<CH,MCUType>::instance();
    }

    friend class SPIInstance<CH>;
    SPI(sfr8_t spdr,
        sfr8_t spcr,
        sfr8_t spsr,
        sfr8_t sreg,
        uint8_t spif,
        uint8_t mstr,
        uint8_t spe)
        : SPIBase(spdr,
                  spcr,
                  spsr,
                  sreg,
                  spif,
                  mstr,
                  spe)
    { }
};

}

#endif // DRIVER_AVR_SPI_HPP

