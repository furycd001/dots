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

list(APPEND QT_DEFINITIONS -DQT_BUILTIN_GIF_READER=1 -DQT_XFT)

if(ENABLE_THREAD_SUPPORT)
    list(APPEND QT_DEFINITIONS -D_REENTRANT -DQT_THREAD_SUPPORT)
endif()

add_subdirectory(3rdparty)
add_subdirectory(moc)
add_subdirectory(compat)
add_subdirectory(tools)
add_subdirectory(kernel)
add_subdirectory(widgets)
add_subdirectory(dialogs)
add_subdirectory(iconview)
add_subdirectory(workspace)
add_subdirectory(network)
add_subdirectory(canvas)
add_subdirectory(table)
add_subdirectory(xml)
if(ENABLE_OPENGL)
    add_subdirectory(opengl)
endif()

add_library(Qt2 SHARED
    $<TARGET_OBJECTS:tools>
    $<TARGET_OBJECTS:kernel>
    $<TARGET_OBJECTS:widgets>
    $<TARGET_OBJECTS:dialogs>
    $<TARGET_OBJECTS:iconview>
    $<TARGET_OBJECTS:workspace>
    $<TARGET_OBJECTS:network>
    $<TARGET_OBJECTS:canvas>
    $<TARGET_OBJECTS:table>
    $<TARGET_OBJECTS:xml>
    $<TARGET_OBJECTS:opengl>
)

add_library(Qt::Qt2 ALIAS Qt2)

target_link_libraries(Qt2
    PRIVATE
		${FREETYPE_LIBRARIES}
		${FONTCONFIG_LDFLAGS}
		${FONTCONFIG_LIBRARIES}
        ${X11_LIBRARIES}
        ${JPEG_LIBRARIES}
        $<$<BOOL:XFT_FOUND>:Xft::Xft>
        $<$<BOOL:MNG_FOUND>:MNG::MNG>
        GLUT::GLUT
        OpenGL::GL
        PNG::PNG
        ZLIB::ZLIB
    )

if(ENABLE_THREAD_SUPPORT)
    target_link_libraries(Qt2
        PUBLIC
        Threads::Threads
        )
endif()

target_compile_definitions(Qt2
    PUBLIC
        ${QT_DEFINITIONS}
    )

if(MNG_FOUND)
    list(APPEND QT_DEFINITIONS QT_NO_IMAGEIO_MNG)
endif()

set_target_properties(Qt2 PROPERTIES
    OUTPUT_NAME qt2
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    LINK_FLAGS "-Wl,--as-needed -Wl,--no-undefined"
    )

target_include_directories(Qt2
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/qt2>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    PRIVATE
        ${FREETYPE_INCLUDE_DIRS}
        ${FONTCONFIG_INCLUDE_DIRS}
		${X11_INCLUDE_DIRS}
		${X11_X11_INCLUDE_PATH}
        ${JPEG_INCLUDE_DIRS}
    )

install(TARGETS Qt2 EXPORT Qt2Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/Qt2/Qt2ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

export(EXPORT Qt2Targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/Qt2/Qt2Targets.cmake"
    NAMESPACE Qt::
)

configure_file(${PROJECT_SOURCE_DIR}/cmake/Qt2Config.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/Qt2/Qt2Config.cmake"
    COPYONLY
)

install(EXPORT Qt2Targets
    FILE Qt2Targets.cmake
    NAMESPACE Qt::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Qt2
    )

install(FILES
    ${PROJECT_SOURCE_DIR}/cmake/Qt2Config.cmake
    ${PROJECT_SOURCE_DIR}/cmake/Qt2Utils.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/Qt2/Qt2ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Qt2
    COMPONENT Devel
)

