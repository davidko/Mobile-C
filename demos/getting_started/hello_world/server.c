/* File: hello_world/server.c */

#include <stdio.h>
#include <libmc.h>

int main() 
{
  MCAgency_t agency;

  int local_port = 5051;
  setbuf(stdout, NULL);
  
  char embedchhome[] = "/home/dko/sys/ch7.0.0";
  ChOptions_t* ch_options;

  MCAgencyOptions_t mc_options;

  MC_InitializeAgencyOptions(&mc_options);

  ch_options = (ChOptions_t*)malloc(sizeof(ChOptions_t));

  ch_options->shelltype = CH_REGULARCH;
  ch_options->chhome = malloc(strlen(embedchhome)+1);
  strcpy(ch_options->chhome, embedchhome);

  mc_options.ch_options = ch_options;


  agency = MC_Initialize(local_port, &mc_options);

  MC_MainLoop(agency);

  MC_End(agency);
  return 0;
}
