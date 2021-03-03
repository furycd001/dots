function(create_kde2_config_header)
    include(CheckIncludeFiles)
    include(CheckFunctionExists)
    include(CheckStructHasMember)
    include(CheckCSourceCompiles)
    include(CheckSymbolExists)

    # TODO: need to check architecture or something
    set(HAVE_ALIGNED_DOUBLES True)
    set(HAVE_ALIGNED_LONGLONGS True)
    set(HAVE_LONG_LONG True)

    # TODO:
    set(HAVE_OSF_C2_PASSWD False)

    check_include_files(crypt.h HAVE_CRYPT_H)
    check_include_files(fstab.h HAVE_FSTAB_H)
    check_include_files(libdevinfo.h HAVE_LIBDEVINFO_H)
    check_include_files(limits.h HAVE_LIMITS_H)
    check_include_files(paths.h HAVE_PATHS_H)
    check_include_files(sensors/sensors.h HAVE_SENSORS_SENSORS_H)
    check_include_files(string.h HAVE_STRING_H)
    check_include_files(sys/socket.h HAVE_SYS_SOCKET_H)
    check_include_files(sys/select.h HAVE_SYS_SELECT_H)
    check_include_files(sys/sockio.h HAVE_SYS_SOCKIO_H)
    check_include_files(sys/stream.h HAVE_SYS_STREAM_H)
    check_include_files(sys/stropts.h HAVE_SYS_STROPTS_H)
    check_include_files(sys/time.h HAVE_SYS_TIME_H)
    check_include_files(sys/wait.h HAVE_SYS_WAIT_H)
    check_include_files(termio.h HAVE_TERMIO_H)
    check_include_files(termios.h HAVE_TERMIOS_H)
    check_include_files(vfork.h HAVE_VFORK_H)
    check_include_files(shadow.h HAVE_SHADOW)
    check_include_files(util.h HAVE_UTIL_H)

    # TODO: proper Find*
    check_include_files(Xm/Xm.h HAVE_MOTIF)
    check_include_files(utempter.h HAVE_UTEMPTER)

    # TODO: it's in bsd/
    check_include_files(libutil.h HAVE_LIBUTIL_H)

    check_function_exists("vsnprintf" HAVE_VSNPRINTF)
    check_function_exists("usleep" HAVE_USLEEP)
    check_function_exists("setpriority" HAVE_SETPRIORITY)
    check_function_exists("grantpt" HAVE_GRANTPT)
    check_function_exists("ptsname" HAVE_PTSNAME)
    check_symbol_exists("mkstemp" "stdlib.h" HAVE_MKSTEMP)
    check_symbol_exists("vsnprintf" "stdio.h" HAVE_VSNPRINTF)
    check_symbol_exists("mmap" "sys/mman.h" HAVE_MMAP)
    check_symbol_exists("vsyslog" "sys/syslog.h" HAVE_VSYSLOG)
    check_symbol_exists("waitpid" "sys/wait.h" HAVE_WAITPID)
    #check_symbol_exists("setpriority" "sys/mman.h" HAVE_SETPRIORITY)
    check_symbol_exists("kstat" "sys/stat.h" HAVE_KSTAT)

    check_struct_has_member("struct tm" tm_zone "time.h;sys/time.h" HAVE_TM_ZONE LANGUAGE C)

    check_cxx_source_compiles(
        "
            #define _XOPEN_SOURCE=500
            #include <stdlib.h>
            int main(int argc, char *argv[]) { unlockpt(0); }
        "
        HAVE_UNLOCKPT)

    # Couldn't get the more automatic things to work
    check_cxx_source_compiles("#include <sys/socket.h>
                                int main(int argc, char *argv[]) { struct ucred cred; }"
                                HAVE_STRUCT_UCRED)

    find_library(HAVE_LIBUTIL libutil.so)
    if (HAVE_LIBUTIL)
        list(APPEND CMAKE_REQUIRED_LIBRARIES util)
        check_function_exists("openpty" HAVE_OPENPTY)
    endif()

    if (OPENGL_FOUND)
        set(HAVE_GL True)
        check_include_files(GL/xmesa.h HAVE_GL_XMESA_H)
    endif()

    if (GLUT_FOUND)
        set(HAVE_GLUT True)
        check_include_files(GL/glut.h HAVE_GL_GLUT_H)
    endif()

    if (X11_ICE_FOUND)
        list(APPEND CMAKE_REQUIRED_LIBRARIES X11::ICE)
        check_symbol_exists("_ICETRANSNOLISTEN" "X11/ICE/ICElib.h" HAVE__ICETRANSNOLISTEN)
    endif()

    if (X11_dpms_FOUND)
        set(HAVE_DPMS True)
    endif()

    if (X11_Xinerama_FOUND)
        set(HAVE_XINERAMA True)
    endif()

    if (X11_Xkb_FOUND)
        set(HAVE_XKB True)
        list(APPEND CMAKE_REQUIRED_LIBRARIES X11::Xkb)
        #check_symbol_exists("XkbSetPerClientControls" "X11/XKBlib.h" HAVE_XKBSETPERCLIENTCONTROLS)
        check_function_exists("XkbSetPerClientControls" HAVE_XKBSETPERCLIENTCONTROLS)
    endif()

    if (X11_Xtst_FOUND)
        set(HAVE_XTEST True)
    endif()

    if (OPENSSL_FOUND)
        set(HAVE_SSL True)
    endif()

    if (VORBIS_FOUND)
        set(HAVE_VORBIS True)
    endif()

    if (LAME_FOUND)
        set(HAVE_LAME True)
    endif()

    if (PAM_FOUND)
        set(HAVE_PAM True)
    endif()
    if (PAM_FOUND)
        set(HAVE_VORBIS True)
    endif()

    configure_file(${PROJECT_SOURCE_DIR}/common/config.h.in ${PROJECT_BINARY_DIR}/config.h)
    include_directories(${PROJECT_BINARY_DIR})
    add_definitions(-DHAVE_CONFIG_H)
endfunction()

