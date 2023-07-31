#ifndef _POWER_ADC_H
#define _POWER_ADC_H

#include "gd32f4xx.h"
#include "systick.h"
void adc_gpio_config(void);
void adc_config(void);
extern volatile uint32_t adc0_value[6];

#endif 

