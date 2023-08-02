/*
 ***********************************************************************************
 *
 * @file       dp_filter.c
 * @brief      �����˲��㷨
 * @author     hepeng(1130001324@qq.com)
 * @attention  1.���������ѧϰʹ�ã��������������κ���;��
 *             2.��������������BUG�����ϿƼ�����Կ��ܷ��������⸺�κ����Ρ�
 *             3.���ǽ����ϵضԸô�����и������ƣ����и��²�������֪ͨ��
 *             4.��Ȩ���У�����ؾ���
 *
 * Copyright (c) 2018-2020, �������������Ƽ����޹�˾ All rights reserved. 
 * 
 * History:
 ***********************************************************************************
 * 2018-08-22 hepeng ����v1.0
 * 2018-12-22 hepeng ����v1.1�����ӵ�ͨ�˲������޷��˲���
 * 2019-01-15 hepeng ����v1.2�����������˲�����ȥë���˲���
 */


#include "dp_filter.h"
#include "string.h"
#include "math.h"

/**
 * @����������ƽ��ֵ�˲���ʼ��
 * @���룺NewValue:�µĲ�������ֵ
 * @���أ���
 * @���ߣ��Ρ�СP
 * @�汾��V1.0,2018-08-22
 ******************************************************************************
 */
void average_filter_init( MEAN_FILTER_STRUCT* p)
{

  memset(&p->Base,0x00,AverageFilterBuf_MaxLen);
  p->FullFlag = 0;
  p->Head     = 0;
  p->Mean     = 0;
  p->Rear     = 0;
  p->Sum      = 0;
  
}
/**
 ******************************************************************************
 * @����������ƽ��ֵ�˲�
 * @���룺NewValue:�µĲ�������ֵ
 * @���أ���
 * @���ߣ��Ρ�СP
 * @�汾��V1.0,2018-08-22
 ******************************************************************************
 */
void average_filter_clac( MEAN_FILTER_STRUCT* p, float32 NewValue)
{
  
  if(p->FullFlag != 0)
  {
    p->Rear  = (p->Rear + 1)%AverageFilterBuf_MaxLen;
    p->Sum  -= p->Base[p->Rear];
    p->Sum  += NewValue;
    
    p->Base[p->Rear] = NewValue;
    
    if(p->Head == p->Rear)p->Head = (p->Head + 1)%AverageFilterBuf_MaxLen;
    
    p->Mean  = p->Sum / AverageFilterBuf_MaxLen;
  }
  else
  {

    p->Base[p->Rear]  = NewValue;
    
    p->Sum  += NewValue;    
    p->Mean  = p->Sum / (p->Rear + 1);

    if(p->Rear >= AverageFilterBuf_MaxLen - 2)
      p->FullFlag = 0x01;
    
    p->Rear += 1;
  }
}

/**
 ******************************************************************************
 * @��������ͨ�˲�����ʼ��
 * @���룺LOW_FILTER_STRUCT�ṹ��
 * @���أ���
 * @���ߣ��Ρ�СP
 * @�汾��V1.0,2018-12-22
 ******************************************************************************
 */
void low_filter_init(LOW_FILTER_STRUCT *p)
{
  float32 Tc;//ʱ�䳣����
  
  if( p->Fc <= 0.0f || p->Fs <= 0.0f )
  {
      p->Ka      = 1.0f;
      p->Kb      = 0.0f;
      return;
  }

  Tc         = 1.0f / (PI2 * p->Fc);

  p->Ka      = 1.0f / (1.0f + Tc * p->Fs);
  p->Kb      = 1.0f - p->Ka;
  p->Input   = 0.0f;
  p->Output  = 0.0f;
}
/**
 ******************************************************************************
 * @��������ͨ�˲���
 * @���룺LOW_FILTER_STRUCT�ṹ��
 * @���أ���
 * @���ߣ��Ρ�СP
 * @�汾��V1.0,2018-12-22
 ******************************************************************************
 */
void low_filter_calc(LOW_FILTER_STRUCT *p)
{
  if(p->Output == p->Input)return;

  p->Output = p->Ka * p->Input + p->Kb * p->Output;

}

/**
 ******************************************************************************
 * @�������޷��˲�����ʼ��
 * @���룺LIM_FILTER_STRUCT�ṹ��
 * @���أ���
 * @���ߣ��Ρ�СP
 * @�汾��V1.0,2018-12-22
 ******************************************************************************
 */
void lim_filter_init(LIM_FILTER_STRUCT *p)
{
  if(p->Fs <= 0)p->Fs = 1;
  
  p->LimValue  = p->Slope / p->Fs;
  p->Input     = 0;
  p->LastInput = 0;
  p->Output    = 0;
}
/**
 ******************************************************************************
 * @�������޷��˲���
 * @���룺LIM_FILTER_STRUCT�ṹ��
 * @���أ���
 * @���ߣ��Ρ�СP
 * @�汾��V1.0,2018-12-22
 ******************************************************************************
 */
void lim_filter_calc(LIM_FILTER_STRUCT *p)
{
  
  if( fabs(p->Input - p->LastInput) < p->LimValue)
  {
     p->Output = p->Input;
  }
  p->LastInput = p->Input;

}

/**
 ******************************************************************************
 * @������ȥë���˲���
 * @���룺BURR_FILTER_STRUCT�ṹ��
 * @���أ���
 * @���ߣ��Ρ�СP
 * @�汾��V1.0,2019-1-15
 ******************************************************************************
 */
void burr_filter_init(BURR_FILTER_STRUCT * p)
{
  p->Input  = 0;
  p->Output = 0;
  p->Err    = 0;
}
/**
 ******************************************************************************
 * @������ȥë���˲���
 * @���룺BURR_FILTER_STRUCT�ṹ��
 * @���أ���
 * @���ߣ��Ρ�СP
 * @�汾��V1.0,2019-1-15
 ******************************************************************************
 */
void burr_filter_calc(BURR_FILTER_STRUCT * p)
{
  float32 m_delta;

  m_delta = fabs( p->Input - p->Output );
  
  if(m_delta > p->Err)
  {
    p->Err = p->Err*2.0F;
  }
  else
  {
    p->Output = p->Input;
    if(m_delta < p->Err*0.5f)
    {
      p->Err = p->Err*0.5f;
    }
  }
  p->Err = (p->Err > p->Max) ? p->Max : p->Err;
  p->Err = (p->Err < p->Min) ? p->Min : p->Err;
}
/**
  ******************************************************************************
 * @�����������˲�����ʼ��
 * @���룺BURR_FILTER_STRUCT�ṹ��
 * @���أ���
 * @���ߣ��Ρ�СP
 * @�汾��V1.0,2019-1-15
  ******************************************************************************
 */
void shak_filter_init(SHAK_FILTER_STRUCT * p)
{
  p->Input  = 0;
  p->Output = 0;
  p->Status = 0;
  p->Count  = 0;
}
/**
 ******************************************************************************
 * @�����������˲���
 * @���룺shak_FILTER_STRUCT�ṹ��
 * @���أ���
 * @���ߣ��Ρ�СP
 * @�汾��V1.0,2019-1-15
 ******************************************************************************
 */
void shak_filter_calc(SHAK_FILTER_STRUCT * p)
{

  if( p->Input > p->Output )
  {
    if(p->Status != 1)
    {
      p->Count  = 0;
      p->Status = 0x01;
    }
    else
    {
      p->Count++;
    }
  }
  else if( p->Input < p->Output )
  {
    if(p->Status != 2)
    {
      p->Count  = 0;
      p->Status = 0x02;
    }
    else
    {
      p->Count++;
    }
  }
  else if( p->Input == p->Output )
  {
    p->Status = 0x00;  
  }
  
  if( p->Count > p->N)
  {
    p->Status = 0;
    p->Output = p->Input;
  }
  
}

/*       [] END OF FILE          */

