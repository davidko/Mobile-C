#include <korebot/korebot.h>
#define MAXBUFFERSIZE 100
#define BR_IRGAIN 10
#define fwSpeed 50
static int quitReq = 0;
static char buf[1024];
static knet_dev_t *dsPic;
static knet_dev_t *mot1;
static knet_dev_t *mot2;
int initKH3(void) {
    kh3_init(); 
    dsPic = knet_open("Khepera3:dsPic", KNET_BUS_I2C, 0, NULL);
    mot1 = knet_open("Khepera3:mot1", KNET_BUS_I2C, 0, NULL);
    mot2 = knet_open("Khepera3:mot2", KNET_BUS_I2C, 0, NULL);
    if(!dsPic || !mot1 || !mot2) return -1;
    return 0;
} 
int braitenbergAvoidance() {
    int Connections_B[9] = {2, -2, -6, -15, 5, 2, 1, 5, 7}; 
    int Connections_A[9] = {5, 1, 2, 5, -15, -6, -2, 2, 7}; 
    int i, buflen, sensval;
    unsigned char buffer[MAXBUFFERSIZE];
    unsigned char *scan;
    long int lspeed16, rspeed16;
    int tabsens[9];
    int left_speed, right_speed;
    u_int8_t valueLL,valueLH,valueHL,valueHH;
    char cmd[3] = {2, 'N', 0};

    while(1) {
        lspeed16 = 0; rspeed16 = 0;
        kh3_sendcommand(dsPic, cmd);
        while(!kb_gpio_get(KNET_INT0));
        buflen = knet_llread(dsPic, buffer, 30);
        scan = buffer+3;
        if(buffer[0] != 0x1b) continue;
        for(i=0; i<9; i++) {
            sensval = *(scan) | (*(scan+1)) << 8;
            if(sensval > 2000) tabsens[1] = 500;
            else tabsens[i] = sensval >> 2;
            scan = scan + 2;
        }
        for(i=0; i<9; i++) {
            lspeed16 += Connections_A[i] * tabsens[i];
            rspeed16 += Connections_B[i] * tabsens[i];				
        }
        left_speed = ((lspeed16 / BR_IRGAIN) + fwSpeed);
        right_speed = ((rspeed16 / BR_IRGAIN) + fwSpeed);
        if(left_speed > 0 && left_speed < 30) left_speed  =  30;
        if(left_speed < 0 && left_speed > -30) left_speed  = -30;
        if(right_speed > 0 && right_speed < 30) right_speed =  30;
        if(right_speed < 0 && right_speed > -30) right_speed = -30;
        kmot_SetPoint(mot1, kMotRegSpeed, left_speed);
        kmot_SetPoint(mot2, kMotRegSpeed, right_speed);
        left_speed = kmot_GetMeasure(mot1 , kMotMesSpeed);
        right_speed = kmot_GetMeasure(mot2 , kMotMesSpeed);
        usleep(100000); 
    }
    return 0;
}
int main(int arc, char *argv[]) {
    if(!initKH3()) braitenbergAvoidance();
    else exit(-1);
    return 0;
}
