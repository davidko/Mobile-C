/* file: mc_barrier_example/server.c */

#ifndef _WIN32
#include <libmc.h>
#else
#include <libmc.h>
#endif
#include <embedch.h>
#include <stdio.h>
#ifndef _WIN32
#include <unistd.h>
#endif

int main()
{
  MCAgency_t agency;
  MCAgent_t agent;
  ChInterp_t interp;
  MCAgencyOptions_t options;
  int i;
  int retval;
  int arg[2];
  int local_port=5050;
  /* We want _all_ the threads on: EXCEPT, the command prompt thread */
  MC_InitializeAgencyOptions(&options);
  for (i = 0; i < MC_THREAD_ALL; i++) {
    MC_SetThreadOn(&options, i);
  }
  /* If the following line is uncommented, the command prompt will
   * be turned off */
  /*MC_SetThreadOff(&options, MC_THREAD_CP);  */

  /* Init the agency */

  agency = MC_Initialize(
      local_port,
      &options);

  MC_MainLoop(agency);
  return 0;
}
