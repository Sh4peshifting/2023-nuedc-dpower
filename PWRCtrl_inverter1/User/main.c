/********************************************************************************
  * 文 件 名: main.c
  * 版 本 号: 初版
  * 修改作者: LC
  * 修改日期: 2022年04月12日
  * 功能介绍:          
  ******************************************************************************
  * 注意事项:
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

uint8_t flag_hmi_tx=0;
/************************************************
函数名称 ： main
功    能 ： 主函数
参    数 ： 无
返 回 值 ： 无
作    者 ： LC
*************************************************/
int main(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    systick_config();   // 滴答定时器初始化
	led_gpio_config();  // led初始化

    // gpio_bit_set(GPIOD,GPIO_PIN_7);             // PD7输出高电平
    // gpio_bit_reset(GPIOD,GPIO_PIN_7);						// PD7输出低电平
    // gpio_bit_write(PORT_LED2,PIN_LED2,RESET);		// LED2输出低电平
    gpio_bit_write(PORT_LED2,PIN_LED2,SET);  			// LED2输出高电平
    
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);
    
    usart0_gpio_config(115200U);
    uart3_gpio_config(4800U);
    power_info_init();
    eg2104_sd_init();
    
    buck_boost_init();
    //timer1_set_pwm(700);
    //pfc_init();
    timer2_pwm_config();//spwm
    timer8_int_init();//10K
    zcd_init();
    timer11_int_init();//200Hz
    
    for(uint8_t i=0;i<3;i++)
    {
        beep(100);
        delay_1ms(200);
    }
    

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
//        gpio_bit_toggle(GPIOE,GPIO_PIN_2);
//        gpio_bit_toggle(GPIOE,GPIO_PIN_4);
//        gpio_bit_toggle(GPIOE,GPIO_PIN_5);
//        gpio_bit_toggle(GPIOE,GPIO_PIN_6);
        delay_1ms(100);
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
            //buck_boost_en=1;
            pfc_en=1;
        }
        else if(rec.val==0)
        {
            gpio_bit_reset(GPIOE,GPIO_PIN_2);
            buck_boost_en=0;
            pfc_en=0;
        }
    }
}
void task_hmi_tx()
{
    send_two_decimal("voltage",v_in3.Value);
    send_two_decimal("current",v_in4.Value);
    send_two_decimal("V_IN1",v_in1.Value);
    send_two_decimal("V_IN2",v_in2.Value);
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

/*systick中断里每1ms调用一次*/
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
