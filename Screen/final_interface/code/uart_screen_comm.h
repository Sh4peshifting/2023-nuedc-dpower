#ifndef UART_SCREEN_COMM_H
#define UART_SCREEN_COMM_H


enum {
    CURRENT = 0x00,
    RATE = 0x01,
};

typedef struct {
    unsigned char objname;
    float val;
} obj;


void send_msg_proc(char* objname, float val);
obj receive_msg_proc(char msg[]);
void beep(int ms);

#endif
