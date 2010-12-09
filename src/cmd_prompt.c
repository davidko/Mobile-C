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

/* Filename: cmd_prompt.c */

#include <stdio.h>
#ifndef _WIN32
#include <unistd.h>
#include "config.h"
#else
#include <windows.h>
#include "winconfig.h"
#endif
#include <stdlib.h>
#include <string.h>
#include "include/cmd_prompt.h"
#include "include/commands.h"
#include "config.h"

#ifdef HAVE_LIBREADLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

/* 
 * Readline / History Functions
 */
#ifdef HAVE_LIBREADLINE
int initialize_readline (void);
char ** command_completion (char* text, int start, int end);
char * command_generator (char* text, int state);

int initialize_readline (void)
{
  rl_readline_name = "$";
  rl_attempted_completion_function = (CPPFunction *)command_completion;
  return 0;
}

char ** command_completion (char* text, int start, int end)
{
  char **matches;
  matches = (char **)NULL;

  if (start == 0)
    matches = (char**)completion_matches (text, command_generator);

  return (matches);
}

char * command_generator (char* text, int state)
{
  static int list_index, len;
  char *name;

  if (!state)
  {
    list_index = 0;
    len = strlen (text);
  }

  while ((name = command_cmds[list_index]))
  {
    list_index++;

    if (strncmp (name, text, len) == 0)
      return (strdup(name));
  }

  return ((char *)NULL);
}

// STRIPWHITE ------------------------------------------------------------------
//   Strip whitespace from the start and end of STRING.
//   Return a pointer into STRING.
char* stripwhite (char* string)
{
  register char *s, *t;
  for (s = string; whitespace (*s); s++);
  if (*s == 0) return (s);
  t = s + strlen (s) - 1;
  while (t > s && whitespace (*t)) t--;
  *++t = '\0';
  return s;
}
// STRIPWHITE ------------------------------------------------------------------
#endif /* HAVE_LIBREADLINE */

cmd_prompt_p
cmd_prompt_Initialize(mc_platform_p mc_platform)
{
  cmd_prompt_p cmd_prompt;
  cmd_prompt = (cmd_prompt_p)malloc(sizeof(cmd_prompt_t));
  return cmd_prompt;
}

int
cmd_prompt_Destroy(cmd_prompt_p cmd_prompt)
{
  free(cmd_prompt);
  return MC_SUCCESS;
}

  void
cmd_prompt_Start( mc_platform_p mc_platform )
{
  cmd_prompt_p cmd_prompt = mc_platform->cmd_prompt;
#ifndef _WIN32
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  if(mc_platform->stack_size[MC_THREAD_CP] != -1) {
    pthread_attr_setstacksize
      (
       &attr,
       mc_platform->stack_size[MC_THREAD_CP]
      );
  }
#else
  int stack_size;
  if (mc_platform->stack_size[MC_THREAD_CP] < 1) {
    /* In windows, 0 is default, not min */
    stack_size = mc_platform->stack_size[MC_THREAD_CP]+1; 
  } else {
    stack_size = mc_platform->stack_size[MC_THREAD_CP];
  }
#endif
  THREAD_CREATE
    (
     &cmd_prompt->thread,
     cmd_prompt_Thread,
     (void*)mc_platform
    );
}

#ifndef _WIN32
  void*
cmd_prompt_Thread(void* arg) 
#else
  DWORD WINAPI
cmd_prompt_Thread( LPVOID arg )
#endif
{
    char *buf;
    command_t cmd;
    mc_platform_p mc_platform = (mc_platform_p)arg;
    cmd.index = COMMAND_COUNT;
    printf("\n");
#if !defined(HAVE_LIBREADLINE) || defined(_WIN32)
    buf = (char*)malloc(sizeof(char) * 100);
    if (!buf) {fprintf(stderr, "Malloc failed at %s:%d.\n", __FILE__, __LINE__); }
#endif

#ifdef HAVE_LIBREADLINE
    initialize_readline();
#endif
    while(1)
    {
        /* We want to repeatedly grab and execute the user's commands. */
#ifdef HAVE_LIBREADLINE
        buf = readline("MobileC > ");
        if (buf == NULL) {
          sleep(5);
          continue;
        }
        if (*buf) {
            add_history(buf);
        }
#else
        printf("MobileC > ");
#ifndef _WIN32
        if (!fgets(buf, 100, stdin)) 
#else
            if (!gets(buf))  /* FIXME, for the love of god FIXME */
#endif
            {
                fprintf(stderr, "fgets failed at %s:%d\n", __FILE__, __LINE__);
                Sleep(5000);
            }
#endif
        /* Get rid of newline character */
        if(strlen(buf) > 0) {
            if (buf[strlen(buf)-1] == '\n') 
            {
                buf[strlen(buf)-1] = '\0';
            }
        }
        cmd.num_args = split_string(&cmd.args, buf);
        process_command(&cmd);
        exec_command(cmd, mc_platform);
        dealloc_command(&cmd);
#ifdef HAVE_LIBREADLINE
        free(buf);
#endif
    }
    return 0;
} /*}}}*/

/* Function: split_string
   This function takes the address of a **char data type and a character
   string. It splits up the character string by spaces, placing each
   word into each successive arg. It automatically allocates the first
   argument. */
int split_string(char ***args, const char *buf) /*{{{*/
{
    /* First lets figure out how many args. */
    int toggle = 0;
    int num_args = 0;
    int i;
    int j;
    char *word;
    char *_buf;
    _buf = (char*)malloc(strlen(buf) * sizeof(char) + 1);
    strcpy(_buf, buf);
    for(i=0; i<(int)strlen(_buf); i++) 
    {
        if(_buf[i] != ' ') 
        {
            if(toggle == 0) 
            {
                toggle = 1;
                num_args++;
            }
        } 
        else 
        {
            toggle = 0;
        }
    }

    /* Now we have num_args */
    *args = (char **)malloc(sizeof(char *)*num_args);

    /* Now we must allocate space and copy each arg */
    j = 0;
    word = strtok(_buf, " ");
    while(word != NULL) 
    {  
        (*args)[j] = (char*)malloc(sizeof(char)*strlen(word)+1);
        strcpy((*args)[j], word);
        j++;
        word = strtok(NULL, " ");
    }
    free(_buf);

    return num_args;
} /*}}}*/

/* Function: process_command
   This function looks at the information inside cmd.args and
   fills out the cmd.cmd enum */
int process_command(command_t *cmd) /*{{{*/
{
    int i;
    if(cmd->num_args == 0) 
    {
        return 0;
    }
    for(i=0; i<COMMAND_COUNT; i++) 
    {
        if(!strcmp(cmd->args[0], command_cmds[i])) 
        {
            break;
        }
    }
    cmd->index = i;
    return 0;
} /*}}}*/
        
int exec_command(command_t cmd, mc_platform_p global) /*{{{*/
{
    if(cmd.num_args == 0) 
    { 
        return 0; 
    }
    if(cmd.index == COMMAND_COUNT) 
    {
        printf("Unknown command: %s\n", cmd.args[0]);
        printf("Type \"help\" for a listing of commands.\n");
        return 1;
    }

    return cmd_handlers[cmd.index](&cmd, global);
} /*}}}*/

int dealloc_command(command_t *cmd) /*{{{*/
{
    int i;
    for(i=0; i<cmd->num_args; i++) 
    {
        free(cmd->args[i]);
    }
    free(cmd->args);

    return 0;
} /*}}}*/

/* * * * * * * * * * * * * * * *
   command handlers begin here *
   * * * * * * * * * * * * * * */
int handler_QUIT(void *arg, mc_platform_p global) /*{{{*/
{
    MUTEX_LOCK(global->quit_lock);
    global->quit = 1;
    COND_BROADCAST (global->quit_cond);
    MUTEX_UNLOCK(global->quit_lock);
    return 0;
} /*}}}*/

int handler_HELP(void *arg, mc_platform_p global) /*{{{*/
{
    command_t *cmd = (command_t*)arg;
    int i;
    /* Find the command number */
    if(cmd->num_args > 1) 
    {
        for(i=0; i<COMMAND_COUNT; i++) 
        {
            if(!strcmp(command_cmds[i], cmd->args[1])) 
            {
                break;
            }
        }

        if(i == COMMAND_COUNT) 
        { 
            /* No such command */
            printf("Sorry, the command '%s' does not exist.\n", cmd->args[1]);
        } 
        else 
        {
            printf("%s\n", command_descriptions[i]);
        }
    } 
    else 
    {
        printf("For info about help, type \"help help\"\n");
        printf("Current commands are:\n");
        for(i=0; i<COMMAND_COUNT; i++) 
        {
            printf("%s\n", command_cmds[i]);
        }
    }

    return 0;
} /*}}}*/

int handler_SEND(void *arg, mc_platform_p global) /*{{{*/
{
    command_t* cmd = (command_t*)arg;
    if(cmd->num_args != 2) 
    {
        printf("%s\n", command_descriptions[COMMAND_SEND]);
        return 0;
    }
    return MC_SendAgentFile(NULL, cmd->args[1]);
} /*}}}*/
    
int handler_PRINT_CONNECTLIST(void *arg, mc_platform_p global) /*{{{*/
{
    connection_queue_Print(global->connection_queue);
    return 0;
} /*}}}*/

int handler_PRINTLIST_MESSAGE(void *arg, mc_platform_p global) /*{{{*/
{
    message_queue_Print(global->message_queue);
    return 0;
} /*}}}*/

int handler_PRINTLIST_AGENTS(void *arg, mc_platform_p global) /*{{{*/
{
    agent_queue_Print(global->agent_queue);
    return 0;
} /*}}}*/

int handler_FLUSH_AGENTS(void *arg, mc_platform_p global) /*{{{*/
{
    agent_queue_Flush(global->agent_queue);
    return 0;
} /*}}}*/

int handler_COMPOSE_SEND(void *arg, mc_platform_p global)
{
    /* args: filename, hostname, port */
    command_t* cmd = (command_t*)arg;
    MCAgent_t agent;
    char name[80];
    char server[200];
    if(cmd->num_args != 4) 
    {
        printf("%s\n", command_descriptions[COMMAND_COMPOSE_SEND]);
        return 0;
    }
    sprintf(name, "agent%d", rand()%1000);
    sprintf(server, "%s:%s", cmd->args[2], cmd->args[3]);
    agent = MC_ComposeAgentFromFile(
      name,
      global->hostname,
      name,
      cmd->args[1],
      NULL,
      server,
      0
      );
    if (agent != NULL) {
      MC_AddAgent(global->agency, agent);
      return 0;
    } else {
      return -1;
    }
}
