/**
  ******************************************************************************
  * @author  何。小P
  * @version V1.0
  * @date    2017-08-22
  * @brief   柳州启明电气科技有限公司 版权所有（C）All rights reserved.
  ******************************************************************************
  * 
  *  文件名：dp_pid.h
  *  功  能：PID算法头文件
  *
  ******************************************************************************
**/

#ifndef __DP_PID_H
#define __DP_PID_H

typedef struct  _PID{ 
  
  float  T;       //输入:周期
  float  Kp;      //输入:比例
  float  Ti;      //输入:积分
  float  Td;      //输入:微分

  float  a0;      //计算值:a0 = Kp(1.0 + T/Ti + Td/T)
  float  a1;      //计算值:a1 = Kp(1.0 + 2*Td/T)
  float  a2;      //计算值:a2 = Kp*Td/T

  float Ref;     //基准值
  float Fdb;     //反馈值
  
  float Ek_Dead; //误差死区
  
  float  Ek_0;    //计算值:Error[k-0],本次误差
  float  Ek_1;    //计算值:Error[k-1],前一次误差
  float  Ek_2;    //计算值:Error[k-2],前二次误差

  float  Increm;   //计算值:增量值
  float  Output;  //计算值:PID输出值

  float  OutMax;   //输入:PID输出最大值
  float  OutMin;   //输入:PID输出最小值
  
}PID_STRUCT;


typedef struct _PID_FUNC{

  void (*reset) ( PID_STRUCT *p);//复位PID，将PID所有的参数都清零
  void (*init ) ( PID_STRUCT *p);//初始化PID，计算a0,a1,a2等系数
  void (*clc  ) ( PID_STRUCT *p);//清除PID过程数据，在停止运行PID时用到
  void (*calc ) ( PID_STRUCT *p);//PID计算

}PID_FUNC_STRUCT;


extern PID_FUNC_STRUCT pid_func;

#endif


