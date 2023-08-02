#ifndef _POWER_PARA_H
#define POWER_PARA_H

#include "gd32f4xx.h"

typedef struct {

  float32 x1;      //��һ�����ֵ
  float32 y1;      //��һ����ʵֵ
  float32 x2;      //�ڶ������ֵ
  float32 y2;      //�ڶ�����ʵֵ
  float32 Value;   //ʵʱֵ
  float32 Coeff;   //ADCֵ����ʵֵ�ı���ϵ��
  float32 Offset;  //���ƫ��ֵ
  
}ELEC_INFO_STRUCT;//���������ṹ��

#endif POWER_PARA_H