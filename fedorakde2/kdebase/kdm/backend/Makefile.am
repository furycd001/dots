INCLUDES = $(all_includes) $(KERBEROS_INCS)
AM_CFLAGS = $(XDM_CFLAGS)
LDFLAGS = $(all_libraries) $(KDE_RPATH) $(KRB_RPATH)
LDADD = $(LIB_X11) -lXau $(LIBXDMCP) $(PASSWDLIBS) \
    $(LIB_LIBS) $(KERBEROS_LIBS) $(LIBSOCKET) $(LIBRESOLV) \
    $(LIBUCB) $(LIBUTIL)

bin_PROGRAMS = kdm
kdm_SOURCES = \
    access.c \
    auth.c \
    choose.c \
    client.c \
    daemon.c \
    dm.c \
    dpylist.c \
    error.c \
    file.c \
    genauth.c \
    krb5auth.c \
    mitauth.c \
    netaddr.c \
    policy.c \
    process.c \
    protodpy.c \
    reset.c \
    resource.c \
    rpcauth.c \
    server.c \
    session.c \
    socket.c \
    streams.c \
    util.c \
    xdmauth.c \
    xdmcp.c

EXTRA_DIST = printf.c

noinst_HEADERS = dm.h dm_socket.h dm_error.h dm_auth.h greet.h
