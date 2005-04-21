# Makefile for GNU Make.
# $Id: GNUmakefile 16 2004-12-05 21:19:18Z hex $

include ../../Makedefs.faerion.include

TARGET = library.shared
PROG   = libtest-wait.$(MDF_SOEXT)
SRCS   = $(wildcard *.cc)
LIBS  += parabellym stdc++
DEFS  += VERSION_NUM="1,0,0,0"

ifeq ($(MDF_OSNAME),NT)
  SRCS += $(wildcard *.rc)
endif

include ../../Makedefs.faerion
