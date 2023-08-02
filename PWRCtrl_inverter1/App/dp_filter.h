/**
  ******************************************************************************
  * @author  何。小P
  * @version V1.0
  * @date    2018-08-22
  * @brief   柳州启明电气科技有限公司 版权所有（C）All rights reserved.
  ******************************************************************************
  * 
  *  文件名：dp_filter.h
  *  功  能：数字滤波算法
  *
  ******************************************************************************
**/ 


#ifndef _DP_FILTER_H
#define _DP_FILTER_H

#include "system_type.h"



/*  定义圆周率π  */
#ifndef PI
#define PI    3.14159265F // π
#define PI2   6.28318530F // 2π
#endif

/*  低通滤波器数据  */
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

/*  限幅滤波器数据  */
typedef struct{

  float32  Slope; //变化率
  float32  Fs;    //采样频率
  float32  Input; //输入
  float32  Output;//输出
  float32  LastInput;//上一次输入
  float32  LimValue;//限幅值 = 变化率 * 采样周期

}LIM_FILTER_STRUCT;

/*  平均值滤波器数据  */
#define AverageFilterBuf_MaxLen 16u

typedef struct{

  float32 Base[AverageFilterBuf_MaxLen];
  float32 Sum;
  float32 Mean;
  uint8   FullFlag;
  uint16  Head;
  uint16  Rear;

}MEAN_FILTER_STRUCT;

/*  去除毛刺滤波处理的数据结构  */
typedef struct {          
  float32 Input;    //本次采样数据
  float32 Output;   //当前使用数据
  float32 Err;      //偏差限值
  float32 Max;      //最大偏差值
  float32 Min;      //最小偏差值
}BURR_FILTER_STRUCT;

/*  消抖滤波器的数据结构  */
typedef struct {          
  float32 Input;    //本次采样数据
  float32 Output;   //当前使用数据
  uint16  N;        //计数值
  uint16  Count;    //计数器
  uint16  Status;   //状态
}SHAK_FILTER_STRUCT;

/*  TD微分跟踪器的数据结构  */
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










