#include <stdio.h>
#include <libmc.h>

int main() {
  MCAgency_t agency;
  MCAgencyOptions_t options;
  int local_port = 5050;

  /* If server is not running on localhost, replace 'localhost' with the
   * fully qualified hostname of your server. i.e., if the server is running
   * on iel2.engr.ucdavis.edu, use */
  /* char *server_name = "iel2.engr.ucdavis.edu"; */

  MC_InitializeAgencyOptions(&options);
  MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */

  agency = MC_Initialize(local_port, &options);
  printf("Sending the squad leader...\n");
  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
  MC_SendAgentFile( agency, "./squad_leader.xml");
  printf("Sending Agent 1...\n");
  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
  MC_SendAgentFile( agency, "./agent1.xml");
  printf("Sending Agent 2...\n");
  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
  MC_SendAgentFile( agency, "./agent2.xml");
  printf("Sending Agent 3...\n");
  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
  MC_SendAgentFile( agency, "./agent3.xml");

  MC_End(agency);

  return 0;
}
