#include <stdio.h>
#include <libmc.h>

int main() {
  MCAgency_t agency;
  MCAgencyOptions_t options;

  char *file_name = "./test1.xml";

  /* If server is not running on localhost, replace 'localhost' with the
   * fully qualified hostname of your server. i.e., if the server is running
   * on iel2.engr.ucdavis.edu, use */
  /* char *server_name = "iel2.engr.ucdavis.edu"; */

  MC_InitializeAgencyOptions(&options);
  MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */
  agency = MC_Initialize(5050, &options);

  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
  MC_SendAgentFile(agency, file_name);

  printf("Terminating...\n");
  MC_End(agency);

  return 0;
}
