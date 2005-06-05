# Makefile for GNU Make.
# $Id: GNUmakefile 16 2004-12-05 21:19:18Z hex $

include ../../Makedefs.faerion.include

TARGET = library.shared
PROG   = libtest-send.$(MDF_SOEXT)
SRCS   = $(wildcard *.cc)
LIBS  += stdc++
DEFS  += VERSION_NUM="1,0,0,0"

ifeq ($(OSNAME),nt)
  SRCS += $(wildcard *.rc)
endif

ifeq ($(OSNAME),darwin)
  FRAMEWORKS += Parabellym
else
  LIBS += parabellym
endif

include ../../Makedefs.faerion
