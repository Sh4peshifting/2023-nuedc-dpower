/**
  ******************************************************************************
  * @author  �Ρ�СP
  * @version V1.0
  * @date    2017-08-22
  * @brief   �������������Ƽ����޹�˾ ��Ȩ���У�C��All rights reserved.
  ******************************************************************************
  * 
  *  �ļ�����dp_pid.h
  *  ��  �ܣ�PID�㷨ͷ�ļ�
  *
  ******************************************************************************
**/

#ifndef __DP_PID_H
#define __DP_PID_H

typedef struct  _PID{ 
  
  float  T;       //����:����
  float  Kp;      //����:����
  float  Ti;      //����:����
  float  Td;      //����:΢��

  float  a0;      //����ֵ:a0 = Kp(1.0 + T/Ti + Td/T)
  float  a1;      //����ֵ:a1 = Kp(1.0 + 2*Td/T)
  float  a2;      //����ֵ:a2 = Kp*Td/T

  float Ref;     //��׼ֵ
  float Fdb;     //����ֵ
  
  float Ek_Dead; //�������
  
  float  Ek_0;    //����ֵ:Error[k-0],�������
  float  Ek_1;    //����ֵ:Error[k-1],ǰһ�����
  float  Ek_2;    //����ֵ:Error[k-2],ǰ�������

  float  Increm;   //����ֵ:����ֵ
  float  Output;  //����ֵ:PID���ֵ

  float  OutMax;   //����:PID������ֵ
  float  OutMin;   //����:PID�����Сֵ
  
}PID_STRUCT;


typedef struct _PID_FUNC{

  void (*reset) ( PID_STRUCT *p);//��λPID����PID���еĲ���������
  void (*init ) ( PID_STRUCT *p);//��ʼ��PID������a0,a1,a2��ϵ��
  void (*clc  ) ( PID_STRUCT *p);//���PID�������ݣ���ֹͣ����PIDʱ�õ�
  void (*calc ) ( PID_STRUCT *p);//PID����

}PID_FUNC_STRUCT;


extern PID_FUNC_STRUCT pid_func;

#endif


