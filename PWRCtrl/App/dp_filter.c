/*
 ***********************************************************************************
 *
 * @file       dp_filter.c
 * @brief      数字滤波算法
 * @author     hepeng(1130001324@qq.com)
 * @attention  1.本代码仅做学习使用，不得用于其他任何用途。
 *             2.本代码难免会存在BUG，安合科技不会对可能发生的意外负任何责任。
 *             3.我们将不断地对该代码进行更新完善，如有更新不会另行通知。
 *             4.版权所有，盗版必究。
 *
 * Copyright (c) 2018-2020, 柳州启明电气科技有限公司 All rights reserved. 
 * 
 * History:
 ***********************************************************************************
 * 2018-08-22 hepeng 发布v1.0
 * 2018-12-22 hepeng 发布v1.1，增加低通滤波器、限幅滤波器
 * 2019-01-15 hepeng 发布v1.2，增加消抖滤波器、去毛刺滤波器
 */


#include "dp_filter.h"
#include "string.h"
#include "math.h"

/**
 * @描述：滑动平均值滤波初始化
 * @输入：NewValue:新的采样采样值
 * @返回：无
 * @作者：何。小P
 * @版本：V1.0,2018-08-22
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
 * @描述：滑动平均值滤波
 * @输入：NewValue:新的采样采样值
 * @返回：无
 * @作者：何。小P
 * @版本：V1.0,2018-08-22
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
 * @描述：低通滤波器初始化
 * @输入：LOW_FILTER_STRUCT结构体
 * @返回：无
 * @作者：何。小P
 * @版本：V1.0,2018-12-22
 ******************************************************************************
 */
void low_filter_init(LOW_FILTER_STRUCT *p)
{
  float32 Tc;//时间常数τ
  
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
 * @描述：低通滤波器
 * @输入：LOW_FILTER_STRUCT结构体
 * @返回：无
 * @作者：何。小P
 * @版本：V1.0,2018-12-22
 ******************************************************************************
 */
void low_filter_calc(LOW_FILTER_STRUCT *p)
{
  if(p->Output == p->Input)return;

  p->Output = p->Ka * p->Input + p->Kb * p->Output;

}

/**
 ******************************************************************************
 * @描述：限幅滤波器初始化
 * @输入：LIM_FILTER_STRUCT结构体
 * @返回：无
 * @作者：何。小P
 * @版本：V1.0,2018-12-22
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
 * @描述：限幅滤波器
 * @输入：LIM_FILTER_STRUCT结构体
 * @返回：无
 * @作者：何。小P
 * @版本：V1.0,2018-12-22
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
 * @描述：去毛刺滤波器
 * @输入：BURR_FILTER_STRUCT结构体
 * @返回：无
 * @作者：何。小P
 * @版本：V1.0,2019-1-15
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
 * @描述：去毛刺滤波器
 * @输入：BURR_FILTER_STRUCT结构体
 * @返回：无
 * @作者：何。小P
 * @版本：V1.0,2019-1-15
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
 * @描述：消抖滤波器初始化
 * @输入：BURR_FILTER_STRUCT结构体
 * @返回：无
 * @作者：何。小P
 * @版本：V1.0,2019-1-15
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
 * @描述：消抖滤波器
 * @输入：shak_FILTER_STRUCT结构体
 * @返回：无
 * @作者：何。小P
 * @版本：V1.0,2019-1-15
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

