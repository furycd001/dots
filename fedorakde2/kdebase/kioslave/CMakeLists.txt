kde2_module(kio_gopher SOURCES gopher/gopher.cc LIBS kde2::kio kde2::ksycoca)
install(FILES gopher/gopher.protocol DESTINATION "${KDE2_SERVICESDIR}")

kde2_module(kio_filter SOURCES filter/filter.cc LIBS kde2::kio kde2::ksycoca)
install(FILES filter/bzip2.protocol filter/bzip.protocol filter/gzip.protocol DESTINATION "${KDE2_SERVICESDIR}")

kde2_module(kio_tar SOURCES tar/tar.cc LIBS kde2::kio kde2::ksycoca)
install(FILES tar/tar.protocol DESTINATION "${KDE2_SERVICESDIR}")

add_subdirectory(thumbnail)
add_subdirectory(man)

#add_subdirectory(info)
#add_subdirectory(nfs)
#add_subdirectory(nntp)
#add_subdirectory(pop3)
#add_subdirectory(imap4)
#add_subdirectory(smbro)
#add_subdirectory(finger)
#add_subdirectory(ldap)
#add_subdirectory(smb)

# Needs cdparanoia, libvorbis and lame at least
#add_subdirectory(audiocd)

# Needs mtools, which it just calls
#add_subdirectory(floppy)
