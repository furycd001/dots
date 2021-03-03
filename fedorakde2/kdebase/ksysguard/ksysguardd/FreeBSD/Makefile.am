CFLAGS = -Wall $(CONFIGFLAGS)
KSYSGUARDDRCFILE=@sysconfdir@/ksysguarddrc

INCLUDES = -DKSYSGUARDDRCFILE="\"$(KSYSGUARDDRCFILE)\"" -I$(srcdir)/../../CContLib -I$(srcdir)/../../CContLib

bin_PROGRAMS = ksysguardd

ksysguardd_SOURCES = CPU.c Command.c Memory.c ProcessList.c ksysguardd.c conf.c
ksysguardd_LDADD = -L../../CContLib -lccont -lkvm
