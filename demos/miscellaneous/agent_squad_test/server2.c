#include <libmc.h>

int main() {
    MCAgency_t agency;
    int local_port;

    local_port = 5052;

    agency = MC_Initialize(local_port, NULL);

    if(MC_MainLoop(agency) != 0) {
        MC_End(agency);
        return -1;
    }

    return 0;
}
