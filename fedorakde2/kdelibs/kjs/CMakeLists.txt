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

bison_target(MyParser
    grammar.y
    ${CMAKE_CURRENT_BINARY_DIR}/grammar.cpp
    COMPILE_FLAGS "-d -p kjsyy"
    )

set(libkjs_SOURCES
    array_object.cpp
    bool_object.cpp
    collector.cpp
    date_object.cpp
    debugger.cpp
    error_object.cpp
    function.cpp
    function_object.cpp
    global_object.cpp
    internal.cpp
    kjs.cpp
    lexer.cpp
    lookup.cpp
    math_object.cpp
    nodes.cpp
    number_object.cpp
    object.cpp
    object_object.cpp
    operations.cpp
    regexp.cpp
    regexp_object.cpp
    string_object.cpp
    testkjs.cpp
    types.cpp
    ustring.cpp
    ${BISON_MyParser_OUTPUTS}
    )

set(libkjs_noinst_HEADERS
    nodes.h
    lexer.h
    regexp.h
    internal.h
    collector.h
    object_object.h
    function_object.h
    bool_object.h
    math_object.lut.h
    array_object.h
    string_object.h
    number_object.h
    date_object.h
    regexp_object.h
    error_object.h
    debugger.h
    )

set(libkjs_HEADERS
    kjs.h
    object.h
    operations.h
    ustring.h
    function.h
    lookup.h
    types.h
    )

install(FILES ${libkjs_HEADERS} DESTINATION ${KDE2_INCLUDEDIR}/kjs)

kde2_library(libkjs
    SOURCES
        ${libkjs_SOURCES}
    VERSION
        1.0.0
    )
if (HAVE_PCREPOSIX)
    target_link_libraries(libkjs PkgConfig::libpcreposix)
endif()

add_executable(testkjs testkjs.cpp)
target_link_libraries(testkjs kde2::kjs)
