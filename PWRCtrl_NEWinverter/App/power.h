#ifndef _POWER_H
#define _POWER_H

#include "gd32f4xx.h"
#include "dp_para.h"
#include "foc_utils.h"
#include "dp_pid.h"
#include "power_adc.h"
#include "power_pwm.h"
#include "power_uart.h"
#include "power_timer.h"
#include "dp_filter.h"
#include "HLW8032.h"

#define DP_PWM_PER   TIMER_CAR(TIMER1)
#define MAX_PWM_CMP   (uint16_t)(0.90F * TIMER_CAR(TIMER1)) //PWM最大比较值
#define MIN_PWM_CMP   (uint16_t)(0.08F * TIMER_CAR(TIMER1)) //PWM最小比较值

/*para by hizrd*/
#define CURRENT_RATIO   (25.00f/4095)  //输出电流比例 3.3/0.005/20 = 33.00
#define DC_VOLTAGE_RETIO    (66.f/4095)  //输入电压比例
#define AC_VOLTAGE_RETIO    (-66.f/4095)  //输入电压比例
#define ADC_I_OFFSET        2048L         //电流采样的1.65V偏移电压对应的ADC采样值
#define ADC_ACV_OFFSET        2048L         //电流采样的1.65V偏移电压对应的ADC采样值

#define SIN_TIME _sin(((float)cnt_spwm/400*_2PI))
extern uint8_t aci_loop_en,acv_loop_en,acv_openloop_en;
extern float vp_inverter,ip_inverter,i_acout,v_inverter_out,v_gird;
extern AC_Para ac1info;
extern ELEC_INFO_STRUCT v_in1 ,v_in2,v_in3,v_in4,i_in1,i_in2,i_in3;
void dp_ElecLineCorr(ELEC_INFO_STRUCT *p);
void eg2104_sd_init(void);
void buck_boost_init(void);
void zcd_init(void);
void adc_value_process(void);
void power_info_init(void);
void relay_gpio_init(void);
void relay_cmd(uint8_t ch,bit_status bit);
void set_spwm(float pwm);

#endif 
