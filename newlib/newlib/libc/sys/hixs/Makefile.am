## Process this file with automake to generate Makefile.in

## AUTOMAKE_OPTIONS = cygnus

INCLUDES = $(NEWLIB_CFLAGS) $(CROSS_CFLAGS) $(TARGET_CFLAGS)

noinst_LIBRARIES = lib.a

AM_CCASFLAGS = $(INCLUDES)

lib_a_SOURCES = syscallstubs.c
lib_a_CCASFLAGS = $(AM_CCASFLAGS)
lib_a_CFLAGS = $(AM_CFLAGS)


all-local: crt0.o

ACLOCAL_AMFLAGS = -I ../../.. -I pillesnoppen
CONFIG_STATUS_DEPENDENCIES = $(newlib_basedir)/configure.host
