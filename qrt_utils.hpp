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

#ifndef _QRT_UTIL_HPP_
#define _QRT_UTIL_HPP_ 

namespace qrt {

    // assembly policies are taken from the linux kernel 2.6/include/arch-.../
    //
 
    namespace detail 
    {
#if defined(__i386__) && !defined(__LP64__)    
            typedef unsigned long long cycles_t;

            static cycles_t get_cycles() 
            {
                cycles_t val;
                __asm__ __volatile__("rdtsc" : "=A" (val));
                return val;
            }
#endif

#if defined(__LP64__)    
            typedef unsigned long long cycles_t;
        
            static cycles_t get_cycles() 
            {
                cycles_t val;
                unsigned long __a,__d;
                __asm__ __volatile__("rdtsc" : "=a" (__a), "=d" (__d));
                val = ((unsigned long)__a) | (((unsigned long long)__d)<<32);
                return val;
            }
#endif

#if defined(__ia64__)
            typedef unsigned long long cycles_t;

            static inline cycles_t get_cycles ()
            {
                cycles_t val;
                __asm__ __volatile__ ("mov %o=ar%1" : "=r" (val) : "i" (44));  
                return val;
            }
#endif
    } // detail

    // this_cpu implemented as policy class
    //

    struct this_cpu
    {
        typedef detail::cycles_t cycles_type;

        static inline cycles_type 
        get_cycles()
        {
            return detail::get_cycles();
        }

        static inline
        bool busywait_until(const cycles_type &t)
        {
            if (detail::get_cycles() >= t)
                return false;
            while (detail::get_cycles() < t)
            {}
            return true;
        }
        
        static inline
        bool busywait_for(const cycles_type &d)
        {
            return busywait_until(detail::get_cycles() + d);
        }
    }; 
} // namespace qrt

#endif /* _QRT_UTIL_HPP_ */
