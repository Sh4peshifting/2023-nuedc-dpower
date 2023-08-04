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

void task_hmi_rx(void);
void task_hmi_tx(void);
void task_hlw8032(void);
void startup(void);

uint8_t flag_hmi_tx=0;
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
    
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);
    
    usart0_gpio_config(115200U);
    power_info_init();
    eg2104_sd_init();
    relay_gpio_init();
    
    buck_boost_init();
//    zcd_init();
    
//    timer2_pwm_config();//spwm
//    timer8_int_init();//20K
    timer11_int_init();//50Hz
    
    for(uint8_t i=0;i<3;i++)
    {
        beep(100);
        delay_1ms(200);
    }
    
    relay_cmd(0,SET);
    relay_cmd(1,SET);
    
    while(1) 
    {
		if(uart3_complete_flag == 1)
        {   
           uart3_complete_flag = 0;
           task_hlw8032();
        } 
        if(uart0_complete_flag == 1)
        {
            uart0_complete_flag = 0;
            task_hmi_rx();
        }
        if(flag_hmi_tx == 1)
        {
            flag_hmi_tx=0;
            task_hmi_tx();
        }

    }
}

void task_hmi_rx()
{
    obj rec;
    rec=receive_msg_proc((char *)uart0_buff);
    if(rec.objname==SWITCH)
    {
        if(rec.val==1)
        {
            gpio_bit_set(GPIOE,GPIO_PIN_2);
            gpio_bit_set(GPIOE,GPIO_PIN_5);
            acv_openloop_en=1;
            //aci_loop_en=1;


            
        }
        else if(rec.val==0)
        {
            gpio_bit_reset(GPIOE,GPIO_PIN_2);
            gpio_bit_reset(GPIOE,GPIO_PIN_5);
            acv_openloop_en=0;
            aci_loop_en=0;
        }
    }
    else if(rec.objname==CURRENT)
    {
        i_acout=rec.val;
    }
}
void task_hmi_tx()
{
    send_two_decimal("voltage",v_grid);
    send_two_decimal("current",ip_inverter);
    send_two_decimal("V_IN1",v_in1.Value);
    send_two_decimal("V_IN2",v_in4.Value);
    send_two_decimal("I_IN1",i_in1.Value);
    send_two_decimal("I_IN2",i_in2.Value);
}
void task_hlw8032()
{
    HLW8032_Get_NoCalibration(uart3_buff,&ac1info);
    send_two_decimal("current",ac1info.I);
    send_two_decimal("voltage",ac1info.V);
    send_two_decimal("power",ac1info.AC_P);
    send_three_decimal("power_factor",ac1info.F);
}
void startup()
{
    if(v_grid>=5) 
    {
        
    }
    else
    {
        exti_interrupt_disable(EXTI_3);
    }
    
}
/*systick�ж���ÿ1ms����һ��*/
void task_manager_beat()
{
    static uint16_t cnt_hmi_tx=0;
    
    if(cnt_hmi_tx == 0) 
    {
        flag_hmi_tx=1;
        cnt_hmi_tx=100;
    }
    
    cnt_hmi_tx--;
}
