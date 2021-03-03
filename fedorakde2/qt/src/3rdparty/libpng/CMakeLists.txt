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

set(PNG_SRCS
    png.c
    pngerror.c
    pngget.c
    pngmem.c
    pngpread.c
    pngread.c
    pngrio.c
    pngrtran.c
    pngrutil.c
    pngset.c
    pngtest.c
    pngtrans.c
    pngvcrd.c
    pngwio.c
    pngwrite.c
    pngwtran.c
    pngwutil.c
)

add_library(libpng STATIC ${PNG_SRCS})
target_include_directories(libpng SYSTEM BEFORE
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
   )

add_library(PNG::PNG ALIAS libpng)

set_target_properties(libpng PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    )

install(TARGETS libpng EXPORT libpngTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt2
    )

export(EXPORT libpngTargets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/libpngTargets.cmake
    TARGETS libpng
    NAMESPACE PNG::
)

install(EXPORT libpngTargets
    FILE libpngTargets.cmake
    NAMESPACE PNG::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Qt2
    )
