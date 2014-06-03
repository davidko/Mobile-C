/* File: hello_world/server.c */

#include <stdio.h>
#include <libmc.h>

int main() 
{
  MCAgency_t agency;

  int local_port = 5051;
  char embedchhome[] = "/home/dko/sys/ch7.0.0";

  ChOptions_t* ch_options;
  MCAgencyOptions_t mc_options;

  setbuf(stdout, NULL);
  printf("Initializing options...\n");

  ch_options = (ChOptions_t*)malloc(sizeof(ChOptions_t));

  MC_InitializeAgencyOptions(&mc_options);

  ch_options->shelltype = CH_REGULARCH;
  ch_options->chhome = malloc(strlen(embedchhome)+1);
  strcpy(ch_options->chhome, embedchhome);

  mc_options.ch_options = ch_options;

  printf("Initializing agency...\n");

  //agency = MC_Initialize(local_port, &mc_options);
  agency = MC_Initialize(local_port, NULL);

  printf("Running mainloop... \n");
  MC_MainLoop(agency);

  MC_End(agency);
  return 0;
}
