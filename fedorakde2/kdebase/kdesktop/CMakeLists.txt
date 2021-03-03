# This file is part of kdesktop.

# kdesktop is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# kdesktop is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with kdesktop.  If not, see <https://www.gnu.org/licenses/>.

# (C) 2018 by Helio Chissini de Castro <helio@kde.org>


add_subdirectory(pics)
#add_subdirectory(kwebdesktop)

KDE2_KIDL(KDesktopIface.h)

set(libkdesktop_SRCS
    xautolock.cc
    lockeng.cc
    lockdlg.cc
    pixmapserver.cc
    kcustommenu.cc
    startupid.cpp
    KDesktopIface_stub.cpp
)

set(kdesktop_SRCS
    krootwm.cc 
    init.cc
    kdiconview.cc
    minicli.cpp
    main.cc
    desktop.cc
    bgmanager.cc
    )

kde2_wrap_kidl(libkdesktop_SRCS
    SOURCES
        KBackgroundIface.h
        KDesktopIface.h
        KScreensaverIface.h
    )

qt2_wrap_cpp(kdesktop_SRCS
    SOURCES
        desktop.h
        krootwm.h
        kdiconview.h
        bgmanager.h

        kwebdesktop/kwebdesktop.h
        kwebdesktop/kwebdesktop.h
    )

qt2_wrap_cpp(libkdesktop_SRCS
    SOURCES
        kcustommenu.h
        kdmdesktop.h
        lockdlg.h
        lockeng.h
        minicli.h
        pixmapserver.h
        startupid.h
        xautolock.h

    ../kcontrol/background/bgrender.h
    )

kde2_library(libkdesktop
    SOURCES
        ${libkdesktop_SRCS}
    HEADERS
        ${CMAKE_CURRENT_BINARY_DIR}/KDesktopIface_stub.h
    LIBS
        kde2::kdecore
        kde2::kdeui
        kde2::kio
        kde2::kfile
        kde2::konq
        ZLIB::ZLIB
    )

add_executable(kdesktop ${kdesktop_SRCS})
install(TARGETS kdesktop RUNTIME DESTINATION ${KDE2_BINDIR})
target_link_libraries(kdesktop
    kde2::kdesktop
    kde2::bgnd
    )
#target_compile_definitions(kdesktop PUBLIC
#    PACKAGE="kdesktop"
#    VERSION="${PROJECT_VERSION}"
#    )

target_include_directories(kdesktop PRIVATE
    ../kcontrol/background/
    )

#target_link_libraries(kdesktop
#    kde2::kdecore
#    kde2::kdeui
#    kde2::kio
#    kde2::kfile
#    kde2::konq
#    ZLIB::ZLIB
#    Threads::Threads
#    )

# Match everything else, though I don't like the global autostart dir
install(FILES kdesktop.desktop DESTINATION ${KDEDIR}/share/autostart)
install(FILES kdesktop_custom_menu1 kdesktop_custom_menu2 DESTINATION ${KDE2_CONFDIR})

install(FILES
    programs/xearth.desktop
    programs/xglobe.desktop
    programs/xplanet.desktop
    DESTINATION ${KDE2_DATADIR}/kdesktop/programs)

install(FILES
    init/directory.trash
    init/directory.autostart
    init/directory.templates
    init/directory.desktop
    DESTINATION ${KDE2_DATADIR}/kdesktop/)

install(FILES
    init/Home.desktop
    DESTINATION ${KDE2_DATADIR}/kdesktop/DesktopLinks/)

install(FILES
    init/Templates/linkProgram.desktop
    init/Templates/linkURL.desktop
    init/Templates/linkFloppy.desktop
    init/Templates/linkHD.desktop
    init/Templates/linkCDROM.desktop
    init/Templates/Directory.desktop
    init/Templates/TextFile.desktop
    init/Templates/HTMLFile.desktop
    DESTINATION ${KDE2_TEMPLATESDIR})

install(FILES
    init/Templates/Program.desktop
    init/Templates/URL.desktop
    init/Templates/Floppy.desktop
    init/Templates/HD.desktop
    init/Templates/CDROM-Device.desktop
    init/Templates/TextFile.txt
    init/Templates/HTMLFile.html
    DESTINATION ${KDE2_TEMPLATESDIR}/.source/)

install(DIRECTORY
    DESTINATION ${KDE2_TEMPLATESDIR}/.source/emptydir)

install(FILES
    patterns/night-rock.desktop
    patterns/night-rock.jpg
    patterns/stonewall2.png
    patterns/stonewall2.desktop
    patterns/fish.desktop
    patterns/fish.png
    patterns/flowers.desktop
    patterns/flowers.png
    patterns/pavement.desktop
    patterns/pavement.png
    patterns/rattan.desktop
    patterns/rattan.png
    patterns/triangles.desktop
    patterns/triangles.png
    DESTINATION ${KDE2_DATADIR}/kdesktop/patterns/)
