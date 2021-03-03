# This file is part of konqueror.

# konqueror is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# konqueror is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with konqueror.  If not, see <https://www.gnu.org/licenses/>.

# (C) 2018 by Helio Chissini de Castro <helio@kde.org>

KDE2_KIDL(KonquerorIface.h)

set(konqueror_SRCS
    KonquerorIface.cc
    KonqMainWindowIface.cc
    KonqViewIface.cc
    konq_mainwindow.cc
    konq_guiclients.cc
    konq_run.cc
    konq_view.cc
    konq_viewmgr.cc
    konq_misc.cc
    konq_frame.cc
    konq_actions.cc
    konq_profiledlg.cc
    konq_factory.cc
    konq_combo.cc
    konq_browseriface.cc
    KonquerorIface_stub.cpp
    KonquerorIface_skel.cpp
)
kde2_wrap_kidl(konqueror_SRCS
    SOURCES
        KonqMainWindowIface.h
        KonqViewIface.h
    )

qt2_wrap_cpp(konqueror_SRCS
    SOURCES
        konq_actions.h
        konq_browseriface.h
        konq_combo.h
        konq_frame.h
        konq_guiclients.h
        konq_mainwindow.h
        konq_profiledlg.h
        konq_run.h
        konq_view.h
        konq_viewmgr.h
    )

kde2_library(libkonqueror
    SOURCES
        ${konqueror_SRCS}
    HEADERS
        ${CMAKE_CURRENT_BINARY_DIR}/KonquerorIface_stub.h
    LIBS
        kde2::konq
        ZLIB::ZLIB
    )


add_executable(konqueror konq_main.cc)
install(TARGETS konqueror RUNTIME DESTINATION ${KDE2_BINDIR})

target_link_libraries(konqueror
    kde2::konqueror
    )

install(FILES konqueror.rc DESTINATION ${KDE2_DATADIR}/konqueror)
install(FILES konqueror.desktop konqueror_config.desktop DESTINATION ${KDE2_SERVICESDIR})

install(FILES
    konquerorsu.desktop kfmclient.desktop kfmclient_dir.desktop kfmclient_html.desktop
    DESTINATION ${KDE2_APPSDIR}/System)

install(FILES Home.desktop DESTINATION ${KDE2_APPSDIR}/)
install(FILES konqbrowser.desktop DESTINATION ${KDE2_APPSDIR}/Internet/)
install(FILES konqfilemgr.desktop DESTINATION ${KDE2_APPSDIR}/.hidden/)

set(PROFILEDIR ${KDE2_DATADIR}/konqueror/profiles)
install(FILES profile_webbrowsing.desktop DESTINATION ${PROFILEDIR}/webbrowsing)
install(FILES profile_filemanagement.desktop DESTINATION ${PROFILEDIR}/filemanagement)
install(FILES profile_midnightcommander.desktop DESTINATION ${PROFILEDIR}/midnightcommander)
install(FILES profile_filepreview.desktop DESTINATION ${PROFILEDIR}/filepreview)

add_subdirectory(kfmexec)
add_subdirectory(client)
add_subdirectory(iconview)
add_subdirectory(dirtree)
add_subdirectory(listview)
add_subdirectory(keditbookmarks)
add_subdirectory(kfindpart)
add_subdirectory(shellcmdplugin)
add_subdirectory(about)
add_subdirectory(pics)
add_subdirectory(sidebar)
