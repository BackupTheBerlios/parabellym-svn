# Makefile for GNU Make.

TARGET  = docbook.paged
OBJDIR  = html
DOCBOOK = xhtml
DOCNAME = parabellym
DOCSUPP = parabellym.css
DEFS    = chapter.autolabel=1 html.stylesheet=parabellym.css chunk.section.depth=3 chunk.first.sections=1 use.id.as.filename=1 html.extra.head.links=1 toc.section.depth=3 section.autolabel=1 section.label.includes.component.label=1
DEPS    = $(wildcard *.dtd app.*.docbook)
SRCS    = parabellym.docbook
OBJDIR  = html.out

include ../../Makedefs.faerion
