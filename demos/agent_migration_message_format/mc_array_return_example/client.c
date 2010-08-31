/* File: mc_array_return_example/client.c */

#include <libmc.h>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#endif
int main()
{
    MCAgency_t agency;
    MCAgent_t agent;
    int* data;
    int total_tasks, task_num, size, num_elements, j;
    //char *xml;
    MCAgencyOptions_t options;
    int local_port = 5051;
    ChType_t type;

    MC_InitializeAgencyOptions(&options);
    MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */
    agency = MC_Initialize(
            local_port,
            &options);

    agent = MC_ComposeAgentFromFile(
            "mobagent1",        /* Name */
            "localhost:5051",   /* Home - This is the host the agent will return to
                                 * when it has finished its tasks. */
            "IEL",              /* Owner */
            "agent.c",          /* Code filename */
            "a",     /* Return Variable Name */
            "localhost:5050",   /* server/destination host */
            0                   /* persistent */
            );


    /* Add the agent */
    MC_AddAgent(agency, agent);

    /* Wait for return-agent arrival signal */
    MC_WaitSignal(agency, MC_RECV_RETURN);

    /* Make sure we caught the correct agent */
    agent = MC_FindAgentByName(agency, "mobagent1");
    if (agent == NULL) {
        fprintf(stderr, "Did not receive correct agent. \n");
        exit(1);
    }

    total_tasks = MC_GetAgentNumTasks(agent);
    printf("%d tasks.\n", total_tasks);
    for (task_num = 0; task_num < total_tasks; task_num++) {
        type = MC_AgentReturnDataType(agent, task_num);
        if (type != CH_INTTYPE) {
            printf("Error, expected 'int' type.");
            continue;
        }
        size = MC_AgentReturnDataSize(agent, task_num);
        if (size < 0) {
            printf("Error retrieving return variable size.\n");
            continue;
        }
        num_elements = MC_AgentReturnArrayNum(agent, task_num);
        if (num_elements < 0) {
            printf("Error retrieving number of elements in agent data.\n");
            continue;
        }

        printf("Received a %d dimensional array.\n", MC_AgentReturnArrayDim(agent, task_num));
        printf("The array extents are: ");
        for(j = 0; j < MC_AgentReturnArrayDim(agent, task_num); j++) {
            printf("%d, ", MC_AgentReturnArrayExtent(agent, task_num, j));
        }
        printf("\n");

        data = (int*)malloc(size * num_elements);
        memcpy ( data, MC_AgentReturnDataGetSymbolAddr(agent, task_num), size * num_elements);
        printf("The array elements are: \n");
        for(j = 0; j < num_elements; j++) {
            printf("%d ", data[j]);
        }
        printf("\n");

        free (data);
    }

    /* We must reset the signal that we previously caught with the 
     * MC_WaitSignal() function with MC_ResetSignal() */
    MC_ResetSignal(agency);

    MC_End(agency);

    return 0;
}

