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

#ifndef _QRT_THREAD_HPP_
#define _QRT_THREAD_HPP_ 

#include <qrt_coroutines.hpp>
#include <qrt_scheduler.hpp>
#include <qrt_utils.hpp>      

#include <type_traits>

namespace qrt {

    ///////////////////////////////////////
    // quasi-RT thread class
    ///////////////////////////////////////

    template <typename T, /* timestamp */ 
              typename Native, 
              template <typename, typename > class Heap = qrt::random_access::vector_heap >
    class basic_thread 
    {
    public:
        typedef Heap< typename T::cycles_type, basic_thread *> heap_type;
        typedef typename T::cycles_type cycles_type;

    protected:        
        static int &
        _S_counter() 
        {
            static int n;
            return n;
        }

        static int & 
        _S_id() 
        {
            static int id;
            return ++id;
        }

        int _M_state;
        int _M_id;

        const typename T::cycles_type _M_init;     /* init time */
        const typename T::cycles_type _M_fini;     /* fini time */
              typename T::cycles_type _M_next;     /* next deadline */
              typename T::cycles_type _M_tstamp;   /* tstamp, used by sleep_for */

        typename basic_scheduler<T, Native, Heap>::heap_type * _M_heap;

        basic_thread(const typename T::cycles_type &b, const typename T::cycles_type &e) 
        : _M_state(0), 
          _M_id(_S_id()), 
          _M_init(b), 
          _M_fini(e),
          _M_next(b)
        {}

        virtual ~basic_thread() 
        {}

        basic_thread(const basic_thread &) = delete;
        basic_thread& operator=(const basic_thread &) = delete;

        void
        incr()
        { _S_counter()++; } 

        void
        decr()
        { _S_counter()--; }

    public:
        int 
        get_id() const 
        { return _M_id; }

        typename T::cycles_type 
        begin() const 
        { return _M_init; }

        typename T::cycles_type 
        end() const 
        { return _M_fini; }

        typename T::cycles_type 
        next_deadline() const 
        { return _M_next; }

        void 
        next_deadline(typename T::cycles_type value)  
        { _M_next = value; }

        bool 
        is_running() const 
        {
            typename T::cycles_type r = T::get_cycles(); 
            return (r > _M_init) && (r < _M_fini); 
        }
        
        template <typename H>
        void set_heap(H &heap)
        {
            _M_heap = &heap;            
        }
    
        virtual typename T::cycles_type run(typename T::cycles_type)=0;
    };

#ifdef __linux__

    typedef basic_thread<qrt::this_cpu, linux_native_thread, qrt::random_access::vector_heap> thread; 

#else
    
    typedef basic_thread<qrt::this_cpu, null_native_thread, qrt::random_access::vector_heap> thread; 

#endif

}

#endif /* _QRT_THREAD_HPP_ */
