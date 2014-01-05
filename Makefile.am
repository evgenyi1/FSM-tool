# Makefile.am -- Process this file with automake to produce Makefile.in

INCLUDES = -I$(top_srcdir)/..

#if DEBUG
#DBGFLAGS = -ggdb -D_DEBUG_
#else
DBGFLAGS = -g
#endif

CFLAGS = @CFLAGS@ $(CFLAGS_FSMTOOL_COMMON) 

lib_LTLIBRARIES = libFsmFramework.la

libFsmFramework_la_SOURCES = fsm_tool_framework.c

libFsmFramework_la_LIBADD=  -ldl 
