/********************************************************************************
  * �� �� ��: main.c
  * �� �� ��: ����
  * �޸�����: LC
  * �޸�����: 2022��04��12��
  * ���ܽ���:          
  ******************************************************************************
  * ע������:
*********************************************************************************/

#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "bsp_led.h"

#include "power.h"
#include "uart_screen_comm.h"
#include "HLW8032.h"

/************************************************
�������� �� main
��    �� �� ������
��    �� �� ��
�� �� ֵ �� ��
��    �� �� LC
*************************************************/
int main(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    systick_config();   // �δ�ʱ����ʼ��
	led_gpio_config();  // led��ʼ��

    // gpio_bit_set(GPIOD,GPIO_PIN_7);             // PD7����ߵ�ƽ
    // gpio_bit_reset(GPIOD,GPIO_PIN_7);						// PD7����͵�ƽ
    // gpio_bit_write(PORT_LED2,PIN_LED2,RESET);		// LED2����͵�ƽ
    gpio_bit_write(PORT_LED2,PIN_LED2,SET);  			// LED2����ߵ�ƽ
    //buck_boost_init();
    
    usart0_gpio_config(115200U);
    uart3_gpio_config(4800U);

 
    
    AC_Para ac1info;
    eg2104_sd_init(); 
    
    timer1_set_pwm(600);
    
    pwm_config2();
    timer_channel_output_pulse_value_config(TIMER7,TIMER_CH_0,6000);
    
    for(uint8_t i=0;i<3;i++)
    {
        beep(100);
        delay_1ms(200);
    }
    
    
    obj rec;
    while(1) 
    {
		if(uart3_complete_flag == 1)
        {
           
           uart3_complete_flag = 0;
           HLW8032_Get_NoCalibration(uart3_buff,&ac1info);
           send_two_decimal("current",ac1info.I);
           send_two_decimal("voltage",ac1info.V);
           send_two_decimal("power",ac1info.AC_P);
           send_three_decimal("power_factor",ac1info.F);
        } 
        if(uart0_complete_flag == 1)
        {
            uart0_complete_flag = 0;
            rec=receive_msg_proc((char *)uart0_buff);
            if(rec.objname==SWITCH)
            {
                if(rec.val==1)
                {
                    gpio_bit_set(GPIOE,GPIO_PIN_2);
                    //buck_boost_en=1;
                }
                else if(rec.val==0)
                {
                    gpio_bit_reset(GPIOE,GPIO_PIN_2);
                    buck_boost_en=0;
                }
            }
            
        }
//        gpio_bit_toggle(GPIOE,GPIO_PIN_2);
//        gpio_bit_toggle(GPIOE,GPIO_PIN_4);
//        gpio_bit_toggle(GPIOE,GPIO_PIN_5);
//        gpio_bit_toggle(GPIOE,GPIO_PIN_6);
        send_two_decimal("voltage",(float)adc0_value[0]*3.3f/4096.f*20);
        delay_1ms(100);
    }
}
