#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([smart-bus], [0.01], [general_zyj@163.com])
AM_INIT_AUTOMAKE(smart-bus, 0.01)
AC_CONFIG_SRCDIR([src/main/main.cpp])
# AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
# AC_PROG_CC
AC_PROG_INSTALL

# Checks for pthread libraries.
AC_SEARCH_LIBS([pthread_create], [pthread])
AC_SEARCH_LIBS([xmlStrcmp], [xml2])
AC_SEARCH_LIBS([shm_open], [rt])
AC_SEARCH_LIBS([json_object_from_file], [json])
# AC_CHECK_LIB([pthread], [pthread_create])
AC_PROG_RANLIB
# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_PID_T

# Check for Macro defines
# RT_LIB
# AC_ARG_WITH([rtlib],
#             [AS_HELP_STRING([--with-rtlib],
#               [Add support for rt lib])],
#             [],
#             [with_rtlib=no])
          
# SYSLIBS =
# AS_IF(
#     [test "x$with_rtlib" != xno],
#     [AC_SUBST([SYSLIBS], [("-lrt")])
#             AC_DEFINE([RT_LIB], [1], [Define if we want to use rt_lib ])
#     ])
#    [AC_SEARCH_LIBS([shm_open], [rt], 
#        [],
#        [AC_MSG_FAILURE([rtlib test failed (--without-rtlib to disable)])], [])]

# RT_PREMPT
AC_ARG_WITH([rtprempt],
            [AS_HELP_STRING([--with-rtprempt],
              [Add support for rt patch])],
            [],
            [with_rtprempt=no])
          
# RTLIB=
# AS_IF(
#     [test "x$with_rtprempt" != xno],
#     [AC_SEARCH_LIBS([clock_nanosleep], [rt], 
#         [AC_SUBST([RTLIB], ["-lrt"])
#             AC_DEFINE([RT_PREMPT], [1], [Define if we want to use rt_time patch])],
#         [AC_MSG_FAILURE([rtlib: clock_nanosleep test failed (--without-rtprempt to disable)])], [])]
#     )

# BOARD 
AC_ARG_WITH([board],
            [AS_HELP_STRING([--with-board],
              [Add support for app runing in dev-board])],
            [],
            [with_board=no])

# 3rd-party libs
THIRD_PARTY_STATIC_LIBS_PATH = 
THIRD_PARTY_DYN_LIBS =
# [AC_DEFINE([BOARD], [1], [Define if we are running app in dev-board]) AC_SUBST(LDFLAGS,"--static ${src_path}/lib/third-party/libmodbus.a")],
#   
AS_IF(
    [test "x$with_board" != xno],
    [AC_DEFINE([BOARD], [1], [Define if we are running app in dev-board]) AC_SUBST(THIRD_PARTY_DYN_LIBS,("-L${src_path}/lib/third-party")) AC_SUBST(THIRD_PARTY_STATIC_LIBS_PATH,("${src_path}/lib/third-party"))]
    [test "x$with_board" == xno],
    [AC_DEFINE([HOST_PC], [1], [Define if we are running app in host-pc]) AC_SUBST( THIRD_PARTY_DYN_LIBS,("-L${src_path}/lib/third-party/PC")) AC_SUBST(THIRD_PARTY_STATIC_LIBS_PATH,("${src_path}/lib/third-party/PC"))]
    )

# DEBUG MSG 
AC_ARG_WITH([print],
            [AS_HELP_STRING([--with-print],
              [Add support for debug printing])],
            [],
            [with_prt=no])
          
AS_IF(
    [test "x$with_prt" != xno],
    [AC_DEFINE([DEBUG_MSG], [1], [Define if we are printing debug msg])],
    [test "x$with_prt" == xno],
    [AC_DEFINE([DEBUG_MSG], [0], [Define if we are not printing debug msg])]
    )

# DEBUG or RELEASE 
AC_ARG_WITH([assert],
            [AS_HELP_STRING([--with-assert],
              [Add support for assert mode])],
            [],
            [with_assert=no])
          
AS_IF(
    [test "x$with_assert" != xno],
    [AC_DEFINE([ASSERT_ON], [1], [Define if we are in assert mode])]
    [test "x$with_assert" == xno],
    [AC_DEFINE([ASSERT_OFF], [1], [Define if we are not in assert mode])]
    )

# TEST 
# AC_ARG_WITH([testmacro],
#             [AS_HELP_STRING([--with-testmacro],
#               [Add support for macro])],
#             [],
#             [with_testmacro=no])
#           
# AS_IF(
#     [test "x$with_testmacro" != xno],
#     [AC_DEFINE([TEST_MACRO], [1], [Define if we want to use macro])]
#     )

# Checks for library functions.
AC_CHECK_FUNCS([clock_gettime])

AC_CONFIG_FILES([
                 Makefile
                 src/Makefile
                 src/main/Makefile
                 src/os/Makefile
                 src/cmn/Makefile
                 src/cmn/db/Makefile
                 src/cmn/pc/Makefile
                 src/cmn/utils/Makefile
                 src/cmn/omc/Makefile
                 src/app/Makefile
                 src/app/gps/Makefile
                 src/app/led/Makefile
                 src/app/hmi/Makefile
                 src/app/dvr/Makefile
                 src/app/test/Makefile
                 src/adp/Makefile
                 src/adp/gps/Makefile
                 src/adp/led/Makefile
                 src/adp/hmi/Makefile
                 src/adp/dvr/Makefile
                 src/drv/Makefile
                 src/drv/gps/Makefile
                 src/drv/led/Makefile
                 src/drv/serial/Makefile
                 src/drv/spi/Makefile
                 lib/Makefile
                 ])

# VAR DEFINE

AC_OUTPUT
