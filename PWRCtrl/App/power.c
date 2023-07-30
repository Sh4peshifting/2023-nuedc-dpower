#include "power.h"

uint8_t buck_boost_en=0;
PID_STRUCT gPID_VoltOutLoop; //输出电压环PID数据
/*定义校正参数                  x1*****y1*****x2*****y2*****y******a*********************b   */
ELEC_INFO_STRUCT v_in1_struct = {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, DP_VOLTAGE_OUT_RATIO, 0.00f}; //输出电压参数  
ELEC_INFO_STRUCT i_in1_struct = {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, DP_VOLTAGE_OUT_RATIO, 0.00f}; //输出电压参数 



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

void eg2104_sd_init()
{
    rcu_periph_clock_enable(RCU_GPIOE);
    gpio_mode_set(GPIOE,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLDOWN,GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6);
    gpio_output_options_set(GPIOE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6);
    
}

void buck_boost_init()
{
    pid_func.reset(&gPID_VoltOutLoop);
    gPID_VoltOutLoop.T       = 0.50f;//PID控制周期，单位100us
    gPID_VoltOutLoop.Kp      = 10.0f;      
    gPID_VoltOutLoop.Ti      = 0.35f;
    gPID_VoltOutLoop.Td      = 0.02f;
    gPID_VoltOutLoop.Ek_Dead = 0.01f;
    gPID_VoltOutLoop.OutMin  = 0.03f * DP_PWM_PER;//最小占空比
    gPID_VoltOutLoop.OutMax  = 1.70f * DP_PWM_PER;//最大占空比
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
/*以 10KHz调用这个函数，以调制单极性SPWM*/
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
   * 升降压的PWM方案：
   * 1.由于MOS采用自举电容驱动方式，故上管不能100%占空比导通，这里我们设为90%。
   * 2.本函数的输入参数为总的占空比，需要分别算出两个半桥的占空比。
   * 3.降压工作时，BOOST的半桥的占空比是固定的为90%，通过改变BUCK半桥占空比实现稳压。
   * 3.升压或等压工作时，BUCK半桥的占空比是固定的为90%，通过改变BOOST半桥占空比实现稳压。
   */  

  if( pwm_cmp_value >= MAX_PWM_CMP)
  {
    boost_duty  = pwm_cmp_value - MAX_PWM_CMP + MIN_PWM_CMP;//BOOST半桥占空比计算
  }
  else
  {
    boost_duty  = MIN_PWM_CMP;  //BUCK模式,boost_duty给固定占空比
  }

  if( pwm_cmp_value >= MAX_PWM_CMP)
  {
    buck_duty  = MAX_PWM_CMP;   //BOOST模式,buck_duty给固定占空比
  }
  else
  {
    buck_duty  = pwm_cmp_value;//BUCK半桥占空比设定
  }

  /*
   *  占空比限制
   */
  if( boost_duty > MAX_PWM_CMP )boost_duty = MAX_PWM_CMP;
  if( boost_duty < MIN_PWM_CMP )boost_duty = MIN_PWM_CMP;
  if( buck_duty  > MAX_PWM_CMP )buck_duty  = MAX_PWM_CMP;
  if( buck_duty  < MIN_PWM_CMP )buck_duty  = MIN_PWM_CMP;
	
	/*
   *  更新比较寄存器
   */
//  HRTIM1->sMasterRegs.MCMP1R  = ( DP_PWM_PER - buck_duty  ) >> 1;
//  HRTIM1->sMasterRegs.MCMP2R  = ( DP_PWM_PER + buck_duty  ) >> 1;
//  HRTIM1->sMasterRegs.MCMP3R  = ( DP_PWM_PER + boost_duty ) >> 1;
//  HRTIM1->sMasterRegs.MCMP4R  = ( DP_PWM_PER - boost_duty ) >> 1;
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_0,DP_PWM_PER-buck_duty);
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,DP_PWM_PER-boost_duty);
}
