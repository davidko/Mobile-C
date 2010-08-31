#include <stdio.h>
#include <math.h>
int main()
{
    char* str;
    MCAgent_t agent;
    printf("Hello World!\n");
    printf("This is mobagent1 from the agency at local_port 5050.\n");
    printf("I will now generate a new agent to send back to\n");
    printf("my home host.\n");
    agent = mc_ComposeAgent(
        "NewAgent",
        "localhost:5051",
        "mobagent1",
        "\n#include <stdio.h>\n \
        int main()\n \
        {\n \
          printf(\"Hello World!!\\n\");\n \
          }\n",
        "no-return",
        "localhost:5050",
        0 );
          
    mc_AddAgent(agent);

    return 0;
}
