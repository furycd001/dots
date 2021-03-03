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

# MOC is a special case, to boostrap moc we need to include
# external source files
set(TOOLS_IMPORTED_SRCS
    ${PROJECT_SOURCE_DIR}/src/tools/qbuffer.cpp
    ${PROJECT_SOURCE_DIR}/src/tools/qcollection.cpp
    ${PROJECT_SOURCE_DIR}/src/tools/qcstring.cpp
    ${PROJECT_SOURCE_DIR}/src/tools/qdatastream.cpp
    ${PROJECT_SOURCE_DIR}/src/tools/qdatetime.cpp
    ${PROJECT_SOURCE_DIR}/src/tools/qfile.cpp
    ${PROJECT_SOURCE_DIR}/src/tools/qgarray.cpp
    ${PROJECT_SOURCE_DIR}/src/tools/qgdict.cpp
    ${PROJECT_SOURCE_DIR}/src/tools/qglist.cpp
    ${PROJECT_SOURCE_DIR}/src/tools/qglobal.cpp
    ${PROJECT_SOURCE_DIR}/src/tools/qgvector.cpp
    ${PROJECT_SOURCE_DIR}/src/tools/qiodevice.cpp
    ${PROJECT_SOURCE_DIR}/src/tools/qregexp.cpp
    ${PROJECT_SOURCE_DIR}/src/tools/qstring.cpp
    ${PROJECT_SOURCE_DIR}/src/tools/qtextcodec.cpp
    ${PROJECT_SOURCE_DIR}/src/tools/qtextstream.cpp
    ${PROJECT_SOURCE_DIR}/src/tools/qutfcodec.cpp
)

if(UNIX)
    list(APPEND TOOLS_IMPORTED_SRCS
        ${PROJECT_SOURCE_DIR}/src/tools/qfile_unix.cpp)
elseif(MSVC)
    list(APPEND TOOLS_IMPORTED_SRCS
        ${PROJECT_SOURCE_DIR}/src/tools/qfile_win.cpp)
endif()

byacc_target(MyParser moc.y ${CMAKE_CURRENT_BINARY_DIR}/mocgen.cpp)
flex_target(MyScanner moc.l ${CMAKE_CURRENT_BINARY_DIR}/lex.yy)
add_flex_byacc_dependency(MyScanner MyParser)

#add_custom_command(
#    OUTPUT lex.yy.c
#    COMMAND flex -o lex.yy.c ${CMAKE_CURRENT_SOURCE_DIR}/moc.l
#    )
#add_custom_command(
#    OUTPUT mocgen.cpp
#    DEPENDS lex.yy.c
#    COMMAND yacc -d -o mocgen.cpp ${CMAKE_CURRENT_SOURCE_DIR}/moc.y
#    )
#add_custom_target(flexyacc
#    DEPENDS mocgen.cpp
#    )

add_executable(moc-qt2
    ${TOOLS_IMPORTED_SRCS}
    ${BYACC_MyParser_OUTPUTS}
    ${FLEX_MyScanner_OUTPUTS}
    )

target_compile_definitions(moc-qt2 PRIVATE -DQT_NO_CODECS -DQT_LITE_UNICODE)
target_compile_definitions(moc-qt2 PRIVATE -DYYDEBUG=1)

# This is really not the most optimal or pretty way to add the include directories
# But at least we avoid to polute the main cmakelist files
target_include_directories(moc-qt2 PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src/tools
)
target_compile_options(moc-qt2 PRIVATE -Wno-format-security)

install(TARGETS moc-qt2 RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
