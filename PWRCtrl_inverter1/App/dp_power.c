/*
 ***********************************************************************************
 *
 * Copyright (c) 2017-2021, 柳州启明电气科技有限公司
 * All rights reserved.                        
 *                                             
 ***********************************************************************************
 * @file       dp_power.c
 * @version    v1.1.2
 * @data       2019-11-15
 * @brief      数字电源的控制程序 -- 电压电流双闭环控制
 * @hardware   BB4808G4 V1.2
 * @software   MDK-ARM Version 5.18
 * @attention  1.本代码仅做学习使用，不得用于其他任何用途。
 *             2.本代码难免会存在BUG，安合科技不会对可能发生的意外负任何责任。
 *             3.我们将不断地对该代码进行更新完善，如有更新不会另行通知。
 *             4.版权所有，盗版必究。
 ***********************************************************************************
 */
 
#include "dp_power.h"
#include "dp_filter.h"
#include "dp_para.h"
#include "dp_pid.h"
#include "dr_pwm.h"
#include "dr_adc.h"
#include "math.h"

#define DP_PARA_CAL_ENABLE   ( 0x01 )  //电压电流采样校准使能: 0 --> 不使能; 1 --> 使能

volatile SYSTEM_FAULT_STRUCT gPSM_FAULT;
volatile SYSTEM_STA_STRUCT   gPSM_STA;

BASE_CMD_STRUCT   gMainCmd;       //主指令

float32 gAdcViSampleValue;        //Vint的ADC采样值
float32 gAdcIiSampleValue;        //Iint的ADC采样值
float32 gAdcVoSampleValue;        //Vout的ADC采样值
float32 gAdcIoSampleValue;        //Iout的ADC采样值

BURR_FILTER_STRUCT  gBurrFilter_AdcVin;
BURR_FILTER_STRUCT  gBurrFilter_AdcIin;
BURR_FILTER_STRUCT  gBurrFilter_AdcVout;
BURR_FILTER_STRUCT  gBurrFilter_AdcIout;

LOW_FILTER_STRUCT  gLowFilter_ViFdb;
LOW_FILTER_STRUCT  gLowFilter_IiFdb;
LOW_FILTER_STRUCT  gLowFilter_VoFdb;
LOW_FILTER_STRUCT  gLowFilter_IoFdb;

/*
 *   线性校正算法(一次函数)
 *   y = a*x + b; 
 */

/*定义校正参数                  x1*****y1*****x2*****y2*****y******a*********************b   */
ELEC_INFO_STRUCT gVoltOutStr = {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, DP_VOLTAGE_OUT_RATIO, 0.00f}; //输出电压参数   
ELEC_INFO_STRUCT gCurrOutStr = {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, DP_CURRENT_OUT_RATIO, 0.00f}; //输出电流参数   
ELEC_INFO_STRUCT gVoltInStr  = {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, DP_VOLTAGE_IN_RATIO,  0.00f}; //输入电压参数   
ELEC_INFO_STRUCT gCurrInStr  = {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, DP_CURRENT_IN_RATIO,  0.00f}; //输入电流参数   

float32 gDefVoltageOutSet = 5.00f; //默认输出电压设置
float32 gDefCurrentOutSet = 5.00f; //默认输出电流设置


PID_STRUCT gPID_VoltOutLoop; //输出电压环PID数据


float32 gExcursionZeroIin  = DP_ADC_II_OFFSET;  //输入电流零点偏移
float32 gExcursionZeroIout = DP_ADC_IO_OFFSET;  //输出电流零点偏移


/**
 **********************************************************
 * @函数：void dp_PowerCtrlInit(void)
 * @描述：初始化
 * @输入：无
 * @返回：无 
 * @作者：何。小P
 * @版本：V1.0,2019-11-15
 **********************************************************
**/
void dp_PowerCtrlInit(void)
{

  gPSM_FAULT.all     = 0x00;
  gPSM_STA.all       = 0x00;

#if DP_PARA_CAL_ENABLE == 0x00   //默认不使能校准功能

/*
 *   输入端电压
 */
  gVoltInStr.x1  = 14.87f; // 第一回读点
  gVoltInStr.y1  = 15.00f; // 第一真实点
  gVoltInStr.x2  = 47.76f; // 第二回读点
  gVoltInStr.y2  = 48.00f; // 第二真实点
/*
 *   输入端电流
 */
  gCurrInStr.x1  = 0.27f; // 第一回读点
  gCurrInStr.y1  = 0.25f; // 第一真实点
  gCurrInStr.x2  = 5.22f; // 第二回读点
  gCurrInStr.y2  = 5.03f; // 第二真实点
/*
 *   输出端电压
 */
  gVoltOutStr.x1  = 5.00f;  // 第一回读点
  gVoltOutStr.y1  = 5.04f;  // 第一真实点
  gVoltOutStr.x2  = 48.00f; // 第二回读点
  gVoltOutStr.y2  = 48.36f; // 第二真实点
/*
 *   输出端电流
 */
  gCurrOutStr.x1  = 0.54f; // 第一回读点
  gCurrOutStr.y1  = 0.50f; // 第一真实点
  gCurrOutStr.x2  = 4.98f; // 第二回读点
  gCurrOutStr.y2  = 4.83f; // 第二真实点

#endif

/*
 *   电压电流校准处理
 */
  dp_ElecLineCorr( &gVoltInStr ); //计算输入电压校正系数
  dp_ElecLineCorr( &gCurrInStr ); //计算输入电流校正系数

  dp_ElecLineCorr( &gVoltOutStr );//计算输出电压校正系数
  dp_ElecLineCorr( &gCurrOutStr );//计算输出电流校正系数

/*
 *   电压电流滤波器初始化
 */
  gLowFilter_IoFdb.Fc  = 5e3; //截止频率为5KHZ
  gLowFilter_IoFdb.Fs  = 25e3;//采样频率为25KHZ
  low_filter_init(&gLowFilter_IoFdb);  

  gLowFilter_IiFdb.Fc  = 5e3; //截止频率为5KHZ
  gLowFilter_IiFdb.Fs  = 25e3;//采样频率为25KHZ
  low_filter_init(&gLowFilter_IiFdb);  

  gLowFilter_ViFdb.Fc  = 5e3; //截止频率为2KHZ
  gLowFilter_ViFdb.Fs  = 25e3;//采样频率为25KHZ
  low_filter_init(&gLowFilter_ViFdb);
  
  gLowFilter_VoFdb.Fc  = 5e3; //截止频率为2KHZ
  gLowFilter_VoFdb.Fs  = 25e3;//采样频率为25KHZ
  low_filter_init(&gLowFilter_VoFdb);  
  
  dr_adc_init( ); //初始化ADC
  dr_pwm_init( DP_PWM_FREQUENCY ); //初始化PWM

  pid_func.reset(&gPID_VoltOutLoop);
  gPID_VoltOutLoop.T       = 0.50f;//PID控制周期，单位100us
  gPID_VoltOutLoop.Kp      = 10.0f;      
  gPID_VoltOutLoop.Ti      = 0.35f;
  gPID_VoltOutLoop.Td      = 0.01f;
  gPID_VoltOutLoop.Ek_Dead = 0.01f;
  gPID_VoltOutLoop.OutMin  = 0.03f * DP_PWM_PER;//最小占空比
  gPID_VoltOutLoop.OutMax  = 1.70f * DP_PWM_PER;//最大占空比
  pid_func.init(&gPID_VoltOutLoop);

}
/**
 **********************************************************
 * @函数：void dp_ElecLineCorr(ELEC_INFO_STRUCT *p)
 * @描述：电压电流线性校正参数计算 y = a*x + b 中的 a与b
 * @输入：无
 * @返回：无
 * @作者：何。小P
 * @版本：V1.0,2019-11-15
 **********************************************************
**/
void dp_ElecLineCorr(ELEC_INFO_STRUCT *p)
{

  float32 a;
  float32 b;

  if( (p->y2 - p->y1 == 0) || (p->x2 - p->x1 == 0) )
  {
    return;
  }

  a = (p->y2 - p->y1)/(p->x2 - p->x1);
  b = (p->y1 - p->x1 * a);

  p->Value  = 0;
  p->Coeff  = a * p->Coeff;
  p->Offset = b; 

}
/**
 **********************************************************
 * @函数：void dp_ExcursionCheck(void)
 * @描述：电流零漂检测
 * @输入：无
 * @返回：无
 * @作者：何。小P
 * @版本：V1.0,2019-11-15
 **********************************************************
**/
void dp_ExcursionCheck(void)
{

  static uint8   sys_stop_count   = 0;
  static uint8   excursion_count  = 0;
  static uint8   excursion_ErrCnt = 0;
  static float32 excursion_total1 = 0;  
  static float32 excursion_total2 = 0;  

  if(gPSM_STA.bit.RUN != 0)
  {
      sys_stop_count = 0;
      return;
  }

  /* 停机计数，200次以后才进行零漂检验 */
  sys_stop_count = (sys_stop_count >= 200) ? 200 : sys_stop_count+1;
  if((sys_stop_count < 200))
  {
      excursion_total1 = 0;
      excursion_total2 = 0;
      excursion_count  = 0;
      return;
  }

  excursion_total1 += (uint16)( gAdcSampleBuf[0] &  0xFFFF );//输入电流ADC采样值
  excursion_total2 += (uint16)( gAdcSampleBuf[1] &  0xFFFF );//输出电流ADC采样值

  excursion_count  += 1;

  if( excursion_count >= 32 )//每32拍校验一次零漂
  {

    float32 n_Err    = excursion_total1 / 32.0f;
    float32 m_Err    = excursion_total2 / 32.0f;
    excursion_total1 = 0;
    excursion_total2 = 0;
    excursion_count  = 0;
    
    if( fabs(n_Err - DP_ADC_II_OFFSET) < 80 &&  fabs(m_Err - DP_ADC_IO_OFFSET) < 80 )//判断零漂是否过大
    {
        gExcursionZeroIin    = n_Err;  //更新零漂数据
        gExcursionZeroIout   = m_Err;  //更新零漂数据
        gPSM_FAULT.bit.excu  = 0;      //零漂校验完成
        gPSM_STA.bit.RDY     = 1;      //就绪
    }
    else if( (excursion_ErrCnt++) > 5 ) //连续5次零漂过大报故障
    {
        excursion_ErrCnt     = 0;
        excursion_count      = 0;
        gPSM_FAULT.bit.excu  = 1; //零漂校验错误
        gPSM_STA.bit.RDY     = 0; //复位就绪信号
    }
  }
}
/**
 **********************************************************
 * @函数：void dp_BuckCmcControl(void)
 * @描述：CMC模式(电压电流双闭环)控制程序,大约用时30us
 * @说明: 以25KHZ周期调用此函数
 * @作者：何。小P
 * @版本：V1.0,2019-11-15
 **********************************************************
**/
void dp_PowerCmcCtrl(void)
{
  static uint16  j;
  static uint16  oocp_timecount = 0;
  static uint16  oovp_timecount = 0;
  static uint16  iocp_timecount = 0;
  static uint16  iovp_timecount = 0;
  static uint16  uvp_timecount  = 0;

	static float32 m_pwm_cmp;
	
  float32 k;

  float32 m_vout_ref;

	
  gAdcIiSampleValue  = (uint16)( gAdcSampleBuf[0] &  0xFFFF ) - gExcursionZeroIin;
  gAdcIoSampleValue  = (uint16)( gAdcSampleBuf[1] &  0xFFFF ) - gExcursionZeroIout;
  gAdcVoSampleValue  = (uint16)( gAdcSampleBuf[0] /  0xFFFF );
  gAdcViSampleValue  = (uint16)( gAdcSampleBuf[1] /  0xFFFF );

  /*  将ADC值转换为相应的电气参数  */
  gLowFilter_ViFdb.Input  = (gAdcViSampleValue * gVoltInStr.Coeff  ) + gVoltInStr.Offset;
  gLowFilter_VoFdb.Input  = (gAdcVoSampleValue * gVoltOutStr.Coeff ) + gVoltOutStr.Offset;
  gLowFilter_IiFdb.Input  = (gAdcIiSampleValue * gCurrInStr.Coeff  ) + gCurrInStr.Offset;
  gLowFilter_IoFdb.Input  = (gAdcIoSampleValue * gCurrOutStr.Coeff ) + gCurrOutStr.Offset;

  /*  对输入信号滤波,以降低抖动  */
  low_filter_calc(&gLowFilter_ViFdb);
  low_filter_calc(&gLowFilter_IiFdb);
  low_filter_calc(&gLowFilter_VoFdb);
  low_filter_calc(&gLowFilter_IoFdb);

  gVoltOutStr.Value  = gLowFilter_VoFdb.Output;
  gCurrOutStr.Value  = gLowFilter_IoFdb.Output;
  gVoltInStr.Value   = gLowFilter_ViFdb.Output;
  gCurrInStr.Value   = gLowFilter_IiFdb.Output;

  iovp_timecount = gVoltInStr.Value        > DP_VOLTAGV_IN_MAX  ? ++iovp_timecount : 0;//输入过压计时
  oovp_timecount = gVoltOutStr.Value       > DP_VOLTAGV_OUT_MAX ? ++oovp_timecount : 0;//输出过压计时
  iocp_timecount = fabs(gCurrInStr.Value)  > DP_CURRENT_IN_MAX  ? ++iocp_timecount : 0;//输入过流计时
  oocp_timecount = fabs(gCurrOutStr.Value) > DP_CURRENT_OUT_MAX ? ++oocp_timecount : 0;//输出过流计时
  uvp_timecount  = gVoltInStr.Value        < DP_VOLTAGV_IN_MIN  ? ++uvp_timecount  : 0;//欠压计时


  if( gPSM_STA.bit.PWRUP == 0x01 )
  {
    if( iocp_timecount > 25 )gPSM_FAULT.bit.iocp  = 1;  //输入过流保护  
    if( oocp_timecount > 25 )gPSM_FAULT.bit.oocp  = 1;  //输出过流保护
    if( iovp_timecount > 25  )gPSM_FAULT.bit.iovp = 1;  //输入过压保护
    if( oovp_timecount > 25  )gPSM_FAULT.bit.oovp = 1;  //输出过压保护
    if( uvp_timecount  > 50 )gPSM_FAULT.bit.iuvp  = 1;  //欠压保护
  }

  if( gPSM_FAULT.all != 0 )
  {
    gPSM_STA.bit.FAUT = 0x01;
		
    j  += 1; //恢复运行时间计时
		
    if(j >= RESET_FAULT_TIME)
    {
      j = 0;
      gPSM_FAULT.all = 0x00;//复位故障
    }
  }
  
  gPSM_STA.bit.RUN  = !gPSM_STA.bit.FAUT && gPSM_STA.bit.RDY && gPSM_STA.bit.EN&& gPSM_STA.bit.PWRUP;
  
  if( gPSM_STA.bit.RUN == 0) // 运行与否？
  {

    dr_pwm_stop();// PWM停止输出
    
    pid_func.clc(&gPID_VoltOutLoop); // 清除gPID_VoltOutLoop

    k = gVoltOutStr.Value / gVoltInStr.Value; //计算输出输入电压之比

    if(k > 1.70f)k = 1.70f;
    if(k < 0.01f)k = 0.01f;

    m_pwm_cmp = k * DP_PWM_PER; //计算预置占空比

    gPID_VoltOutLoop.Output = m_pwm_cmp; // 保存本次PWM比较值

    dr_pwm_update( m_pwm_cmp ); // PWM预置

    gPSM_STA.bit.RUN    = 0;
    gPSM_STA.bit.CHARGE = 0;
    gPSM_STA.bit.DISCHA = 0;
    gPSM_STA.bit.CC     = 0;
    gPSM_STA.bit.CV     = 0;
    gPSM_STA.bit.CW     = 0;
    gPSM_STA.bit.MPPT   = 0;

    m_pwm_cmp = 0;   

    return;
  }

  m_vout_ref = fminf( gMainCmd.VoRefSet, DP_VOLTAGV_OUT_MAX );//输出电压设置不能超过硬件的最大值

  /*
   *  输出电压环PID计算
   */
  gPID_VoltOutLoop.Ref = m_vout_ref;
  gPID_VoltOutLoop.Fdb = gVoltOutStr.Value;
  pid_func.calc( &gPID_VoltOutLoop );

	m_pwm_cmp = gPID_VoltOutLoop.Output;

  dr_pwm_update( m_pwm_cmp ); // 更新PWM

  dr_pwm_start( );// PWM启动输出

}

/*
 *   [] END OF FILE
 */
