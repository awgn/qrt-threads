/* $Id: qrt_test.cpp 7 2010-01-19 19:56:59Z nicola.bonelli $ */
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

#include <qrt_cpufreq.hpp>
#include <qrt_thread.hpp>

#include <sys/mman.h>
#include <iostream>

// this test measures the cost of context switch in timestamp counters 
// 

qrt::this_cpu::cycles_type ts_beg;
qrt::this_cpu::cycles_type ts_end;

struct mythread : public qrt::thread
{
    int _M_rate;

    qrt::this_cpu::cycles_type inter_time;
    qrt::this_cpu::cycles_type ts;

public:
    mythread(qrt::this_cpu::cycles_type b, qrt::this_cpu::cycles_type e, int r)
    : qrt::thread(b,e),
      _M_rate(r)
    {}

    // thread body...

    qrt::this_cpu::cycles_type 
    run(qrt::this_cpu::cycles_type)
    {
        qrt_context_begin;

        inter_time = qrt::cpufreq(0).freq_hardware() * 1000/_M_rate;

        for( ts = qrt::this_cpu::get_cycles(); qrt::this_cpu::get_cycles() < this->end() ; )
        {
            ts += inter_time;

            ts_beg = qrt::this_cpu::get_cycles();
            qrt_context_switch(ts);
            ts_end = qrt::this_cpu::get_cycles();

            printf("%llu\n", ts_end-ts_beg);
            qrt::this_cpu::busywait_until(ts);
        }

        ts_beg = qrt::this_cpu::get_cycles();

        qrt_context_end;
    }    
};


int
main(int argc, char *argv[])
{
    if (argc < 3) {
        std::cerr << "usage: threads rate" << std::endl;
        exit(1);
    }
    
    int nthread = atoi(argv[1]);
    int rate    = atoi(argv[2]);

    qrt::cpufreq(0).set_policy_governor("performance");
    qrt::this_cpu::cycles_type sec = qrt::cpufreq(0).freq_hardware() * 1000;

    qrt::stat_deadline_scheduler sched0;

    sched0.schedparam(SCHED_FIFO,99);
    sched0.affinity(0 /* core */);

    for(int i = 0; i < nthread; ++i ) 
    {
        mythread * t = new mythread(qrt::this_cpu::get_cycles(), qrt::this_cpu::get_cycles() + sec * 5, rate);
        sched0(t);        
    }

    mlockall(MCL_CURRENT|MCL_FUTURE);

    sched0.start();
    sched0.join();

    std::cerr << sched0.stat() << std::endl;
    return 0;
}

