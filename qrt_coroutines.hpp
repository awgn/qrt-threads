/* $Id$ */
/* 
 * qrt::thread++ - LGPL library 
 *
 * Copyright (C) 2010 Nicola Bonelli
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _QRT_COROUTINES_HPP_
#define _QRT_COROUTINES_HPP_ 

#include <qrt_utils.hpp>

template <typename Tp>
inline bool likely(Tp x)
{
    return __builtin_expect(!!(x), 1);
}

template <typename Tp>
inline bool unlikely(Tp x)
{
    return __builtin_expect(!!(x), 0);
}


#define qrt_context_begin switch(_M_state) { case 0: this->incr();


#define qrt_context_end } this->decr(); return 0;


#define qrt_schedule(deadline,pending) if (unlikely(!_M_heap->empty()) && ( pending < deadline )) \
    do { _M_state = __LINE__; return deadline; case __LINE__:; } \
while(0); \
qrt::this_cpu::busywait_until(deadline)


#define qrt_force_schedule(deadline) do { \
    _M_state = __LINE__; return deadline; case __LINE__:; } \
while(0); \
qrt::this_cpu::busywait_until(deadline)


#define qrt_context_switch(deadline) do { \
    _M_state = __LINE__; return deadline; case __LINE__:; } \
while(0); 


#define qrt_sleep_for(ticks) do { \
    _M_tstamp = qrt::this_cpu::get_cycles() + ticks; \
    _M_state = __LINE__; return _M_tstamp; case __LINE__:; } \
while(0); \
qrt::this_cpu::busywait_until(_M_tstamp)


#endif /* _QRT_COROUTINES_HPP_ */
