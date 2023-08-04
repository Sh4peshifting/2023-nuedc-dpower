#include "uart_screen_comm.h"
#include <stdio.h>
#include <string.h>

void send_msg_proc(char *objname, float val)
{
    printf("%s.val=%d\xff\xff\xff", objname, (int)(val * 100));
}

obj receive_msg_proc(char msg[])
{
    obj objlist;
    unsigned int val = (msg[5] << 24) | (msg[4] << 16) | (msg[3] << 8) | msg[2];
    switch (msg[1])
    {
    case CURRENT:
        objlist.objname = CURRENT;
        objlist.val = (float)val / 100.0f;
        break;
    case RATE:
        objlist.objname = RATE;
        objlist.val = (float)val / 10.0f;
        break;
    default:
        objlist.objname = 0xff;
        objlist.val = 0.0f;
        break;
    }
    return objlist;
}

void beep(int ms)
{
    printf("beep %d\xff\xff\xff", ms);
}
