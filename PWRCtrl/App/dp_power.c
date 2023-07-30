/*
 ***********************************************************************************
 *
 * Copyright (c) 2017-2021, �������������Ƽ����޹�˾
 * All rights reserved.                        
 *                                             
 ***********************************************************************************
 * @file       dp_power.c
 * @version    v1.1.2
 * @data       2019-11-15
 * @brief      ���ֵ�Դ�Ŀ��Ƴ��� -- ��ѹ����˫�ջ�����
 * @hardware   BB4808G4 V1.2
 * @software   MDK-ARM Version 5.18
 * @attention  1.���������ѧϰʹ�ã��������������κ���;��
 *             2.��������������BUG�����ϿƼ�����Կ��ܷ��������⸺�κ����Ρ�
 *             3.���ǽ����ϵضԸô�����и������ƣ����и��²�������֪ͨ��
 *             4.��Ȩ���У�����ؾ���
 ***********************************************************************************
 */
 
#include "dp_power.h"
#include "dp_filter.h"
#include "dp_para.h"
#include "dp_pid.h"
#include "dr_pwm.h"
#include "dr_adc.h"
#include "math.h"

#define DP_PARA_CAL_ENABLE   ( 0x01 )  //��ѹ��������У׼ʹ��: 0 --> ��ʹ��; 1 --> ʹ��

volatile SYSTEM_FAULT_STRUCT gPSM_FAULT;
volatile SYSTEM_STA_STRUCT   gPSM_STA;

BASE_CMD_STRUCT   gMainCmd;       //��ָ��

float32 gAdcViSampleValue;        //Vint��ADC����ֵ
float32 gAdcIiSampleValue;        //Iint��ADC����ֵ
float32 gAdcVoSampleValue;        //Vout��ADC����ֵ
float32 gAdcIoSampleValue;        //Iout��ADC����ֵ

BURR_FILTER_STRUCT  gBurrFilter_AdcVin;
BURR_FILTER_STRUCT  gBurrFilter_AdcIin;
BURR_FILTER_STRUCT  gBurrFilter_AdcVout;
BURR_FILTER_STRUCT  gBurrFilter_AdcIout;

LOW_FILTER_STRUCT  gLowFilter_ViFdb;
LOW_FILTER_STRUCT  gLowFilter_IiFdb;
LOW_FILTER_STRUCT  gLowFilter_VoFdb;
LOW_FILTER_STRUCT  gLowFilter_IoFdb;

/*
 *   ����У���㷨(һ�κ���)
 *   y = a*x + b; 
 */

/*����У������                  x1*****y1*****x2*****y2*****y******a*********************b   */
ELEC_INFO_STRUCT gVoltOutStr = {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, DP_VOLTAGE_OUT_RATIO, 0.00f}; //�����ѹ����   
ELEC_INFO_STRUCT gCurrOutStr = {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, DP_CURRENT_OUT_RATIO, 0.00f}; //�����������   
ELEC_INFO_STRUCT gVoltInStr  = {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, DP_VOLTAGE_IN_RATIO,  0.00f}; //�����ѹ����   
ELEC_INFO_STRUCT gCurrInStr  = {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, DP_CURRENT_IN_RATIO,  0.00f}; //�����������   

float32 gDefVoltageOutSet = 5.00f; //Ĭ�������ѹ����
float32 gDefCurrentOutSet = 5.00f; //Ĭ�������������


PID_STRUCT gPID_VoltOutLoop; //�����ѹ��PID����


float32 gExcursionZeroIin  = DP_ADC_II_OFFSET;  //����������ƫ��
float32 gExcursionZeroIout = DP_ADC_IO_OFFSET;  //����������ƫ��


/**
 **********************************************************
 * @������void dp_PowerCtrlInit(void)
 * @��������ʼ��
 * @���룺��
 * @���أ��� 
 * @���ߣ��Ρ�СP
 * @�汾��V1.0,2019-11-15
 **********************************************************
**/
void dp_PowerCtrlInit(void)
{

  gPSM_FAULT.all     = 0x00;
  gPSM_STA.all       = 0x00;

#if DP_PARA_CAL_ENABLE == 0x00   //Ĭ�ϲ�ʹ��У׼����

/*
 *   ����˵�ѹ
 */
  gVoltInStr.x1  = 14.87f; // ��һ�ض���
  gVoltInStr.y1  = 15.00f; // ��һ��ʵ��
  gVoltInStr.x2  = 47.76f; // �ڶ��ض���
  gVoltInStr.y2  = 48.00f; // �ڶ���ʵ��
/*
 *   ����˵���
 */
  gCurrInStr.x1  = 0.27f; // ��һ�ض���
  gCurrInStr.y1  = 0.25f; // ��һ��ʵ��
  gCurrInStr.x2  = 5.22f; // �ڶ��ض���
  gCurrInStr.y2  = 5.03f; // �ڶ���ʵ��
/*
 *   ����˵�ѹ
 */
  gVoltOutStr.x1  = 5.00f;  // ��һ�ض���
  gVoltOutStr.y1  = 5.04f;  // ��һ��ʵ��
  gVoltOutStr.x2  = 48.00f; // �ڶ��ض���
  gVoltOutStr.y2  = 48.36f; // �ڶ���ʵ��
/*
 *   ����˵���
 */
  gCurrOutStr.x1  = 0.54f; // ��һ�ض���
  gCurrOutStr.y1  = 0.50f; // ��һ��ʵ��
  gCurrOutStr.x2  = 4.98f; // �ڶ��ض���
  gCurrOutStr.y2  = 4.83f; // �ڶ���ʵ��

#endif

/*
 *   ��ѹ����У׼����
 */
  dp_ElecLineCorr( &gVoltInStr ); //���������ѹУ��ϵ��
  dp_ElecLineCorr( &gCurrInStr ); //�����������У��ϵ��

  dp_ElecLineCorr( &gVoltOutStr );//���������ѹУ��ϵ��
  dp_ElecLineCorr( &gCurrOutStr );//�����������У��ϵ��

/*
 *   ��ѹ�����˲�����ʼ��
 */
  gLowFilter_IoFdb.Fc  = 5e3; //��ֹƵ��Ϊ5KHZ
  gLowFilter_IoFdb.Fs  = 25e3;//����Ƶ��Ϊ25KHZ
  low_filter_init(&gLowFilter_IoFdb);  

  gLowFilter_IiFdb.Fc  = 5e3; //��ֹƵ��Ϊ5KHZ
  gLowFilter_IiFdb.Fs  = 25e3;//����Ƶ��Ϊ25KHZ
  low_filter_init(&gLowFilter_IiFdb);  

  gLowFilter_ViFdb.Fc  = 5e3; //��ֹƵ��Ϊ2KHZ
  gLowFilter_ViFdb.Fs  = 25e3;//����Ƶ��Ϊ25KHZ
  low_filter_init(&gLowFilter_ViFdb);
  
  gLowFilter_VoFdb.Fc  = 5e3; //��ֹƵ��Ϊ2KHZ
  gLowFilter_VoFdb.Fs  = 25e3;//����Ƶ��Ϊ25KHZ
  low_filter_init(&gLowFilter_VoFdb);  
  
  dr_adc_init( ); //��ʼ��ADC
  dr_pwm_init( DP_PWM_FREQUENCY ); //��ʼ��PWM

  pid_func.reset(&gPID_VoltOutLoop);
  gPID_VoltOutLoop.T       = 0.50f;//PID�������ڣ���λ100us
  gPID_VoltOutLoop.Kp      = 10.0f;      
  gPID_VoltOutLoop.Ti      = 0.35f;
  gPID_VoltOutLoop.Td      = 0.01f;
  gPID_VoltOutLoop.Ek_Dead = 0.01f;
  gPID_VoltOutLoop.OutMin  = 0.03f * DP_PWM_PER;//��Сռ�ձ�
  gPID_VoltOutLoop.OutMax  = 1.70f * DP_PWM_PER;//���ռ�ձ�
  pid_func.init(&gPID_VoltOutLoop);

}
/**
 **********************************************************
 * @������void dp_ElecLineCorr(ELEC_INFO_STRUCT *p)
 * @��������ѹ��������У���������� y = a*x + b �е� a��b
 * @���룺��
 * @���أ���
 * @���ߣ��Ρ�СP
 * @�汾��V1.0,2019-11-15
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
 * @������void dp_ExcursionCheck(void)
 * @������������Ư���
 * @���룺��
 * @���أ���
 * @���ߣ��Ρ�СP
 * @�汾��V1.0,2019-11-15
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

  /* ͣ��������200���Ժ�Ž�����Ư���� */
  sys_stop_count = (sys_stop_count >= 200) ? 200 : sys_stop_count+1;
  if((sys_stop_count < 200))
  {
      excursion_total1 = 0;
      excursion_total2 = 0;
      excursion_count  = 0;
      return;
  }

  excursion_total1 += (uint16)( gAdcSampleBuf[0] &  0xFFFF );//�������ADC����ֵ
  excursion_total2 += (uint16)( gAdcSampleBuf[1] &  0xFFFF );//�������ADC����ֵ

  excursion_count  += 1;

  if( excursion_count >= 32 )//ÿ32��У��һ����Ư
  {

    float32 n_Err    = excursion_total1 / 32.0f;
    float32 m_Err    = excursion_total2 / 32.0f;
    excursion_total1 = 0;
    excursion_total2 = 0;
    excursion_count  = 0;
    
    if( fabs(n_Err - DP_ADC_II_OFFSET) < 80 &&  fabs(m_Err - DP_ADC_IO_OFFSET) < 80 )//�ж���Ư�Ƿ����
    {
        gExcursionZeroIin    = n_Err;  //������Ư����
        gExcursionZeroIout   = m_Err;  //������Ư����
        gPSM_FAULT.bit.excu  = 0;      //��ƯУ�����
        gPSM_STA.bit.RDY     = 1;      //����
    }
    else if( (excursion_ErrCnt++) > 5 ) //����5����Ư���󱨹���
    {
        excursion_ErrCnt     = 0;
        excursion_count      = 0;
        gPSM_FAULT.bit.excu  = 1; //��ƯУ�����
        gPSM_STA.bit.RDY     = 0; //��λ�����ź�
    }
  }
}
/**
 **********************************************************
 * @������void dp_BuckCmcControl(void)
 * @������CMCģʽ(��ѹ����˫�ջ�)���Ƴ���,��Լ��ʱ30us
 * @˵��: ��25KHZ���ڵ��ô˺���
 * @���ߣ��Ρ�СP
 * @�汾��V1.0,2019-11-15
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

  /*  ��ADCֵת��Ϊ��Ӧ�ĵ�������  */
  gLowFilter_ViFdb.Input  = (gAdcViSampleValue * gVoltInStr.Coeff  ) + gVoltInStr.Offset;
  gLowFilter_VoFdb.Input  = (gAdcVoSampleValue * gVoltOutStr.Coeff ) + gVoltOutStr.Offset;
  gLowFilter_IiFdb.Input  = (gAdcIiSampleValue * gCurrInStr.Coeff  ) + gCurrInStr.Offset;
  gLowFilter_IoFdb.Input  = (gAdcIoSampleValue * gCurrOutStr.Coeff ) + gCurrOutStr.Offset;

  /*  �������ź��˲�,�Խ��Ͷ���  */
  low_filter_calc(&gLowFilter_ViFdb);
  low_filter_calc(&gLowFilter_IiFdb);
  low_filter_calc(&gLowFilter_VoFdb);
  low_filter_calc(&gLowFilter_IoFdb);

  gVoltOutStr.Value  = gLowFilter_VoFdb.Output;
  gCurrOutStr.Value  = gLowFilter_IoFdb.Output;
  gVoltInStr.Value   = gLowFilter_ViFdb.Output;
  gCurrInStr.Value   = gLowFilter_IiFdb.Output;

  iovp_timecount = gVoltInStr.Value        > DP_VOLTAGV_IN_MAX  ? ++iovp_timecount : 0;//�����ѹ��ʱ
  oovp_timecount = gVoltOutStr.Value       > DP_VOLTAGV_OUT_MAX ? ++oovp_timecount : 0;//�����ѹ��ʱ
  iocp_timecount = fabs(gCurrInStr.Value)  > DP_CURRENT_IN_MAX  ? ++iocp_timecount : 0;//���������ʱ
  oocp_timecount = fabs(gCurrOutStr.Value) > DP_CURRENT_OUT_MAX ? ++oocp_timecount : 0;//���������ʱ
  uvp_timecount  = gVoltInStr.Value        < DP_VOLTAGV_IN_MIN  ? ++uvp_timecount  : 0;//Ƿѹ��ʱ


  if( gPSM_STA.bit.PWRUP == 0x01 )
  {
    if( iocp_timecount > 25 )gPSM_FAULT.bit.iocp  = 1;  //�����������  
    if( oocp_timecount > 25 )gPSM_FAULT.bit.oocp  = 1;  //�����������
    if( iovp_timecount > 25  )gPSM_FAULT.bit.iovp = 1;  //�����ѹ����
    if( oovp_timecount > 25  )gPSM_FAULT.bit.oovp = 1;  //�����ѹ����
    if( uvp_timecount  > 50 )gPSM_FAULT.bit.iuvp  = 1;  //Ƿѹ����
  }

  if( gPSM_FAULT.all != 0 )
  {
    gPSM_STA.bit.FAUT = 0x01;
		
    j  += 1; //�ָ�����ʱ���ʱ
		
    if(j >= RESET_FAULT_TIME)
    {
      j = 0;
      gPSM_FAULT.all = 0x00;//��λ����
    }
  }
  
  gPSM_STA.bit.RUN  = !gPSM_STA.bit.FAUT && gPSM_STA.bit.RDY && gPSM_STA.bit.EN&& gPSM_STA.bit.PWRUP;
  
  if( gPSM_STA.bit.RUN == 0) // �������
  {

    dr_pwm_stop();// PWMֹͣ���
    
    pid_func.clc(&gPID_VoltOutLoop); // ���gPID_VoltOutLoop

    k = gVoltOutStr.Value / gVoltInStr.Value; //������������ѹ֮��

    if(k > 1.70f)k = 1.70f;
    if(k < 0.01f)k = 0.01f;

    m_pwm_cmp = k * DP_PWM_PER; //����Ԥ��ռ�ձ�

    gPID_VoltOutLoop.Output = m_pwm_cmp; // ���汾��PWM�Ƚ�ֵ

    dr_pwm_update( m_pwm_cmp ); // PWMԤ��

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

  m_vout_ref = fminf( gMainCmd.VoRefSet, DP_VOLTAGV_OUT_MAX );//�����ѹ���ò��ܳ���Ӳ�������ֵ

  /*
   *  �����ѹ��PID����
   */
  gPID_VoltOutLoop.Ref = m_vout_ref;
  gPID_VoltOutLoop.Fdb = gVoltOutStr.Value;
  pid_func.calc( &gPID_VoltOutLoop );

	m_pwm_cmp = gPID_VoltOutLoop.Output;

  dr_pwm_update( m_pwm_cmp ); // ����PWM

  dr_pwm_start( );// PWM�������

}

/*
 *   [] END OF FILE
 */
