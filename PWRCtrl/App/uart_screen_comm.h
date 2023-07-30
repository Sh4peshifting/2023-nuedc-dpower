#ifndef UART_SCREEN_COMM_H
#define UART_SCREEN_COMM_H

#define switch_off 0
#define switch_on 1


enum {
    CURRENT = 0x00,
    VOLTAGE = 0x01,
    POWER = 0x02,
    POWER_FACTOR = 0x03,
    SWITCH = 0x04,
    CURRENT_CALIBRATION = 0x05,
    VOLTAGE_CALIBRATION = 0x06,
    POWER_CALIBRATION = 0x07,
    CALIBRATION = 0x08
};

typedef struct {
    unsigned char objname;
    float val;
} obj;


void send_two_decimal(char* objname, float val);
void send_three_decimal(char* objname, float val);
obj receive_msg_proc(char msg[]);
void beep(int ms);

#endif