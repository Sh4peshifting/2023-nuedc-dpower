#ifndef _POWER_PARA_H
#define POWER_PARA_H

#include "gd32f4xx.h"

typedef struct {

  float32 x1;      //第一点采样值
  float32 y1;      //第一点真实值
  float32 x2;      //第二点采样值
  float32 y2;      //第二点真实值
  float32 Value;   //实时值
  float32 Coeff;   //ADC值与真实值的比例系数
  float32 Offset;  //零点偏移值
  
}ELEC_INFO_STRUCT;//电气参数结构体

#endif POWER_PARA_H