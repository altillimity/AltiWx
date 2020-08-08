# - Try to find libonion
# Once done, this will define
#
#  ONION_FOUND - system has libonion
#  ONION_INCLUDE_DIRS - the libonion include directories
#  ONION_LIBRARIES - link these to use libonion
#
# Copyright (C) 2020 Aang23
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1.  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
# 2.  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND ITS CONTRIBUTORS ``AS
# IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR ITS
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

find_package(PkgConfig)
pkg_check_modules(PC_ONION QUIET onion)

find_library(ONION_LIBRARIES
    NAMES onion
    HINTS ${PC_ONION_LIBDIR}
          ${PC_ONION_LIBRARY_DIRS}
)

find_path(ONION_INCLUDE_DIR
    NAMES onion.h
    HINTS ${PC_ONION_INCLUDEDIR}
          ${PC_ONION_INCLUDE_DIRS}
    PATH_SUFFIXES onion
)

set(ONION_INCLUDE_DIRS ${ONION_INCLUDE_DIR})
set(ONION_LIBRARIES ${ONION_LIBRARIES} "-lonioncpp")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Onion REQUIRED_VARS ONION_INCLUDE_DIRS ONION_LIBRARIES)

mark_as_advanced(
    ONION_INCLUDE_DIR
    ONION_INCLUDE_DIRS
    ONION_LIBRARIES
)
