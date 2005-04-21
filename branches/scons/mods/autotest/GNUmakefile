# Makefile for GNU Make.
# $Id: GNUmakefile 16 2004-12-05 21:19:18Z hex $

include ../../Makedefs.faerion.include

TARGET = library.shared
PROG   = autotest.$(MDF_SOEXT)
SRCS   = $(wildcard *.cc)
LIBS  += faeutil parabellym stdc++

ifeq ($(MDF_OSNAME),NT)
  SRCS += $(wildcard *.rc)
endif

include ../../Makedefs.faerion
