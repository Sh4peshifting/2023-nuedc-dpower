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
#define MAX_PWM_CMP   (uint16_t)(0.93F * TIMER_CAR(TIMER1)) //PWM最大比较值
#define MIN_PWM_CMP   (uint16_t)(0.05F * TIMER_CAR(TIMER1)) //PWM最小比较值

/*para by hizrd*/
#define CURRENT_RATIO   (50.00f/4095)  //输出电流比例 3.3/0.005/20 = 33.00
#define DC_VOLTAGE_RETIO    (66.f/4095)  //输入电压比例
#define AC_VOLTAGE_RETIO    (-66.f/4095)  //输入电压比例
#define ADC_I_OFFSET        2048L         //电流采样的1.65V偏移电压对应的ADC采样值
#define ADC_ACV_OFFSET        2048L         //电流采样的1.65V偏移电压对应的ADC采样值

extern uint8_t buck_boost_en;
extern AC_Para ac1info;

void dp_ElecLineCorr(ELEC_INFO_STRUCT *p);
void power_ctrl_buck_boost(void);
void eg2104_sd_init(void);
void timer1_set_pwm(uint16_t pwm_cmp_value);
void buck_boost_init(void);
void zcd_init(void);
void zcd_pfc_handler(void);
void pfc_init(void);
void adc_value_process(void);
void power_info_init(void);
#endif 
