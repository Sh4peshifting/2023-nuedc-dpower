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
#define MAX_PWM_CMP   (uint16_t)(0.90F * TIMER_CAR(TIMER1)) //PWM���Ƚ�ֵ
#define MIN_PWM_CMP   (uint16_t)(0.08F * TIMER_CAR(TIMER1)) //PWM��С�Ƚ�ֵ

/*para by hizrd*/
#define CURRENT_RATIO   (25.00f/4095)  //����������� 3.3/0.005/20 = 33.00
#define DC_VOLTAGE_RETIO    (66.f/4095)  //�����ѹ����
#define AC_VOLTAGE_RETIO    (-66.f/4095)  //�����ѹ����
#define ADC_I_OFFSET        2048L         //����������1.65Vƫ�Ƶ�ѹ��Ӧ��ADC����ֵ
#define ADC_ACV_OFFSET        2048L         //����������1.65Vƫ�Ƶ�ѹ��Ӧ��ADC����ֵ

extern uint8_t buck_boost_en,pfc_en,acv_loop_en,aci_loop_en;
extern float v_buck,vp_inverter,ip_inverter;
extern AC_Para ac1info;
extern ELEC_INFO_STRUCT v_in1 ,v_in2,v_in3,v_in4,i_in1,i_in2,i_in3;
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
void power_ctrl_ACcurrent(void);
void power_ctrl_ACvoltage(void);

#endif 
