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

#ifndef _QRT_SCHEDULER_HPP_
#define _QRT_SCHEDULER_HPP_ 

#include <qrt_utils.hpp>   
#include <qrt_heap.hpp>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <memory>
#include <thread>

#ifdef __linux__
#include <pthread.h>
#endif

namespace qrt { 

    ///////////////////// statistic for qrt::thread

    template <typename T> 
    struct stat_type 
    {
        int                      miss;
        int                      sched;
        typename T::cycles_type  otime;
        typename T::cycles_type  mean;

        stat_type() 
        : miss(0), sched(0), otime(0), mean(0) 
        {}
    };

    struct stat_disabled {};
    struct stat_enabled  {};

    template <typename T>
    std::ostream &
    operator<<(std::ostream &out, const stat_type<T> &s)
    {
        return out << "[" << s.sched << " context-swiches, " << s.miss << " missed deadline, " << 
                s.otime << " max_delay, " << s.mean << " average_dalay]";        
    }
    
    ///////////////////// forward declaration

    template <typename T /* timestamp counter */, typename Native, 
        template <typename, typename> class Heap>
    class basic_thread;  /* forward declaration */ 

    template <typename T, typename Native,  
        template <typename, typename> class Heap, typename>
    class basic_scheduler; /* forward declaration */

    ///////////////////// scheduler_thread 
    
    template <typename T, typename Native, template <typename, typename> class Heap, typename Stat>
    struct scheduler_thread
    {
        typedef basic_scheduler<T, Native, Heap, Stat>  sched_type;
        typedef basic_thread<T, Native, Heap>           thread_type;

        typedef void result_type;
        void operator()(sched_type *sched)
        {
            // scheduler main loop
            for(;;) 
            {            
                thread_type * t = sched->eligible();

                if (unlikely(!t)) 
                    break;
                
                // wait for the first deadline for this thread...
                //
                if (!T::busywait_until(t->begin()))  
                {   
                    if (std::is_same<Stat, qrt::stat_enabled>::value) 
                    { // if stat are enabled...

                        if (t->next_deadline() < T::get_cycles())   // is this a missed deadline?  
                        {
                            typename T::cycles_type diff = T::get_cycles()-t->next_deadline();
                            sched->stat().miss++;
                            sched->stat().otime = std::max(sched->stat().otime,diff);
                            sched->stat().mean = (sched->stat().otime + diff)>>1;
                        }
                    }
                }

                // run the thread...
                typename T::cycles_type deadline = t->run(t->next_deadline());
                if (deadline)
                {
                    // store the next deadline for this thread..
                    t->next_deadline(deadline);

                    // reschedule... 
                    sched->operator()(t, deadline);

                    if (std::is_same<Stat, qrt::stat_enabled>::value)
                        sched->stat().sched++;
                }
            }
        }
    };

    ///////////////////// native thread policy (interface)

    struct null_native_thread
    {
        static void set_affinity(int) 
        {} 

        static void set_schedparam(int,int)
        {}
    };
    
    ///////////////////// basic_scheduler (portable)
                              
    template <typename T, 
              typename Native = null_native_thread,  
              template <typename, typename> class Heap = qrt::random_access::vector_heap, 
              typename Stat = stat_disabled >
    class basic_scheduler 
    {
    public:
        // scheduler statistics...

        typedef Heap< typename T::cycles_type, basic_thread<T, Native, Heap> *> heap_type;
        typedef basic_thread<T, Native, Heap> thread_type;

    protected:
        heap_type       _M_heap;
        std::thread     _M_thread;

        int             _M_cpu;
        int             _M_policy;
        int             _M_prio;
    
        stat_type<T>    _M_stat;

    public:
        basic_scheduler()
        :  _M_heap(), _M_thread(), _M_cpu(), _M_policy(), _M_prio(), _M_stat()  
        {}

        ~basic_scheduler()
        {
            if (_M_thread.joinable())
                _M_thread.join();
        }

        basic_scheduler(const basic_scheduler &) = delete;
        basic_scheduler& operator=(const basic_scheduler &) = delete;

        basic_scheduler(basic_scheduler &&rhs)
        : _M_heap(std::move(rhs._M_heap)),
          _M_thread(std::move(rhs._M_thread)),
          _M_cpu(std::move(rhs._M_cpu)),
          _M_policy(std::move(rhs._M_policy)),
          _M_prio(std::move(rhs._M_prio)),
          _M_stat(std::move(rhs._M_stat)) 
        {}

        basic_scheduler& operator=(basic_scheduler &&rhs)
        {
            _M_heap   = std::move(rhs._M_heap);
            _M_thread = std::move(rhs._M_thread);
            _M_cpu    = std::move(rhs._M_cpu);
            _M_policy = std::move(rhs._M_policy);
            _M_prio   = std::move(rhs._M_prio);
            _M_stat   = std::move(rhs._M_stat); 
            return *this;
        }

        // schedule and setup the heap...
        //

        void
        operator()( basic_thread<T, Native, Heap> *t, typename T::cycles_type deadline = 0)
        {
            t->set_heap(_M_heap);
            _M_heap.push(deadline ? : t->begin(), t);
        } 

        thread_type *
        eligible()
        {
            if (_M_heap.empty())
                return 0;
            return _M_heap.pop_value();
        } 

        void 
        start() 
        {    
            if(_M_thread.get_id() != std::thread::id())
               throw std::runtime_error("qtr::scheduler already started");

            // start the standard thread..
            _M_thread = std::thread(scheduler_thread<T,Native,Heap,Stat>(), this);
        
            Native::set_affinity(_M_thread, _M_cpu);

            Native::set_schedparam(_M_thread, _M_policy, _M_prio);
        }

        void 
        join()  
        {   
            if(_M_thread.joinable())
                _M_thread.join();
        }
        
        const stat_type<T> &
        stat() const
        { 
            return _M_stat;
        }

        stat_type<T> &
        stat() 
        { 
            return _M_stat;
        }
        
        // advanced features, by means of native threads...
        //

        int
        affinity() const
        { return _M_cpu; }

        void
        affinity(int _n)
        {
            if (this->_M_thread.get_id() != std::thread::id())
            {
                Native::set_affinity(_M_thread, _n);
            }
            _M_cpu = _n;
        }

        void
        schedparam(int _policy, int _prio)
        {
            if (this->_M_thread.get_id() != std::thread::id())
            {
                Native::set_schedparam(_M_thread, _policy, _prio);
            }
            _M_policy = _policy;
            _M_prio   = _prio;
        }

        std::pair<int,int>
        schedparam() const
        { 
            return std::make_pair(_M_policy,_M_prio); 
        }
        
        // get the native handle of the scheduler thread...
        //

        std::thread::native_handle_type
        native_handle()
        {
            return _M_thread.native_handle();
        }

    };


#ifdef __linux__
    
    // this class enables a fine-grained control over the thread scheduler
    //
    struct linux_native_thread  
    {
        static void 
        set_affinity(std::thread &t, int n) 
        {
            if(t.get_id() == std::thread::id())
                throw std::runtime_error("qrt::scheduler not running");

            cpu_set_t cpuset;
            CPU_ZERO(&cpuset); CPU_SET(n, &cpuset);

            std::thread::native_handle_type pt = t.native_handle();
            if ( ::pthread_setaffinity_np(pt, sizeof(cpuset), &cpuset) != 0)
                throw std::runtime_error("pthread_setaffinity_np");
        }

        static void 
        set_schedparam(std::thread &t, int policy, int prio)
        {
            if(t.get_id() == std::thread::id())
                throw std::runtime_error("qrt::scheduler not running");

            // set priority..
            sched_param param;
            param.sched_priority = prio;

            std::thread::native_handle_type pt = t.native_handle();
            if ( ::pthread_setschedparam(pt, policy, &param) != 0)
                throw std::runtime_error("pthread_attr_setschedparam");
        }
    };

#endif

#ifdef __linux__

    typedef basic_scheduler< qrt::this_cpu, linux_native_thread, qrt::random_access::vector_heap, stat_disabled > deadline_scheduler;
    typedef basic_scheduler< qrt::this_cpu, linux_native_thread, qrt::random_access::vector_heap, stat_enabled  > stat_deadline_scheduler;

#else

    typedef basic_scheduler< qrt::this_cpu, null_native_thread, qrt::random_access::vector_heap, stat_disabled > deadline_scheduler;
    typedef basic_scheduler< qrt::this_cpu, null_native_thread, qrt::random_access::vector_heap, stat_enabled  > stat_deadline_scheduler;

#endif

}

#endif /* _QRT_SCHEDULER_HPP_ */
