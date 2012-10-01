qrt-threads
===========

Quasi-realtime threads on the top of STL, std::thread and co-routines...

qrt::thread++ is a C++ framework that implements quasi real-time cooperative threads on the top of Standard Template Library (STL), C++0x std::thread, coroutines (and linux native-thread policy).

This implementation of uber-light threads (with context switches ~50 : 500 CPU cycles) relies on linux soft-realtime threads and is intended to be used for applications with strict timing constraints.

Currently the library is under development and major changes may occur in both the design and the performance achievable. Additional information will be provided as the project advances.

Changelog:
* added the support of libcpufreq.
* new c++0x library!
* basic_scheduler<> implemented on the top of std::thread (platform-independent)
* fine-grained control over schedulers by means of native-thread policy (linux-only)


