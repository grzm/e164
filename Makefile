# E164 Type contrb module Makefile
# $Id: Makefile 53 2007-09-10 01:13:48Z glaesema $

MODULES = e164
DATA_built = e164.sql
DOCS = README.e164
REGRESS = e164

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS = $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/e164
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif

