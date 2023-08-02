/*
 ***********************************************************************************
 *
 * Copyright (c) 2017-2020, 柳州启明电气科技有限公司
 * All rights reserved.                        
 *                                             
 ***********************************************************************************
 * @file       dp_pid.c
 *
 * @version    V1.1
 *
 * @date       2017-08-22
 *
 * @brief      增量式PID算法
 *
 * @attention  1.本代码仅做学习使用，不得用于其他任何用途。
 *             2.本代码难免会存在BUG，安合科技不会对可能发生的意外负任何责任。
 *             3.我们将不断地对该代码进行更新完善，如有更新不会另行通知。
 *             4.版权所有，盗版必究。
 ***********************************************************************************
 */

#include "dp_pid.h"
#include "math.h"

/* 函数声明，局部函数不对外公开 */
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
 * @描述：PID参数计算
 * @输入：*p：定义的PID数据
 * @返回：无 
 * @作者：何。小P
 * @版本：V1.0
 **********************************************************
 */
void pid_init( PID_STRUCT *p)
{   
  if(p->T  <= 0)p->T  = 1.0f;
  if(p->Ti == 0)p->Ti = 3.4E+38;//Ti=0时设为尽量大的值
  if(p->Ek_Dead < 0)p->Ek_Dead = 0;
  
  p->a0  = p->Kp*(1.0f + p->T/p->Ti + p->Td/p->T);
  p->a1  = p->Kp*(1.0f + p->Td/p->T * 2.0f);
  p->a2  = p->Kp*p->Td/p->T;

}
/**
 **********************************************************
 * @描述：PID数据复位
 * @输入：*p：定义的PID数据
 * @返回：无 
 * @作者：何。小P
 * @版本：V1.0
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

  p->T       = 0;     //周期
  p->Kp      = 0;     //比例系数P
  p->Ti      = 0;     //积分时间I
  p->Td      = 0;     //微分时间D
  
  p->Ek_Dead   = 0;  
  
  p->Increm    = 0;
  p->Output    = 0;
  
  p->OutMax    = 0;
  p->OutMin    = 0; 
}
/**
 **********************************************************
 * @描述：清楚PID计算中的缓存
 * @输入：*p：定义的PID数据
 * @返回：无 
 * @作者：何。小P
 * @版本：V1.0
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
 * @描述：PID增量式计算
 * @输入：*p：PID结构体 ref:参考值 fbd: 反馈值
 * @返回：PID运算结果   
 * @作者：何。小P
 * @版本：V1.1
 **********************************************************
 */
#pragma arm section code = "SRAMCODE"
void pid_calc( PID_STRUCT *p)
{
  
  p->Ek_0 = p->Ref - p->Fdb;  //计算误差

  //误差小于设定死区，不进行PID计算，保持上一次输出
  if( fabs(p->Ek_0) < p->Ek_Dead )
  {
    p->Increm = 0;
  }
  else
  {
    p->Increm = (   p->a0 * p->Ek_0  \
                  - p->a1 * p->Ek_1 \
                  + p->a2 * p->Ek_2 ); //PID增量计算  
  }

  p->Output += p->Increm; //计算输出

  p->Ek_2        = p->Ek_1; //保存k-2误差
  p->Ek_1        = p->Ek_0; //保存k-1误差
  
  if(p->Output > p->OutMax)
  {
    p->Output   =  p->OutMax; //最大限幅
    return;
  }
  if(p->Output < p->OutMin)
  {
    p->Output   =  p->OutMin; //最小限幅
    return;
  }

}
#pragma arm section


/*       [] END OF FILE          */



