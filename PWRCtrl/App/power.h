#ifndef _POWER_H
#define _POWER_H

#include "gd32f4xx.h"
#include "dp_para.h"
#include "foc_utils.h"
#include "dp_pid.h"
#include "power_adc.h"
#include "power_pwm.h"
#include "power_uart.h"

#define DP_PWM_PER   TIMER_CAR(TIMER1)
#define MAX_PWM_CMP   (uint16_t)(0.90F * TIMER_CAR(TIMER1)) //PWM最大比较值
#define MIN_PWM_CMP   (uint16_t)(0.10F * TIMER_CAR(TIMER1)) //PWM最小比较值

extern uint8_t buck_boost_en;

void power_ctrl_buck_boost(void);
void eg2104_sd_init(void);
void timer1_set_pwm(uint16_t pwm_cmp_value);
void buck_boost_init(void);
#endif _POWER_H
