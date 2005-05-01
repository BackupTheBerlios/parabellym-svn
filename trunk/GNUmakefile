# Makefile for GNU Make.
# $Id: GNUmakefile 21 2005-01-17 22:56:02Z hex $

include Makedefs.faerion.include

DIRS = core # util docs/docbook
VERS = 1.0
PACK = parabellym

ifeq ($(FLAVOR),full)
  DIRS += stub/console mods/autotest
endif

custom-cleandist = custom-cleandist

include Makedefs.faerion

fake: all
	rm -rf fake-$(VERS)
	mkdir -p fake-$(VERS)/lib
	mkdir -p fake-$(VERS)/include/parabellym
	mkdir -p fake-$(VERS)/share/parabellym
	cp LICENSE fake-$(VERS)/share/parabellym
	cp core/api.h fake-$(VERS)/include/parabellym
	cp core/libparabellym.$(MODEXT) fake-$(VERS)/lib
ifeq ($(MDF_OSNAME),NT)
	cp core/libparabellym.$(MODEXT).a fake-$(VERS)/lib
else
	cp core/libparabellym.$(MODEXT) fake-$(VERS)/lib
endif
ifeq ($(FLAVOR),full)
	mkdir -p fake-$(VERS)/bin
	mkdir -p fake-$(VERS)/share/parabellym/modules
	cp stub/console/parabellym fake-$(VERS)/bin
	for i in `find mods -type f -name "*.$(MODEXT)"`; do cp $$i fake-$(VERS)/share/parabellym/modules; done
endif

package: fake
	cd fake-$(VERS) && tar cfz ../parabellym-$(VERS).tar.gz ./*
	rm -rf fake-$(VERS)

install: fake
	cp -R fake-$(VERS)/* $(PREFIX)/ && rm -rf fake-$(VERS)
ifeq ($(MDF_OSNAME),darwin)
	MDF_ICONV="$(MDF_ICONV)" DOCBOOKPATH="$(DOCBOOKPATH)" PACK="$(PACK)" VERS="$(VERS)" osdep/darwin/install
endif

run:
	@echo "Listing the modules that will be launched:"
	@ls -l $(PREFIX)/share/parabellym/modules/*.$(MODEXT)
	if [ -f parabellym.log ]; then echo -n '' > parabellym.log; fi
	$(PREFIX)/bin/parabellym $(wildcard $(PREFIX)/share/parabellym/modules/*.$(MODEXT))

custom-cleandist:
	rm -rf configure.log Makedefs.faerion.include
