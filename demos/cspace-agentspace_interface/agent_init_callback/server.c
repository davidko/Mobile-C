/* File: hello_world/server.c */

#include <stdio.h>
#include <libmc.h>
#include <embedch.h>

int agentCallbackFunc(ChInterp_t interp, MCAgent_t agent, void* user_data);
EXPORTCH double mult_chdl(void* varg);

int main() 
{
  MCAgency_t agency;
  int local_port = 5051;
  setbuf(stdout, NULL);
  
  agency = MC_Initialize(local_port, NULL);
  MC_AddAgentInitCallback(agency, agentCallbackFunc, NULL);

  MC_MainLoop(agency);

  MC_End(agency);
  return 0;
}

/* This callback function is called during the initialization step of each
 * incoming agent. We will add a c-space function to each interpreter that
 * the agent will be able to call. */
int agentCallbackFunc(ChInterp_t interp, MCAgent_t agent, void* user_data)
{
  Ch_DeclareFunc(interp, "double mult(double x, double y);", (ChFuncdl_t)mult_chdl);
  return 0; /* 0 for success error status */
}

EXPORTCH double mult_chdl(void* varg)
{
  double retval;
  double x, y;
  ChInterp_t interp;
  ChVaList_t ap;

  Ch_VaStart(interp, ap, varg);
  x = Ch_VaArg(interp, ap, double);
  y = Ch_VaArg(interp, ap, double);
  retval = x * y;
  Ch_VaEnd(interp, ap);
  return retval;
}
