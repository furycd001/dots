# This file is part of kdebase.

# kdebase is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# kdebase is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with kdebase.  If not, see <https://www.gnu.org/licenses/>.

# (C) 2018 by Helio Chissini de Castro <helio@kde.org>

cmake_minimum_required(VERSION 3.12.0)

cmake_policy(SET CMP0075 NEW)

project(kdebase VERSION 0.3.0 LANGUAGES C CXX)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

option(ENABLE_SANITIZERS "Enable runtime sanitizing (for development)")
if (ENABLE_SANITIZERS)
    message("Enabling asan and ubsan")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
endif()


find_package(PAM)

#TODO or something
set(KDEDIR ${CMAKE_INSTALL_PREFIX})

if (PAM_FOUND)
    add_definitions(-DHAVE_PAM)
    add_definitions(-DUSE_PAM)

    # Some things default to xdm, which doesn't exist anymore
    # TODO: use the config.h crap
    add_definitions(-DKSCREENSAVER_PAM_SERVICE=\"system-auth\")
    add_definitions(-DKCHECKPASS_PAM_SERVICE=\"system-auth\")
endif()

find_package(DCOP REQUIRED)

find_package(kdelibs2 REQUIRED)
find_package(arts REQUIRED)
find_package(qtmcop REQUIRED)

find_package(kdecore REQUIRED)
find_package(kdeui REQUIRED)
find_package(kdesu REQUIRED)
find_package(kssl REQUIRED)
find_package(kio REQUIRED)
find_package(ksycoca REQUIRED)
find_package(kfile REQUIRED)
find_package(kparts REQUIRED)
find_package(kdeprint REQUIRED)
find_package(kdeprint_management REQUIRED)
find_package(kjava REQUIRED)
find_package(khtml REQUIRED)
find_package(kdefakes REQUIRED)
find_package(kscreensaver REQUIRED)
find_package(kspell REQUIRED)
find_package(ktexteditor REQUIRED)
find_package(kdedmodule REQUIRED)

find_package(artskde REQUIRED)

find_package(LAME)
find_package(OggVorbis)

include(kde2_configure)
create_kde2_config_header()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common/)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(libkonq)

add_subdirectory(applnk)
#add_subdirectory(doc)
add_subdirectory(drkonqi)
#add_subdirectory(kaddressbook)
#add_subdirectory(kappfinder)
add_subdirectory(kate)
add_subdirectory(kcheckpass)
add_subdirectory(kcontrol)
add_subdirectory(kdcop)
add_subdirectory(kdebugdialog)
add_subdirectory(kdeprint)
add_subdirectory(kdesktop)
add_subdirectory(kdesu)
#add_subdirectory(kdm)
add_subdirectory(khelpcenter)
add_subdirectory(khotkeys)
add_subdirectory(kicker)
add_subdirectory(kioslave)
add_subdirectory(klipper)
add_subdirectory(kmenuedit)
add_subdirectory(konqueror)
add_subdirectory(konsole)
add_subdirectory(kpager)
add_subdirectory(kpersonalizer)
add_subdirectory(kreadconfig)
add_subdirectory(kscreensaver)
add_subdirectory(ksmserver)
add_subdirectory(ksplash)
add_subdirectory(kstart)
add_subdirectory(ksysguard)
add_subdirectory(ktip)
add_subdirectory(kwin)
add_subdirectory(kxkb)
add_subdirectory(kxmlrpc)
add_subdirectory(l10n)
#add_subdirectory(legacyimport)
#add_subdirectory(nsplugins)
add_subdirectory(pics)

install(FILES
    startkde
    PERMISSIONS WORLD_EXECUTE WORLD_READ
    DESTINATION ${KDE2_BINDIR}
    RENAME startkde2
    )
install(FILES kde2.desktop DESTINATION share/xsessions)
