#include "power.h"

uint8_t buck_boost_en=0;
PID_STRUCT gPID_VoltOutLoop; //�����ѹ��PID����
/*����У������                  x1*****y1*****x2*****y2*****y******a*********************b   */
ELEC_INFO_STRUCT v_in1_struct = {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, DP_VOLTAGE_OUT_RATIO, 0.00f}; //�����ѹ����  
ELEC_INFO_STRUCT i_in1_struct = {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, DP_VOLTAGE_OUT_RATIO, 0.00f}; //�����ѹ���� 



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

void eg2104_sd_init()
{
    rcu_periph_clock_enable(RCU_GPIOE);
    gpio_mode_set(GPIOE,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLDOWN,GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6);
    gpio_output_options_set(GPIOE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6);
    
}

void buck_boost_init()
{
    pid_func.reset(&gPID_VoltOutLoop);
    gPID_VoltOutLoop.T       = 0.50f;//PID�������ڣ���λ100us
    gPID_VoltOutLoop.Kp      = 10.0f;      
    gPID_VoltOutLoop.Ti      = 0.35f;
    gPID_VoltOutLoop.Td      = 0.02f;
    gPID_VoltOutLoop.Ek_Dead = 0.01f;
    gPID_VoltOutLoop.OutMin  = 0.03f * DP_PWM_PER;//��Сռ�ձ�
    gPID_VoltOutLoop.OutMax  = 1.70f * DP_PWM_PER;//���ռ�ձ�
    pid_func.init(&gPID_VoltOutLoop);
    adc_config();
    pwm_config();
}
void power_ctrl_buck_boost()
{
    gPID_VoltOutLoop.Ref = 12.f;
    gPID_VoltOutLoop.Fdb = (float)adc0_value[0]*3.3f/4096.f*20;
    pid_func.calc( &gPID_VoltOutLoop );
    timer1_set_pwm(gPID_VoltOutLoop.Output);
    
}
/*�� 10KHz��������������Ե��Ƶ�����SPWM*/
void power_ctrl_spwm()
{

    static uint16_t count=0;
    if(count==0)
    {
        timer_channel_output_pulse_value_config(TIMER7,TIMER_CH_1,0);
    }
    if(count<100)
    {
        timer_channel_output_pulse_value_config(TIMER7,TIMER_CH_0,(uint16_t)(_sin((float)count/200*_2PI)*TIMER_CAR(TIMER7)));
    }
    if(count==100)
    {
        timer_channel_output_pulse_value_config(TIMER7,TIMER_CH_0,0);
    
    }
    if(count >=100)
    {       
        timer_channel_output_pulse_value_config(TIMER7,TIMER_CH_1,(uint16_t)(_sin((float)(count-100)/200*_2PI)*TIMER_CAR(TIMER7)));
    }
    count++;
    if(count==200)
    {
        count=0;
    }
    
}

void power_ctrl_pfc()
{
}
void TIMER7_UP_TIMER12_IRQHandler()
{
    if(timer_flag_get(TIMER7,TIMER_FLAG_UP) == SET )
    {
        timer_flag_clear(TIMER7,TIMER_FLAG_UP);
        gpio_bit_toggle(GPIOE,GPIO_PIN_4);
        if(buck_boost_en==1)
        {
            power_ctrl_buck_boost();
        }
        power_ctrl_spwm();
    }
}

void timer1_set_pwm(uint16_t pwm_cmp_value)
{
    uint16_t buck_duty  = 0;
    uint16_t boost_duty = 0;

  /* 
   * ����ѹ��PWM������
   * 1.����MOS�����Ծٵ���������ʽ�����Ϲܲ���100%ռ�ձȵ�ͨ������������Ϊ90%��
   * 2.���������������Ϊ�ܵ�ռ�ձȣ���Ҫ�ֱ�����������ŵ�ռ�ձȡ�
   * 3.��ѹ����ʱ��BOOST�İ��ŵ�ռ�ձ��ǹ̶���Ϊ90%��ͨ���ı�BUCK����ռ�ձ�ʵ����ѹ��
   * 3.��ѹ���ѹ����ʱ��BUCK���ŵ�ռ�ձ��ǹ̶���Ϊ90%��ͨ���ı�BOOST����ռ�ձ�ʵ����ѹ��
   */  

  if( pwm_cmp_value >= MAX_PWM_CMP)
  {
    boost_duty  = pwm_cmp_value - MAX_PWM_CMP + MIN_PWM_CMP;//BOOST����ռ�ձȼ���
  }
  else
  {
    boost_duty  = MIN_PWM_CMP;  //BUCKģʽ,boost_duty���̶�ռ�ձ�
  }

  if( pwm_cmp_value >= MAX_PWM_CMP)
  {
    buck_duty  = MAX_PWM_CMP;   //BOOSTģʽ,buck_duty���̶�ռ�ձ�
  }
  else
  {
    buck_duty  = pwm_cmp_value;//BUCK����ռ�ձ��趨
  }

  /*
   *  ռ�ձ�����
   */
  if( boost_duty > MAX_PWM_CMP )boost_duty = MAX_PWM_CMP;
  if( boost_duty < MIN_PWM_CMP )boost_duty = MIN_PWM_CMP;
  if( buck_duty  > MAX_PWM_CMP )buck_duty  = MAX_PWM_CMP;
  if( buck_duty  < MIN_PWM_CMP )buck_duty  = MIN_PWM_CMP;
	
	/*
   *  ���±ȽϼĴ���
   */
//  HRTIM1->sMasterRegs.MCMP1R  = ( DP_PWM_PER - buck_duty  ) >> 1;
//  HRTIM1->sMasterRegs.MCMP2R  = ( DP_PWM_PER + buck_duty  ) >> 1;
//  HRTIM1->sMasterRegs.MCMP3R  = ( DP_PWM_PER + boost_duty ) >> 1;
//  HRTIM1->sMasterRegs.MCMP4R  = ( DP_PWM_PER - boost_duty ) >> 1;
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_0,DP_PWM_PER-buck_duty);
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,DP_PWM_PER-boost_duty);
}
