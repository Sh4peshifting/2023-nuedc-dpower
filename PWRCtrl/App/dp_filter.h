/**
  ******************************************************************************
  * @author  �Ρ�СP
  * @version V1.0
  * @date    2018-08-22
  * @brief   �������������Ƽ����޹�˾ ��Ȩ���У�C��All rights reserved.
  ******************************************************************************
  * 
  *  �ļ�����dp_filter.h
  *  ��  �ܣ������˲��㷨
  *
  ******************************************************************************
**/ 


#ifndef _DP_FILTER_H
#define _DP_FILTER_H

#include "system_type.h"



/*  ����Բ���ʦ�  */
#ifndef PI
#define PI    3.14159265F // ��
#define PI2   6.28318530F // 2��
#endif

/*  ��ͨ�˲�������  */
#define low_filter_default {0, 0, 0, 0, 0, 0}
typedef struct
{
  float32  Input;
  float32  Output;
  float32  Fc;
  float32  Fs;
  float32  Ka;
  float32  Kb;
  
}LOW_FILTER_STRUCT;

/*  �޷��˲�������  */
typedef struct{

  float32  Slope; //�仯��
  float32  Fs;    //����Ƶ��
  float32  Input; //����
  float32  Output;//���
  float32  LastInput;//��һ������
  float32  LimValue;//�޷�ֵ = �仯�� * ��������

}LIM_FILTER_STRUCT;

/*  ƽ��ֵ�˲�������  */
#define AverageFilterBuf_MaxLen 16u

typedef struct{

  float32 Base[AverageFilterBuf_MaxLen];
  float32 Sum;
  float32 Mean;
  uint8   FullFlag;
  uint16  Head;
  uint16  Rear;

}MEAN_FILTER_STRUCT;

/*  ȥ��ë���˲���������ݽṹ  */
typedef struct {          
  float32 Input;    //���β�������
  float32 Output;   //��ǰʹ������
  float32 Err;      //ƫ����ֵ
  float32 Max;      //���ƫ��ֵ
  float32 Min;      //��Сƫ��ֵ
}BURR_FILTER_STRUCT;

/*  �����˲��������ݽṹ  */
typedef struct {          
  float32 Input;    //���β�������
  float32 Output;   //��ǰʹ������
  uint16  N;        //����ֵ
  uint16  Count;    //������
  uint16  Status;   //״̬
}SHAK_FILTER_STRUCT;

/*  TD΢�ָ����������ݽṹ  */
typedef struct {
  
  float v;
  float v1;
  float v2;
  float r;
  float h;
  
}TD_FILTER_STRUCT;

void low_filter_init(LOW_FILTER_STRUCT *p);
void low_filter_calc(LOW_FILTER_STRUCT *p);

void average_filter_init( MEAN_FILTER_STRUCT* p);
void average_filter_clac( MEAN_FILTER_STRUCT* p, float32 NewValue);

void burr_filter_init(BURR_FILTER_STRUCT * p);
void burr_filter_calc(BURR_FILTER_STRUCT * p);

void shak_filter_init(SHAK_FILTER_STRUCT * p);
void shak_filter_calc(SHAK_FILTER_STRUCT * p);

void td_filter_cala(TD_FILTER_STRUCT *p);

#endif










