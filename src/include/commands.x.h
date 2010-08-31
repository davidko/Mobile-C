/* SVN FILE INFO
 * $Revision: 545 $ : Last Committed Revision
 * $Date: 2010-08-04 13:20:13 -0700 (Wed, 04 Aug 2010) $ : Last Committed Date */
/*[
 * Copyright (c) 2007 Integration Engineering Laboratory
                      University of California, Davis
 *
 * Permission to use, copy, and distribute this software and its
 * documentation for any purpose with or without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.
 *
 * Permission to modify the software is granted, but not the right to
 * distribute the complete modified source code.  Modifications are to
 * be distributed as patches to the released version.  Permission to
 * distribute binaries produced by compiling modified sources is granted,
 * provided you
 *   1. distribute the corresponding source modifications from the
 *    released version in the form of a patch file along with the binaries,
 *   2. add special version identification to distinguish your version
 *    in addition to the base release version number,
 *   3. provide your name and address as the primary contact for the
 *    support of your modified version, and
 *   4. retain our contact information in regard to use of the base
 *    software.
 * Permission to distribute the released version of the source code along
 * with corresponding source modifications in the form of a patch file is
 * granted with same provisions 2 through 4 for binary distributions.
 *
 * This software is provided "as is" without express or implied warranty
 * to the extent permitted by applicable law.
]*/

/* Format is:
   COMMAND("name", "cmd", "desc")
   */
COMMAND(QUIT, "quit", 
        "This command ends the MobileC application.")
COMMAND(HELP, "help", 
        "This command displays help for any command. Usage is:\n \
        \"help <command>\"")
COMMAND(SEND, "send", 
        "This command sends an agent XML file to a host. Usage is: \n \
        \"send <filename>\"\n \
        e.g. \"send test1.xml\"")
COMMAND(PRINT_CONNECTLIST,
        "print_connectlist",
        "This command prints the list entries in the connectList.")
COMMAND(PRINTLIST_MESSAGE, 
        "printlist_message",
        "This command prints all the list entries on the linked list:\n \
        message_queue")
COMMAND(PRINTLIST_AGENTS,
        "printlist_agents",
        "This command prints all the agents on the system which are still\n\
        on the agent list.\n")
COMMAND(FLUSH_AGENTS,
        "flush_agents",
        "This command flushes all of the agents on the system which are still\n\
        on the agent list.\n")
COMMAND(COMPOSE_SEND, "compose_send",
        "This command composes an agent from Ch source code and sends \n \
        it to a remote host. Usage is: \n \
        \"compose_send <filename> <target host> <target port>\"\n \
        e.g. \"compose_send agent_code.c iel.ucdavis.edu 5050\"")
