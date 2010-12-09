/************************************************************************
 *  Mobile C
 *  Integration and Engineering Laboratory http://iel.ucdavis.edu
 *  University of California, Davis 
 ************************************************************************/

MobileC is designed to execute ch scripts through the use of Mobile 
Agents. The data is encapsulated in xml packets and then sent through a 
network to the other systems where it is executed in that system's Ch 
interpreter.

**********************************************************
Directory Contents
**********************************************************
The Main directory of the program contains all files necessary in compiling 
and running the MobileC program. 

src           --- Contains the source code, Makefiles, and libraries needed to 
                  compile the library from its source.
demos         --- Contains tests and demo programs that will demonstrate the 
                  utility of the software.
docs          --- documentation of the MobileC C APIs. 
doxygen_guide --- doxygen automatic code documentation.
README        --- This file
Copyright     --- copyright license for Mobile-C

**********************************************************
System Requirements
**********************************************************

- Mobile-C is currently known to work on Linux (32-bit and 64-bit), Mac OS X, 
  FreeBSD 6.0, Windows, and Solaris 2.7.
- Ch version 6.3
- Embedded Ch version 6.3

**********************************************************
Getting Started
**********************************************************
This tutorial will assume that all necessary dependencies are installed 
correctly and function. The software you will need to run Mobile C are:

    (1) Ch : Obtain from http://www.softintegration.com
    (2) Embedded Ch: Obtain from http://www.softintegration.com

Mobile C Library Compilation For Windows
----------------------------------------------------------
The following steps are suggested to build the Mobile-C library.
- Ensure that your "_chrc" file is up to date. Specifically, make sure that the
  "_chrc" file options regarding the Visual C++ version and location are
  correct.
- Unpack the Mobile-C source code. Ensure that you have write permissions
  for the directory you are unpacking Mobile-C into, or you may encounter 
  compile-time errors.
- Open a Ch Terminal
- Navigate to the Mobile-C directory in the Ch terminal. For instance, if
  Mobile-C was unpacked into a directory called "C:\MobileC", type the command
  "cd C:\MobileC" in the Ch terminal.
- Execute the command "nmake -f makefile.win32". This will build Mobile-C and
  all of the demo programs located in the "demos" directory.

New users are advised to try the "hello world" demo located an the
"demos/getting_started/hello_world" directory. In order to run the demo, first
run the program "server.exe" in a terminal window. Then, in a second terminal
window, run the program "client.exe". The "client.exe" program sends a mobile
agent to the "server.exe" agency where it prints out a brief message. 

Mobile C Library Compilation For Unix
----------------------------------------------------------
The following commands compile and install the Mobile-C library into
the directory $HOME/sys. If you wish to install Mobile-C into the default
system directories, simply remove the '--prefix=$HOME/sys' argument from
the './configure' command on the first step. From the MobileC root 
directory, execute the following commands:

    ./configure --prefix=$HOME/sys
    make
    make install

These commands will build the Mobile-C library as well as all of the 
demonstration programs located in the 'demos' directory. For more
information about the demos, see 'demos/README' and the Mobile-C 
User's guide. Note that the demos are not installed anywhere special
during the 'make install' step, and are still located in the 'demos'
directory after 'make install'. Also note that the 'make install' step
is not necessary for the demos to run, but is suggested if you wish
to use the Mobile-C library to create your own programs.

New users are advised to try the "hello world" demo located an the
"demos/hello_world" directory. In order to run the demo, first run the program
"server" in a terminal window. Then, in a second terminal window, run the
program "client". The "client" program sends a mobile agent to the
"server" agency where it prints out a brief message. 

Documentation
----------------------------------------------------------
The official Mobile-C user's guide is included with this package in the
directory 'docs'. Furthermore, much of the Mobile-C source code
is Doxygen enabled. In order to generate the Doxygen documentation,
simply run the commands:
  cd doxygen_guide
  doxygen Doxyfile

If you do not have the Doxygen application, it may be obtained from
http://www.doxygen.org


**********************************************************
Release Notes for the Mobile-C
**********************************************************
Version MobileC v2.0.4, December 1, 2010
*)  Fixed a bug where mc_ComposeAgent calls a deprecated function.
*)  Fixed the mc_compose_agent_example demo.

Version MobileC v2.0.3.1, October 18, 2010
*)  Fixed small issue causing Mobile-C to mis-compile on Mac.

Version MobileC v2.0.3, September 16, 2010
*)  Fixed configuration files to allow Mobile-C to build on machines that do
    not have Bluetooth support. 

Version MobileC v2.0.2, August 31, 2010
*)  Added experimental Bluetooth support for Mobile-C agencies.

Version MobileC v2.0.1, August 6, 2010
*)  Fixed bug in function MC_SendAgent() where the agents would not be 
    sent to the correct destination.
*)  Updated some demos and documentation to use MC_SendAgent*() instead
    of MC_SendAgentMigrationMessage*(), which is now deprecated.

Version MobileC v2.0.0, June 17, 2010
*)  Major changes to the organization and presentation of the User's guide
*)  Added new API function to append tasks onto existing agents, called
    MC_AgentAddTask()
*)  Deprecated the MC_SendAgentMigrationMessage*() functions. All demos
    have been changed to use MC_SendAgentFile() instead.
*)  Reorganized the "demos" directory, and modified the entire project to use
    Windows Makefiles instead of "vcproj" files.

Version MobileC v1.10.12, May 14, 2010
*)  Added a new command to the Mobile-C command line called "compose_send".
    The new command composes a function out of a C file and sends it to
    a Mobile-C agency.
*)  Fixed a memory error regarding the storage of the local hostname. This
    would sometimes cause instability or crashes when using functions
    such as the 'compose_send' command from the Mobile-C command line.
*)  Fixed memory read error in Mobile-C command line.

Version MobileC v1.10.11, May 12, 2010
*)  Changed order of function arguments for secure ComposeAgent functions
*)  Added demo and documentation for MC_AddStationaryAgent() for treating
    binary space functions as Agents with full access to the Mobile-C agent
    api
*)  Fixed bugs regarding intepreter initializations and the api function
    MC_GetAllAgents

Version MobileC v1.10.10, March, 2010
*)  Renamed the 'ComposeAgent' family of functions to 'ComposeAgentS'. Reverted
    the API functions argument list to old form, without workgroup codes. This
    was done to preserve backward compatability with Mobile-C versions 1.10.8 
    and prior.

Version MobileC v1.10.9, January 6, 2010
*)  Improved performance of the Mobile-C Networking code. Agent migrations
    now occur at an improved rate.
*)  Improved performance of running a large number of agents at a time.
*)  Added conceptual agent "workgroups" where each agent may store a secret
    "workgroup code". Only agents that share a certain workgroup code may 
    perform certain operations on each other, such as agent deletion and
    data retrieval. This prevents random, malicious agents from interacting 
    with workgroup-enabled agents in a bad way.
*)  Mobile-C now pre-initialized a number of interpreters for increased 
    performance. Number of preloaded interpreters may be adjusted in the 
    Mobile-C Options structure passed into MC_Initialize().
*)  Numerous bugfixes.

Version MobileC v1.10.8, April 29, 2009
*)  Fixed bug where if an agent has multiple tasks on the same host (in
    succession), the agent will be renamed due to a spurious name collision.
*)  Added MC_MigrateAgent() and mc_MigrateAgent(). These functions allow an 
    agent to dynamically choose where and when it wants to migrate from 
    within the agent code space.
*)  Added new Security Module written by Najmus Malik. Use the 
    "--enable-security=yes" option during the configure step to enable the
		security module. See the demos in demos/hello_world_secure and
		demos/multi_task_example_secure , as well as Chapter 9 in the
		User's guide.

Version MobileC v1.10.7, January 9, 2009
*)  Changed the way agent migration message files are parsed and sent. Before,
    the files were not parsed at all before being sent. Now, agent xml files are
    fully parsed before being sent. This means that the syntax checking for the
    agent xml files will be more strict. Some agent xml files in the demos have
    been updated to reflect this.
*)  Added more documentation regarding persistent agents and the Ch Mobile-C
    Package, which is required for agent FIPA ACL communication.

Version MobileC v1.10.6, December 12, 2008
*)  Changed the way agent migration message files are sent. The XML files are
    now fully parsed into agents before being sent.
*)  Fixed a number of memory leaks.

Version MobileC v1.10.5, November 12, 2008
*)  Fixed MC_RetrieveAgent() : The function no longer returns a shallow copy 
    of the retrieved agent, but a pointer to the actual agent.
*)  Added a new demo example at "demos/multi_data_retrieval"

Version MobileC v1.10.4, October 24, 2008
*)  Added function MC_CallAgentFunc which takes a variable number of
    arguments.

Version MobileC v1.10.3, July 31, 2008
*)  Added documentation about Mobile-C and 64 bit architecture machines.
*)  Added example for MC_ComposeAgent API function at 
    demos/mc_compose_agent_example/

Version MobileC v1.10.2, July 11, 2008
*)  Fixed a bug regarding large agents and http/tcp protocol causing EOF 
    errors, particularly on Gumstix systems. 
*)  Added FIPA enumerations to libmc.h so that they may be accessed by 
    including only libmc.h.
*)  Updated documentation to reflect previous change.
*)  Upgraded included xyssl package from version 0.7 to 0.9. This fixes
    a Mac compile-time error regarding xyssl.

Version MobileC v1.10.1, July 1, 2008
*)  Mobile-C now communicates successfully with JADE agencies. See the demo
    located at demos/mc_to_jade_example/ . 
*)  Bugfixes regarding FIPA message format and compatibility with JADE
*)  Other minor bugfixes

Version MobileC v1.10.0, June 16, 2008
!!  Note: v1.10.0 agents are NOT backwards compatible with previous versions
    of Mobile-C. See note below for details. 
*)  Added API to implement FIPA compliant ACL communication. New functions 
    include 
      MC_AclDestroy()
      MC_AclNew()
      MC_AclPost()
      MC_AclReply()
      MC_AclRetrieve()
      MC_AclSend()
      MC_AclWaitRetrieve()
      MC_AclSetPerformative()
      MC_AclSetSender()
      MC_AclAddReceiver()
      MC_AclAddReplyTo()
      MC_AclSetContent()
*)  Added API function to load agents directly from an xml file into a local
    agency:
      MC_LoadAgentFromFile()
*)  Modified Agent XML format. Please consult the user's guide for specific
    details. The new agents will not be backward compatible with older 
    agencies.
*)  Added ability for agents to carry persistent data with them from task
    to task. See details in API functions mc_AgentVariableRetrieve() and
    mc_AgentVariableSave().
*)  Preliminary .NET Binding implemented by Doug Stark <dpstark@sandia.gov>.
*)  Preliminary testing with Mobilec-C <-> JADE communication using HTTP
    protocol. 

Version MobileC v1.9.5, Apr. 25 2008
*) Removed command prompt from many demo client programs.
*) Added support for parsing and handling FIPA compliant ACL messages.
*) Added numerous API function calls to handle the processing of ACL messages,
   such as MC_AclSend, MC_AclRetrieve, MC_AclReply, etc.
*) Documentation and general polishing of the library, moving towards 2.0.0
   release.

Version MobileC v1.9.4, Jan. 7, 2008
*) Added many functions that were available in Binary space to 
   agent space as well. Some example include:
    mc_GetAgentID(),
    mc_GetAgentName(),
    mc_End(),
    mc_HaltAgency(),
    mc_ResumeAgency(), among others.
*) Added missing documentation for some agent-space functions.

Version MobileC v1.9.2, Sept. 20, 2007
*) Some major updates to the documentation.
*) Added function MC_CopyAgent() to deep-copy agents.
*) Fixed a few bugs with MC_WaitRetrieveAgent and MC_WaitSignal().

Version MobileC v1.9.1, Sept. 12, 2007
*) Updated Windows version. Rudimentary testing of all demos in Windows
   is a success.
*) Ran everything through a memory leak checker again. Some memory leaks
   and errors fixed.
*) Created two seperate Windows project directories for VCNET 2003 and 
   VCNET 2005.
*) Added new agent feature: Agents may now contain seperate blocks of code
   a la version 1.0. See agents in 'demos/multi_task_example' for an 
   example.

Version MobileC v1.9.0, Aug. 30, 2007
*) Reorganized and rewrote 90% of old code.
    - Renamed nearly all of the old data structures and functions.
    - Code now more closely adheres to unified style guide. (mostly)
    - Files are more clearly named and organized.
    - Rewrote XML parser
    - Reorganized agent XML format
    - Extensive use of C macros for declaring/defining multiple 
      queue objects. This is good and bad: Makes the code a little
      weirder to read, but saves a lot of debugging trouble, as 
      only the macros need to be modified if there is any bug 
      with the queues.
*) Added simple symmetric encryption option for added security. 
*) Added MobileC Barriers: Execution based on MPI Barriers. See
   demos/agent_squad_test
*) Numerous bugfixes.

Version MobileC v1.8.1, Mar. 25, 2007
*) Added MobileC condition variable api functions:
   MC_CondDelete
   MC_CondInit
   MC_CondSignal
   MC_CondWait, as well as their agent-space counterparts.
*) Added agent API. All agent api functions start with 'mc_FuncName',
   opposed to the library api which start with 'MC_FuncName'. Agent
   api functions added are:
   mc_FindAgentByName
   mc_TerminateAgent
   mc_GetAgentStatus
   mc_PrintAgentCode
   mc_RetrieveAgentCode
   mc_SetAgentStatus
   mc_SendACLMessageFile
   mc_SendACLMessage
*) Added C API functions
   MC_WaitSignal
   MC_End
*) Combined functions MC_Start and MC_HandleInit into one 
   function: MC_Initialize

Version MobileC v1.8, Feb. 5, 2007
*) The standalone build option is now deprecated. The functionality 
   the standalone Mobile-C program is now entirely emulated by the
   mc_sample_app application, located in demos/libmc/mc_sample_app/.
   By enabling the command prompt thread (CP_THREAD), mc_sample_app
   execution becomes identical to the old standalone MobileC app.
*) Agents may now specify their own initial execution status
   with the 'status' attribute in the <DATA> node.
*) All configure scripts now use GNU Autotools and libtool.
*) Removed standalone compile option. 'demos/libmc/mc_sample_app' 
   offers identical functionality using the MobileC library, libmc.
*) Changed naming conventions of MobileC API functions: 
   i.e. mc_handle_init() is now MC_HandleInit() .
*) Added CDATA section in XML Agent files to hold source code.
*) Added agent persistence
*) Added agent-agency communication and preliminary agent api

Version MobileC v1.7, November 6, 2006
*) Windows version has been updated. Numerous bugfixes
*) Abstraction of mutex code and other OS dependant code into macros,
    LOCK_MUTEX(), UNLOCK_MUTEX(), etc. See src/includes/global.h

Version Mobile v1.6, August 31, 2006
*) MobileC C APIs are added and tested on Linux machines
*) Inside function AgentRunChScriptThread() in file src/mob_agent.c,
   result = (double *)Ch_SymbolAddrByName() has been changed to
   result = *(double **)Ch_SymbolAddrByName() in order to access a
   global array declared in a Ch script. 
   A Ch script indicates a mobile agent code.
*) Inside function MessgQueueInitialize() in file src/messg_queue.c,
   the wrong memory allocation for the mutex variable has been corrected, i.e. 
   mqueue->messg_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_t)); 
   has been changed to  
   mqueue->messg_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
*) A new thread, startAMSCreateAgentThread() in file src/ap_ams.c, 
   has been added to parse through the message list and create new agents 
   for the system.  

Version Mobile v1.5, May 11, 2006
*) Ported to run in Sun Solaris by David Ko <dko@ucdavis.edu>

Version Mobile v1.5, May 4, 2006
*) ported to run in Windows by David Ko <dko@ucdavis.edu>
   Find the windows executable inside the src/bin/ directory.
   Find the windows Visual .NET project file at 
   src/MobileC_v1.5_win32/MobileC_v1.5_win32.sln

Version Mobile v1.1, March 9, 2006 
*) ported to run in FreeBSD by David Ko <dko@ucdavis.edu>
*) Default src/Makefile now works in both FreeBSD and Linux.
   Note:   
   (1) Compile from within a Ch shell for best results. Otherwise, 
   please set the environment variable CHHOME to point to the  
   ch directory. (usually /usr/ch or /usr/local/ch). 
   (2) The pkg-config utility must be installed in order for the 
      Makefile to work.

Version MobileC v1.0, February 15, 2006
*) the original release for running in Linux and Mac OS X

