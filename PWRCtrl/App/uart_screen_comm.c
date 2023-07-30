#include "uart_screen_comm.h"
#include <stdio.h>
#include <string.h>

void send_two_decimal(char* objname, float val) {
    printf("%s.val=%d\xff\xff\xff", objname, (int)(val * 100));
}

void send_three_decimal(char* objname, float val) {
    printf("%s.val=%d\xff\xff\xff", objname, (int)(val * 1000));
}

obj receive_msg_proc(char msg[]) {
    obj objlist;
    if (msg[1] == SWITCH) {
        objlist.objname = SWITCH;
        objlist.val = msg[2];
        return objlist;
    }
    else if (msg[1] == CALIBRATION)
    {
        objlist.objname = CALIBRATION;
        objlist.val = 0xff;
        return objlist;
    }
    else
    {
        unsigned int val = (msg[5] << 24) | (msg[4] << 16) | (msg[3] << 8) | msg[2];
        switch (msg[1]) {
            case CURRENT:
                objlist.objname = CURRENT;
                objlist.val = (float)val / 100.0f;
                break;
            case VOLTAGE:
                objlist.objname = VOLTAGE;
                objlist.val = (float)val / 100.0f;
                break;
            case POWER:
                objlist.objname = POWER;
                objlist.val = (float)val / 100.0f;
                break;
            case POWER_FACTOR:
                objlist.objname = POWER_FACTOR;
                objlist.val = (float)val / 1000.0f;
                break;
            case CURRENT_CALIBRATION:
                objlist.objname = CURRENT_CALIBRATION;
                objlist.val = (float)val / 1000.0f;
                break;
            case VOLTAGE_CALIBRATION:
                objlist.objname = VOLTAGE_CALIBRATION;
                objlist.val = (float)val / 1000.0f;
                break;
            case POWER_CALIBRATION:
                objlist.objname = POWER_CALIBRATION;
                objlist.val = (float)val / 1000.0f;
                break;
            default:
                objlist.objname = 0xff;
                objlist.val = 0.0f;
                break;
        }
    }
    return objlist;
}

void beep(int ms) {
    printf("beep %d\xff\xff\xff", ms);
}