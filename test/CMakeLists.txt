# 
# qrt::thread++ - LGPL library 
#
# Copyright (C) 2010 Nicola Bonelli
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public
# License along with this library; if not, write to the
# Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
#

cmake_minimum_required(VERSION 2.6)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -Ofast -march=native -Wall -Wextra -std=c++11")

project(QRT)

include_directories(. ../)

add_executable(test_dummy test_dummy.cpp)
add_executable(test_context_swich test_context_swich.cpp)
add_executable(test_sleep_for test_sleep_for.cpp)

target_link_libraries(test_dummy -pthread -lcpufreq)
target_link_libraries(test_sleep_for -pthread -lcpufreq)
target_link_libraries(test_context_swich -pthread -lcpufreq)

