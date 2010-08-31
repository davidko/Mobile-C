#include <stdio.h>
#include <stdlib.h>
#include <libmc.h>

int main() 
{
  MCAgency_t agency;
  MCAgencyOptions_t options;

  char *str = "<?xml version=\"1.0\"?>\n\n<!DOCTYPE myMessage SYSTEM \"mobilec.dtd\">\n\n<MOBILEC_MESSAGE>\n <MESSAGE message=\"MOBILE_AGENT\">\n  <MOBILE_AGENT>\n   <AGENT_DATA>\n    <NAME>mobagent1</NAME>\n    <OWNER>IEL</OWNER>\n    <HOME>localhost:5050</HOME>\n    <TASKS task=\"1\" num=\"0\">\n     <TASK num=\"0\" name=\"no-return\" complete=\"0\" server=\"localhost:5051\" />\n     <AGENT_CODE>\n      <![CDATA[\n//#include <stdio.h>\n#include <math.h>\nint main()\n{\n    char* str;\n    printf(\"Hello World!\\n\");\n    printf(\"This is mobagent1 from the agency at local_port 5050.\\n\");\n    printf(\"I am performing the task on the agency at local_port 5051 now.\\n\");\n    printf(\"%f\\n\", hypot(1,2)); \n    str = strdup(\"Hello! This is a test.\");\n    free(str);\n\n    return 0;\n}\n      ]]>\n     </AGENT_CODE>\n    </TASKS>\n   </AGENT_DATA>\n  </MOBILE_AGENT>\n </MESSAGE>\n</MOBILEC_MESSAGE>\n";

  MC_InitializeAgencyOptions(&options);
  MC_SetThreadOff(&options, MC_THREAD_CP);
  agency = MC_Initialize(5050, &options);

  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
  MC_SendAgent(agency,
      str);
  MC_End(agency);
  exit(0);
}
