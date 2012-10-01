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
