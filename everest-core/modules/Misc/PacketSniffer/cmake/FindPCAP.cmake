# FindPCAP.cmake
# ===========================================
# See https://github.com/zeek/cmake/FindPCAP.cmake for usage and update instructions.
#
# BSD License
# -----------
#[[
  Copyright (c) 1995-2017, The Regents of the University of California
  through the Lawrence Berkeley National Laboratory and the
  International Computer Science Institute. All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  (1) Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

  (2) Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

  (3) Neither the name of the University of California, Lawrence Berkeley
      National Laboratory, U.S. Dept. of Energy, International Computer
      Science Institute, nor the names of contributors may be used to endorse
      or promote products derived from this software without specific prior
      written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.

  Note that some files in the distribution may carry their own copyright
  notices.
]]

# - Try to find libpcap include dirs and libraries
#
# Usage of this module as follows:
#
#     find_package(PCAP)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  PCAP_ROOT_DIR             Set this variable to the root installation of
#                            libpcap if the module has problems finding the
#                            proper installation path.
#
# Variables defined by this module:
#
#  PCAP_FOUND                System has libpcap, include and library dirs found
#  PCAP_INCLUDE_DIR          The libpcap include directories.
#  PCAP_LIBRARY              The libpcap library (possibly includes a thread
#                            library e.g. required by pf_ring's libpcap)
#  HAVE_PF_RING              If a found version of libpcap supports PF_RING

find_path(PCAP_ROOT_DIR
    NAMES include/pcap.h
)

find_path(PCAP_INCLUDE_DIR
    NAMES pcap.h
    HINTS ${PCAP_ROOT_DIR}/include
)

find_library(PCAP_LIBRARY
    NAMES pcap
    HINTS ${PCAP_ROOT_DIR}/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PCAP DEFAULT_MSG
    PCAP_LIBRARY
    PCAP_INCLUDE_DIR
)

include(CheckCSourceCompiles)
set(CMAKE_REQUIRED_LIBRARIES ${PCAP_LIBRARY})
check_c_source_compiles("int main() { return 0; }" PCAP_LINKS_SOLO)
set(CMAKE_REQUIRED_LIBRARIES)

# check if linking against libpcap also needs to link against a thread library
if (NOT PCAP_LINKS_SOLO)
    find_package(Threads)
    if (THREADS_FOUND)
        set(CMAKE_REQUIRED_LIBRARIES ${PCAP_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
        check_c_source_compiles("int main() { return 0; }" PCAP_NEEDS_THREADS)
        set(CMAKE_REQUIRED_LIBRARIES)
    endif ()
    if (THREADS_FOUND AND PCAP_NEEDS_THREADS)
        set(_tmp ${PCAP_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
        list(REMOVE_DUPLICATES _tmp)
        set(PCAP_LIBRARY ${_tmp}
            CACHE STRING "Libraries needed to link against libpcap" FORCE)
    else ()
        message(FATAL_ERROR "Couldn't determine how to link against libpcap")
    endif ()
endif ()

include(CheckFunctionExists)
set(CMAKE_REQUIRED_LIBRARIES ${PCAP_LIBRARY})
check_function_exists(pcap_get_pfring_id HAVE_PF_RING)
check_function_exists(pcap_dump_open_append HAVE_PCAP_DUMP_OPEN_APPEND)
set(CMAKE_REQUIRED_LIBRARIES)

mark_as_advanced(
    PCAP_ROOT_DIR
    PCAP_INCLUDE_DIR
    PCAP_LIBRARY
)