#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([MobileC], 2.0.6, [dko@ucdavis.edu])
AC_CONFIG_SRCDIR([src/libmc.c])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([MobileC], 2.0.6)
AM_PROG_LIBTOOL
AC_CONFIG_MACRO_DIR([m4])

MC_VER_MAJ=2
MC_VER_MIN=0
MC_VER_REV=6

# Set up configure options
AC_ARG_ENABLE(security, 
    AC_HELP_STRING([--enable-security], [Enable the MobileC authentication and agent encryption.]),
    [use_security=$enableval], [use_security=no])

if test $use_security = yes; then
  AC_DEFINE(NEW_SECURITY, 1, [Enable the MobileC authentication and encryption])
fi

AC_ARG_ENABLE(readline,
    AC_HELP_STRING([--enable-readline], [Enable gnu readline support for the command line.]),
    [use_readline=$enableval], [use_readline=yes])

AC_ARG_ENABLE(bluetooth,
    AC_HELP_STRING([--enable-bluetooth], [Enable BlueZ Bluetooth stack support.]),
    [use_bluetooth=$enableval], [use_bluetooth=yes])

# touch 'NEWS' and 'COPYING' to trick automake.
touch NEWS
touch COPYING
    
# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB([dl], [dlopen])
AC_CHECK_LIB([mc], [MC_Start])
AC_CHECK_LIB([pthread], [pthread_create])
AC_CHECK_LIB([m], [hypot])
AC_CHECK_LIB([crypt], [crypt])
#AC_CHECK_LIB([curses], [keyok])
if test $use_bluetooth = yes; then
  AC_CHECK_LIB([bluetooth], [str2ba])
fi
if test $use_readline = yes; then
  AC_CHECK_LIB([readline], [readline])
fi

#Check for existence and location of Ch {{{
# We do not want to do this check if we are cross compiling!
if test "$host_alias" == ""; then 
    AC_MSG_CHECKING(for Ch)
    CHHOME=`echo $CHHOME`
    if [[ "$CHHOME" == "" ]] ; then
        CH=`which ch`
        if [[ "$CH" == "" ]] ; then
            AC_MSG_RESULT(no)
            AC_MSG_ERROR(Could not find Ch! Make sure you have Ch installed.)
        fi
            # 'ch' is a symlink.
#        LSWORDS=`ls -l $CH | wc -w`
#        LSWORDS='$'${LSWORDS}
            # get rid of stupid space
#        LSWORDS=`echo $LSWORDS | sed 's/ //'`
#        CHHOME=`ls -l $CH | awk "{print $LSWORDS}"`
#        CHHOME=`echo $CHHOME | sed 's/\([.]*\)\/bin\/ch$/\1/'`
        CHHOME=`ch -c 'echo $CHHOME'`
    fi
    AC_MSG_RESULT(yes)
    LDFLAGS="$LDFLAGS -L$CHHOME/extern/lib"
    CFLAGS="$CFLAGS -I$CHHOME/extern/include"
fi

AC_CHECK_LIB([embedch], [Ch_Abort])
AC_CHECK_LIB([chsdk], [__builtin_Ch_VaArg])
#FIXME: This is weird
#Apparently, mac systems like to prepend an extra underscore to their symbol
#names. Or, maybe this is a Ch inconsistency? In any case, the following seems
#to be necessary.
uname=`uname`
case "$uname" in
  Darwin*)
    LDFLAGS="-u ___builtin_Ch_VaArg $LDFLAGS"
    ;;
  *)
    LDFLAGS="-u __builtin_Ch_VaArg $LDFLAGS"
esac
#FIXME: The following is a dirty hack
#LDFLAGS="$LDFLAGS -L./src/mxml-2.2.2"
#LIBS="$LIBS -lmxml"
#CFLAGS="$CFLAGS -I./mxml-2.2.2"
# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_CHECK_TYPES(pthread_rwlock_t, [], [], [#include<pthread.h>])

# Variable Subsitutions
AC_SUBST(CHHOME)
AC_SUBST(MC_VER_MAJ)
AC_SUBST(MC_VER_MIN)
AC_SUBST(MC_VER_REV)

# Checks for library functions.
#AC_FUNC_MALLOC
#AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_STRTOD
AC_CHECK_FUNCS([gethostbyname gethostname memset socket strchr strstr strtol strtoul])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/mc_list/Makefile
                 src/mc_sync/Makefile
                 src/security/xyssl-0.9/library/Makefile
                 bin/Makefile
                demos/communication_with_other_FIPA_compliant_agents/mc_to_jade_example/Makefile
                demos/getting_started/hello_world/Makefile
                demos/getting_started/bluetooth/Makefile
                demos/cspace-agentspace_interface/persistent_example/Makefile
                demos/cspace-agentspace_interface/agent_init_callback/Makefile
                demos/miscellaneous/multiple_agency_example/Makefile
                demos/miscellaneous/steer_example/Makefile
                demos/miscellaneous/agent_squad_test/Makefile
                demos/miscellaneous/assembly_simulation/Makefile
                demos/miscellaneous/mc_max_num_agents_example/Makefile
                demos/miscellaneous/prompt_example/Makefile
                demos/miscellaneous/mc_sample_app/Makefile
                demos/miscellaneous/simple_send/Makefile
                demos/miscellaneous/migration_tests_20090522/Makefile
                demos/mobilec_security/agent_workgroup_example/Makefile
                demos/synchronization/cspace_mutex_example/Makefile
                demos/synchronization/agent_mutex_example/Makefile
                demos/synchronization/agent_semaphore_example/Makefile
                demos/synchronization/agent_cond_example/Makefile
                demos/synchronization/cspace_cond_example/Makefile
                demos/synchronization/mc_barrier_example/Makefile
                demos/agent_space_functionality/mc_df_example/Makefile
                demos/agent_space_functionality/mc_delete_agent_example/Makefile
                demos/agent_space_functionality/mc_df_service_test/Makefile
                demos/agent_space_functionality/agent_self_migrate/Makefile
                demos/agent_migration_message_format/multi_task_example2/Makefile
                demos/agent_migration_message_format/multi_data_retrieval/Makefile
                demos/agent_migration_message_format/mc_array_return_example/Makefile
                demos/agent_migration_message_format/mc_migration_message_example/Makefile
                demos/agent_migration_message_format/agent_saved_variables_example/Makefile
                demos/agent_migration_message_format/agent_datastate_example/Makefile
                demos/FIPA_compliant_ACL_messages/jade_to_mc_example/Makefile
                demos/FIPA_compliant_ACL_messages/fipa_protocol/Makefile
                demos/FIPA_compliant_ACL_messages/fipa_comm/Makefile
                demos/FIPA_compliant_ACL_messages/fipa_test/Makefile
                demos/composing_agents/multi_task_example/Makefile
                demos/composing_agents/mc_compose_agent_example/Makefile
                demos/binary_stationary_agents/stationary_agent_communication/Makefile
                demos/binary_stationary_agents/stationary_agent_demo/Makefile
                demos/mobilec_c-space_functionality/cspace_misc_examples/Makefile
                demos/mobilec_security/hello_world_secure/client/Makefile
                demos/mobilec_security/hello_world_secure/server/Makefile
								 mobilec.pc
])

AC_CONFIG_SUBDIRS([src/mxml-2.2.2])
AC_OUTPUT
