/* mc_sample_app.c
 *
 * This sample program uses the Mobile C library to build
 * a simple command-line driven client/server app. 
 * 
 * 12/15/2006
 * */
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
    /* Init the agency */
    MCAgency_t agency;
    int local_port=5050;
    agency = MC_Initialize(
            local_port,
            NULL);

    MC_MainLoop(agency);
        return 0;
}
