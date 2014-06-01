# Makefile.am -- Process this file with automake to produce Makefile.in

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/include -I$(srcdir)

DBGFLAGS = -g

CFLAGS = @CFLAGS@ $(CFLAGS_FSMTOOL_COMMON) 

lib_LTLIBRARIES = libFsmFramework.la

libFsmFramework_la_SOURCES = fsm_tool_framework.c

libFsmFramework_la_LIBADD= -ldl 

libFsmFramework_apiincludedir = $(includedir)/FSM_Tool
libFsmFramework_apiinclude_HEADERS = \
        $(top_srcdir)/fsm_tool_framework.h

bin_PROGRAMS = fsmexample

fsmexample_SOURCES = fsm_example.c fsm_tool_simulator.c

fsmexample_LDADD = 	-L. -lFsmFramework \
                    -ldl
