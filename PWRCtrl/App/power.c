#include "power.h"

uint8_t buck_boost_en=0;
uint16_t pfc_active_pwm=00;
PID_STRUCT gPID_VoltOutLoop; //�����ѹ��PID����
PID_STRUCT gPID_PFC_I_Loop; //pfc

LOW_FILTER_STRUCT  lowfilter_vout_buckboost;
LOW_FILTER_STRUCT  lowfilter_vout_pfc;

/*����У������                  x1*****y1*****x2*****y2*****y******a*********************b   */
ELEC_INFO_STRUCT v_in1_struct = {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, DP_VOLTAGE_OUT_RATIO, 0.00f}; //�����ѹ����  
ELEC_INFO_STRUCT i_in1_struct = {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, DP_VOLTAGE_OUT_RATIO, 0.00f}; //�����ѹ���� 
ELEC_INFO_STRUCT acv_in1_struct = {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, DP_VOLTAGE_OUT_RATIO, 0.00f}; //�����ѹ���� 


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

void zcd_init()
{
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_SYSCFG);
    
    gpio_mode_set(GPIOG,GPIO_MODE_INPUT,GPIO_PUPD_NONE,GPIO_PIN_6);
    nvic_irq_enable(EXTI5_9_IRQn,0U,0U);
    syscfg_exti_line_config(EXTI_SOURCE_GPIOG,EXTI_SOURCE_PIN6);
	/* ��ʼ���ж��� */
	exti_init(EXTI_6,EXTI_INTERRUPT,EXTI_TRIG_BOTH);
	/* ʹ���ж� */
	exti_interrupt_enable(EXTI_6);
	/* ����жϱ�־λ */
	exti_interrupt_flag_clear(EXTI_6);
    
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_SYSCFG);
    
    gpio_mode_set(GPIOD,GPIO_MODE_INPUT,GPIO_PUPD_NONE,GPIO_PIN_3);
    nvic_irq_enable(EXTI3_IRQn,0U,0U);
    syscfg_exti_line_config(EXTI_SOURCE_GPIOD,EXTI_SOURCE_PIN3);
	/* ��ʼ���ж��� */
	exti_init(EXTI_3,EXTI_INTERRUPT,EXTI_TRIG_BOTH);
	/* ʹ���ж� */
	exti_interrupt_enable(EXTI_3);
	/* ����жϱ�־λ */
	exti_interrupt_flag_clear(EXTI_3);
    
    
}
void buck_boost_init()
{
    pwm_config();
    adc_config();

    pid_func.reset(&gPID_VoltOutLoop);
    gPID_VoltOutLoop.T       = 0.50f;//PID�������ڣ���λ100us
    gPID_VoltOutLoop.Kp      = 5.0f;      
    gPID_VoltOutLoop.Ti      = 0.46f;
    gPID_VoltOutLoop.Td      = 0.01f;
    gPID_VoltOutLoop.Ek_Dead = 0.01f;
    gPID_VoltOutLoop.OutMin  = 0.015f * DP_PWM_PER;//��Сռ�ձ�
    gPID_VoltOutLoop.OutMax  = 1.80f * DP_PWM_PER;//���ռ�ձ�
    pid_func.init(&gPID_VoltOutLoop); 

	lowfilter_vout_buckboost.Fc  = 5e3; //��ֹƵ��Ϊ2KHZ
    lowfilter_vout_buckboost.Fs  = 12.5e3;//����Ƶ��Ϊ25KHZ
    low_filter_init(&lowfilter_vout_buckboost);     

    
}
void pfc_init()
{
    timer3_pwm_config();
    timer8_int_init();
    zcd_init();
    

    pid_func.reset(&gPID_PFC_I_Loop);
    gPID_PFC_I_Loop.T       = 0.50f;//PID�������ڣ���λ100us
    gPID_PFC_I_Loop.Kp      = 5.0f;      
    gPID_PFC_I_Loop.Ti      = 0.46f;
    gPID_PFC_I_Loop.Td      = 0.01f;
    gPID_PFC_I_Loop.Ek_Dead = 0.01f;
    gPID_PFC_I_Loop.OutMin  = 0.015f * 2399;//��Сռ�ձ�
    gPID_PFC_I_Loop.OutMax  = 0.9f * 2399;//���ռ�ձ�
    pid_func.init(&gPID_PFC_I_Loop);
    
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
        timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_1,0);
    }
    if(count<100)
    {
        timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_0,(uint16_t)(_sin((float)count/200*_2PI)*TIMER_CAR(TIMER2)));
    }
    if(count==100)
    {
        timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_0,0);
    
    }
    if(count >=100)
    {       
        timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_1,(uint16_t)(_sin((float)(count-100)/200*_2PI)*TIMER_CAR(TIMER2)));
    }
    count++;
    if(count==200)
    {
        count=0;
    }
    
}

void power_ctrl_pfc()
{
    float pfc_i_in,pfc_v_out,pfc_v_in;
    gPID_PFC_I_Loop.Ref = (26.0f-pfc_v_out)*pfc_v_in;
    gPID_PFC_I_Loop.Fdb = pfc_i_in;
    pid_func.calc(&gPID_PFC_I_Loop);
    pfc_active_pwm=gPID_PFC_I_Loop.Output;
    
}
void TIMER7_UP_TIMER12_IRQHandler()
{
    if(timer_flag_get(TIMER7,TIMER_FLAG_UP) == SET )
    {
        timer_flag_clear(TIMER7,TIMER_FLAG_UP);
        if(buck_boost_en==1)
        {
           power_ctrl_buck_boost();
        }
        gpio_bit_toggle(GPIOE,GPIO_PIN_4);
    }
}

void TIMER0_BRK_TIMER8_IRQHandler()
{
     if(timer_flag_get(TIMER3,TIMER_FLAG_UP) == SET )
    {
        timer_flag_clear(TIMER3,TIMER_FLAG_UP);
        power_ctrl_spwm();
        gpio_bit_toggle(GPIOE,GPIO_PIN_4);
    }
}
/*ZCD�ж�*/
void EXTI5_9_IRQHandler()
{
    if(exti_interrupt_flag_get(EXTI_6) == SET)
    {
        exti_interrupt_flag_clear(EXTI_6);
        gpio_bit_toggle(GPIOE,GPIO_PIN_4);
        zcd_pfc_handler();
    }
}

void EXTI3_IRQHandler()
{
    if(exti_interrupt_flag_get(EXTI_3) == SET)
    {
        exti_interrupt_flag_clear(EXTI_3);
        gpio_bit_toggle(GPIOE,GPIO_PIN_4);
        zcd_pfc_handler();
    }
}

void zcd_pfc_handler()
{
    if(gpio_input_bit_get(GPIOD,GPIO_PIN_3)== SET)
    {
        timer_channel_output_pulse_value_config(TIMER3,TIMER_CH_2,0);
        timer_channel_output_pulse_value_config(TIMER3,TIMER_CH_3,2399-pfc_active_pwm);
    }
    else
    {
        timer_channel_output_pulse_value_config(TIMER3,TIMER_CH_2,2399-pfc_active_pwm);
        timer_channel_output_pulse_value_config(TIMER3,TIMER_CH_3,0);
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
//  timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_0,DP_PWM_PER-buck_duty);
//  timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,DP_PWM_PER-boost_duty);
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_2,DP_PWM_PER-buck_duty);
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_3,DP_PWM_PER-boost_duty);
}
