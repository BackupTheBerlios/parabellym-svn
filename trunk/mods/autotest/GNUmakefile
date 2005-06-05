# Makefile for GNU Make.
# vim:ts=2:sts=2:sw=2:et:ft=make
# $Id: GNUmakefile 16 2004-12-05 21:19:18Z hex $

include ../../Makedefs.faerion.include

TARGET = library.shared
PROG   = autotest.$(MDF_SOEXT)
SRCS   = $(wildcard *.cc)
LIBS  += faeutil stdc++

ifeq ($(OSNAME),nt)
  SRCS += $(wildcard *.rc)
endif

ifeq ($(OSNAME),darwin)
  FRAMEWORKS += Parabellym
else
  LIBS += parabellym
endif

include ../../Makedefs.faerion
