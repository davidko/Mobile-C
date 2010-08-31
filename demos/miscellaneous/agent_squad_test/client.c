#include <stdio.h>
#include <libmc.h>

int main() {
  MCAgency_t agency;
  MCAgencyOptions_t options;
  int local_port = 5050;
  int server_port = 5051;
  char *server_name = "localhost";

  /* If server is not running on localhost, replace 'localhost' with the
   * fully qualified hostname of your server. i.e., if the server is running
   * on iel2.engr.ucdavis.edu, use */
  /* char *server_name = "iel2.engr.ucdavis.edu"; */

  MC_InitializeAgencyOptions(&options);
  MC_SetThreadOff(&options, MC_THREAD_CP); /* Turn off command prompt */

  agency = MC_Initialize(local_port, &options);
  printf("Press key to send the squad leader...\n");
  getchar();
  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
  MC_SendAgentFile( agency, "./squad_leader.xml");
  printf("Press key to send the Agent 1...\n");
  getchar();
  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
  MC_SendAgentFile( agency, "./agent1.xml");
  printf("Press key to send the Agent 2...\n");
  getchar();
  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
  MC_SendAgentFile( agency, "./agent2.xml");
  printf("Press key to send the Agent 3...\n");
  getchar();
  /* Note: The third argument of the following function may also be a
     valid IP address in the form of a string. i.e. 192.168.0.1 */
  MC_SendAgentFile( agency, "./agent3.xml");

  MC_End(agency);

  return 0;
}
