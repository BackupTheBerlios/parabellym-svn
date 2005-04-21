# Makefile for GNU Make.
# $Id: GNUmakefile 16 2004-12-05 21:19:18Z hex $

include ../../Makedefs.faerion.include

TARGET  = executable
PROG    = parabellym
SRCS   += $(wildcard *.cc)
DEFS   += VERSION_NUM="1,0,0,0"
LIBDIR += ../../core
LIBS   += faeutil parabellym stdc++

ifeq ($(MDF_OSNAME),NT)
  PROG := $(PROG).exe
  SRCS += $(wildcard *.rc)
endif

include ../../Makedefs.faerion

run: all
ifneq ($(MDF_OSNAME),NT)
	@echo "SymLinking modules..."
	@for i in $(shell find ../../mods -name 'lib*.$(MDF_SOEXT)'); do ln -fs $$i ./; done
	@echo "SymLinking core..."
	@ln -fs ../../core/libparacore.$(MDF_SOEXT) ./
endif
	@echo "Running $(PROG)."
	./$(PROG) lib*.$(MDF_SOEXT)

debug: all
	@echo "Running $(PROG)."
	PATH=$(PATH):'../../core' gdb ./$(PROG)
