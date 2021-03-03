#=============================================================================
# Copyright 2017       Helio Chissini de Castro <helio@kde.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(qt2
    VERSION 2.3.2
    LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

include(GNUInstallDirs)
include(Qt2Utils)
include(Qt2InternalBuild)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(ENABLE_XFT "Enable support for Anti-Aliased fonts." ON)
option(ENABLE_SESSION_MANAGEMENT "Support X Session Management, links in -lSM -lICE" ON)
option(ENABLE_THREAD_SUPPORT "Compile with Threading Support" ON)
option(ENABLE_OPENGL "Compile OpenGL module" ON)
option(SYSTEM_ZLIB "Use a zlib from the operating system" ON)
option(SYSTEM_LIBPNG "Use libpng from the operating system." ON)
option(SYSTEM_JPEG "Use jpeglib from the operating system." ON)
option(BUILD_TUTORIAL "Build tutorials." OFF)
option(BUILD_EXAMPLES "Build examples." OFF)

find_package(PkgConfig REQUIRED)

if(ENABLE_XFT)
    find_package(Xft REQUIRED)
    pkg_search_module(FONTCONFIG REQUIRED fontconfig)
    find_package(Freetype REQUIRED)
endif()

if(ENABLE_SESSION_MANAGEMENT)
    pkg_search_module(SM REQUIRED sm)
endif()

if(ENABLE_THREAD_SUPPORT)
    find_package(Threads REQUIRED)

    option(SANITIZE_THREADS "Enable runtime thread sanitizing (for development)")
    if (SANITIZE_THREADS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
    endif()
endif()

if(ENABLE_OPENGL)
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
endif()

if(SYSTEM_ZLIB)
    find_package(ZLIB REQUIRED)
endif()

if(SYSTEM_LIBPNG)
    find_package(PNG REQUIRED)
endif()

find_package(MNG)

if(SYSTEM_JPEG)
    find_package(JPEG REQUIRED)
    pkg_search_module(XMU xmu REQUIRED)
endif()

find_package(X11 REQUIRED)
find_package(BYACC REQUIRED)
find_package(FLEX REQUIRED)

add_subdirectory(src)
add_subdirectory(tools)
add_subdirectory(doc)
if(BUILD_TUTORIAL)
    add_subdirectory(tutorial)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
