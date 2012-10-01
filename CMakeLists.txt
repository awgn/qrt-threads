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

project(QRT)

cmake_minimum_required(VERSION 2.6)

include(CheckIncludeFiles)

subdirs(test)

message("${CMAKE_BUILD_TYPE}")

CHECK_INCLUDE_FILES(cpufreq.h HAVE_CPUFREQ_H)

if (NOT "${HAVE_CPUFREQ_H}" STREQUAL "1" )
    message(FATAL_ERROR "libcpufreq-dev package not installed...")
endif (NOT "${HAVE_CPUFREQ_H}" STREQUAL "1" )

#
# compiler options
#

set(CMAKE_CXX "g++")
set(CMAKE_CXX_FLAGS "-pthread -O3 -g0 -Wall -Wnon-virtual-dtor -Woverloaded-virtual -Wno-strict-aliasing -fomit-frame-pointer -mtune=i686 -std=c++0x")

