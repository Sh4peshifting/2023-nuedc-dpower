#ifndef _POWER_UART_H
#define _POWER_UART_H

#include "gd32f4xx.h"
#include "stdio.h"
extern uint8_t 	uart0_buff[20],uart3_buff[26];// ���ջ�����
extern uint8_t 	uart0_buff_len,uart3_buff_len;// �������ݳ���
extern uint8_t		uart0_complete_flag,uart3_complete_flag; // ����������ɱ�־λ
void usart0_gpio_config(uint32_t band_rate);
void usart_send_data(uint8_t ucch);
void usart_send_string(uint8_t *ucstr,uint8_t length);
void uart3_gpio_config(uint32_t band_rate);

#endif _POWER_UART_H
