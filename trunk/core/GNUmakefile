# Makefile for GNU Make.
# $Id: GNUmakefile 16 2004-12-05 21:19:18Z hex $

include ../Makedefs.faerion.include

TARGET = library.shared
PROG   = libparabellym.$(MDF_SOEXT)
SRCS  += $(wildcard *.cc ../util/*.cc)
LIBS  += stdc++
EXPS   = exports
DEFS  += PARA_BUILD_CORE VERSION_NUM="1,0,0,0" MODEXT=\"$(MDF_SOEXT)\"
VERS   = 1.0

ifeq ($(MDF_OSNAME),NT)
  SRCS += core.rc
  LIBS += pthreadGCE
else
  LIBS += pthread
endif

ifeq ($(MDF_OSNAME),Darwin)
  MDF_LIBCMD += -install_name /Library/Frameworks/Parabellym.framework/Parabellym -compatibility_version $(VERS) -current_version $(VERS)
endif

include ../Makedefs.faerion
