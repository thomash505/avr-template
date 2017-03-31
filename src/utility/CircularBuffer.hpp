//
// CircularBuffer.hpp
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

#ifndef UTILITY_CIRCULARBUFFER_HPP
#define UTILITY_CIRCULARBUFFER_HPP

#include <stdlib.h>

namespace utility {

// Default policy for incrementing buffer index
template<int SIZE, bool IS_POWER_2 = false>
struct BufferIncrementPolicyInt
{
    inline static size_t increment(size_t n)
    {
        return (n + 1) % SIZE;
    }
};

// Specialisation policy for size which is a power of two
template<int SIZE>
struct BufferIncrementPolicyInt<SIZE,true>
{
    inline static size_t increment(size_t n)
    {
        return (n + 1) & (SIZE - 1);
    }
};

template<int SIZE>
struct BufferIncrementPolicy
{
    enum {isPower2 = (SIZE >= 1) & !(SIZE & (SIZE - 1))};
    inline static size_t increment(size_t n)
    {
        return BufferIncrementPolicyInt< SIZE, isPower2 >::increment(n);
    }
};

template<int SIZE, typename T = char>
class CircularBuffer
{
public:
    CircularBuffer() : head(0), tail(0) {}
	
    inline bool isEmpty() const { return head == tail;}
	
    inline bool isFull() const
    {
        return BufferIncrementPolicy<SIZE>::increment(head) == tail;
    }

    void push(const T &c)
    {
        buffer[head] = c;
        head = BufferIncrementPolicy<SIZE>::increment(head);
    }

    T operator[] (uint16_t i) const
    {
        return buffer[BufferIncrementPolicy<SIZE>::increment(i)];
    }

    void queue(const T &c)
    {
        if (!isFull())
        {
            buffer[head] = c;
            head = BufferIncrementPolicy<SIZE>::increment(head);
        }
    }
    
    T dequeue()
    {
        if (isEmpty())
        {
            return T(0);
        }
        else
        {
            T temp = buffer[tail];
            tail = BufferIncrementPolicy<SIZE>::increment(tail);
            return temp;
        }
    }
private:
    volatile size_t head, tail;
    T buffer[SIZE];
};

}

#endif // UTILITY_CIRCULARBUFFER_HPP
