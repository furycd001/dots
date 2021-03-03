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

add_subdirectory(customization)
add_subdirectory(docbook)

set(FILTER_SOURCES
    ${PROJECT_SOURCE_DIR}/kio/gzip/kgzipfilter.cpp
    ${PROJECT_SOURCE_DIR}/kio/bzip2/kbzip2filter.cpp
    )

set(kio_help_SOURCES kio_help.cpp main.cpp xslt.cpp)

kde2_module(kio_help
    SOURCES
        ${FILTER_SOURCES}
        ${kio_help_SOURCES}
    LIBS
        kde2::ksycoca
        BZip2::BZip2
        ${LIBXML2_LIBRARIES}
        ${LIBXSLT_LIBRARIES}
    COMPILE_DEFINITIONS
        -DSRCDIR="${CMAKE_CURRENT_SOURCE_DIR}"
    INCLUDE_DIRECTORIES
        ${LIBXML2_INCLUDE_DIR}
    )

set_property(TARGET module_kio_help PROPERTY CXX_STANDARD 11)

add_executable(meinproc xslt.cpp meinproc.cpp ${FILTER_SOURCES})
set_property(TARGET meinproc PROPERTY CXX_STANDARD 11)
target_compile_definitions(meinproc
    PRIVATE -DSIMPLE_XSLT -DSRCDIR="${CMAKE_CURRENT_SOURCE_DIR}"
    )
target_include_directories(meinproc PRIVATE ${LIBXML2_INCLUDE_DIR})
target_link_libraries(meinproc
    kde2::ksycoca
    BZip2::BZip2
    ${LIBXML2_LIBRARIES}
    ${LIBXSLT_LIBRARIES}
    )

add_executable(xml2man xml2man.cpp xslt.cpp ${FILTER_SOURCES})
set_property(TARGET xml2man PROPERTY CXX_STANDARD 11)
target_compile_definitions(xml2man
    PRIVATE -DSIMPLE_XSLT -DSRCDIR="${CMAKE_CURRENT_SOURCE_DIR}"
    )
target_include_directories(xml2man PRIVATE ${LIBXML2_INCLUDE_DIR})
target_link_libraries(xml2man
    kde2::ksycoca
    BZip2::BZip2
    ${LIBXML2_LIBRARIES}
    ${LIBXSLT_LIBRARIES}
    )

configure_file(checkXML.in ${CMAKE_CURRENT_BINARY_DIR}/checkXML @ONLY)

install(TARGETS meinproc xml2man RUNTIME DESTINATION ${KDE2_BINDIR})

install(
    PROGRAMS
        kdb2html
        ${CMAKE_CURRENT_BINARY_DIR}/checkXML
        xmlizer
    DESTINATION
        ${KDE2_BINDIR}
    )
install(FILES help.protocol DESTINATION ${KDE2_SERVICESDIR})
