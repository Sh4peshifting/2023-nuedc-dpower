/*
 ***********************************************************************************
 *
 * Copyright (c) 2017-2020, �������������Ƽ����޹�˾
 * All rights reserved.                        
 *                                             
 ***********************************************************************************
 * @file       dp_pid.c
 *
 * @version    V1.1
 *
 * @date       2017-08-22
 *
 * @brief      ����ʽPID�㷨
 *
 * @attention  1.���������ѧϰʹ�ã��������������κ���;��
 *             2.��������������BUG�����ϿƼ�����Կ��ܷ��������⸺�κ����Ρ�
 *             3.���ǽ����ϵضԸô�����и������ƣ����и��²�������֪ͨ��
 *             4.��Ȩ���У�����ؾ���
 ***********************************************************************************
 */

#include "dp_pid.h"
#include "math.h"

/* �����������ֲ����������⹫�� */
extern void pid_reset( PID_STRUCT *p);
extern void pid_init ( PID_STRUCT *p);
extern void pid_clc  ( PID_STRUCT *p);
extern void pid_calc ( PID_STRUCT *p);

PID_FUNC_STRUCT pid_func = {

  .reset  = pid_reset,
  .init   = pid_init ,
  .clc    = pid_clc  ,
  .calc   = pid_calc 

};


/**
 **********************************************************
 * @������PID��������
 * @���룺*p�������PID����
 * @���أ��� 
 * @���ߣ��Ρ�СP
 * @�汾��V1.0
 **********************************************************
 */
void pid_init( PID_STRUCT *p)
{   
  if(p->T  <= 0)p->T  = 1.0f;
  if(p->Ti == 0)p->Ti = 3.4E+38;//Ti=0ʱ��Ϊ�������ֵ
  if(p->Ek_Dead < 0)p->Ek_Dead = 0;
  
  p->a0  = p->Kp*(1.0f + p->T/p->Ti + p->Td/p->T);
  p->a1  = p->Kp*(1.0f + p->Td/p->T * 2.0f);
  p->a2  = p->Kp*p->Td/p->T;

}
/**
 **********************************************************
 * @������PID���ݸ�λ
 * @���룺*p�������PID����
 * @���أ��� 
 * @���ߣ��Ρ�СP
 * @�汾��V1.0
 **********************************************************
 */
void pid_reset( PID_STRUCT  *p)
{
  p->a0      = 0;
  p->a1      = 0;
  p->a2      = 0;

  p->Ek_0    = 0;     //  Error[k-0]
  p->Ek_1    = 0;     //  Error[k-1]
  p->Ek_2    = 0;     //  Error[k-2]

  p->T       = 0;     //����
  p->Kp      = 0;     //����ϵ��P
  p->Ti      = 0;     //����ʱ��I
  p->Td      = 0;     //΢��ʱ��D
  
  p->Ek_Dead   = 0;  
  
  p->Increm    = 0;
  p->Output    = 0;
  
  p->OutMax    = 0;
  p->OutMin    = 0; 
}
/**
 **********************************************************
 * @���������PID�����еĻ���
 * @���룺*p�������PID����
 * @���أ��� 
 * @���ߣ��Ρ�СP
 * @�汾��V1.0
 **********************************************************
 */
void pid_clc( PID_STRUCT  *p)
{
  
  p->Ek_0       = 0;     //  Error[k]
  p->Ek_1       = 0;     //  Error[k-1]
  p->Ek_2       = 0;     //  Error[k-2]
  p->Increm     = 0;
  p->Output     = 0;
  
}
/**
 **********************************************************
 * @������PID����ʽ����
 * @���룺*p��PID�ṹ�� ref:�ο�ֵ fbd: ����ֵ
 * @���أ�PID������   
 * @���ߣ��Ρ�СP
 * @�汾��V1.1
 **********************************************************
 */
#pragma arm section code = "SRAMCODE"
void pid_calc( PID_STRUCT *p)
{
  
  p->Ek_0 = p->Ref - p->Fdb;  //�������

  //���С���趨������������PID���㣬������һ�����
  if( fabs(p->Ek_0) < p->Ek_Dead )
  {
    p->Increm = 0;
  }
  else
  {
    p->Increm = (   p->a0 * p->Ek_0  \
                  - p->a1 * p->Ek_1 \
                  + p->a2 * p->Ek_2 ); //PID��������  
  }

  p->Output += p->Increm; //�������

  p->Ek_2        = p->Ek_1; //����k-2���
  p->Ek_1        = p->Ek_0; //����k-1���
  
  if(p->Output > p->OutMax)
  {
    p->Output   =  p->OutMax; //����޷�
    return;
  }
  if(p->Output < p->OutMin)
  {
    p->Output   =  p->OutMin; //��С�޷�
    return;
  }

}
#pragma arm section


/*       [] END OF FILE          */



