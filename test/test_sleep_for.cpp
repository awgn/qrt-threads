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
    run(qrt::this_cpu::cycles_type pending)
    {
        qrt_context_begin;

        inter_time = qrt::cpufreq(0).freq_hardware() * 1000/_M_rate;

        for( ts = qrt::this_cpu::get_cycles(); qrt::this_cpu::get_cycles() < this->end() ; )
        {
            qrt_sleep_for(inter_time);
            std::cerr <<"thread #" << this->get_id() << " hello world! " << qrt::this_cpu::get_cycles() << std::endl;
        }
        
        qrt_context_end;
    }    
};



int
main(int argc, char *argv[])
{
    qrt::this_cpu::cycles_type sec  = qrt::cpufreq(0).freq_hardware() * 1000;

    // switch to performance governor:

    qrt::cpufreq(0).set_policy_governor("performance");

    // qrt::this_cpu::cycles_type msec = qrt::this_cpu::Hz()/10000;

    qrt::deadline_scheduler sched0;

    mythread a(qrt::this_cpu::get_cycles(), qrt::this_cpu::get_cycles() + sec * 5, 1);
    mythread b(qrt::this_cpu::get_cycles(), qrt::this_cpu::get_cycles() + sec * 5, 2);

    sched0.schedparam(SCHED_FIFO,99);
    sched0.affinity(0 /* core */);

    sched0(&a);
    sched0(&b);

    sched0.start();
    sched0.join();

    return 0;
}

