#include "power.h"

uint8_t inverter_en=0,aci_loop_en=0,acv_loop_en=0,acv_openloop_en=0;
uint16_t pfc_active_pwm=00;
uint16_t cnt_spwm=0;
float i_acout=0.4f,vp_inverter=15,v_inverter_out=7,ip_inverter,i_spwm_ratio=1,v_gird;


PID_STRUCT gPID_CurrentOutLoop; //输出电压环PID数据
PID_STRUCT gPID_ACVOutLoop; //输出电压环PID数据

LOW_FILTER_STRUCT  lpf_v_in1;
LOW_FILTER_STRUCT  lpf_v_in2;
LOW_FILTER_STRUCT  lpf_v_in3;
LOW_FILTER_STRUCT  lpf_v_in4;
LOW_FILTER_STRUCT  lpf_i_in1;
LOW_FILTER_STRUCT  lpf_i_in2;
LOW_FILTER_STRUCT  lpf_i_in3;
/*定义校正参数                    x1*****y1*****x2*****y2*****y******a*********************b   */
ELEC_INFO_STRUCT v_in1 =   {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, DC_VOLTAGE_RETIO, 0.00f}; //输出电压参数  
ELEC_INFO_STRUCT v_in2 =   {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, DC_VOLTAGE_RETIO, 0.00f}; //输出电压参数 
ELEC_INFO_STRUCT v_in3 =   {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, DC_VOLTAGE_RETIO, 0.00f}; //输出电压参数  
ELEC_INFO_STRUCT v_in4 =   {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, AC_VOLTAGE_RETIO, 0.00f}; //输出电压参数 
ELEC_INFO_STRUCT i_in1 =   {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, CURRENT_RATIO, 0.00f}; //输出电压参数
ELEC_INFO_STRUCT i_in2 =   {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, CURRENT_RATIO, 0.00f}; //输出电压参数
ELEC_INFO_STRUCT i_in3 =   {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, CURRENT_RATIO, 0.00f}; //输出电压参数
ELEC_INFO_STRUCT acv_in1 = {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, DC_VOLTAGE_RETIO, 0.00f}; //输出电压参数 


AC_Para ac1info;

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

void relay_gpio_init()
{
    rcu_periph_clock_enable(RCU_GPIOC);
    gpio_mode_set(GPIOC,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLDOWN,GPIO_PIN_10|GPIO_PIN_11);
    gpio_output_options_set(GPIOC,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_10|GPIO_PIN_11);
}
void relay_cmd(uint8_t ch,bit_status bit)
{
    uint32_t pin;
    if(ch==0) pin=GPIO_PIN_10;
    else pin=GPIO_PIN_11;
    
    gpio_bit_write(GPIOC,pin,bit);
}

void zcd_init()
{
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_SYSCFG);
    
    gpio_mode_set(GPIOD,GPIO_MODE_INPUT,GPIO_PUPD_NONE,GPIO_PIN_3);
    nvic_irq_enable(EXTI3_IRQn,0U,0U);
    syscfg_exti_line_config(EXTI_SOURCE_GPIOD,EXTI_SOURCE_PIN3);
	/* 初始化中断线 */
	exti_init(EXTI_3,EXTI_INTERRUPT,EXTI_TRIG_RISING);//仅上升沿检测attention!!!!!!!!!
	/* 使能中断 */
	exti_interrupt_enable(EXTI_3);
	/* 清除中断标志位 */
	exti_interrupt_flag_clear(EXTI_3);
    
    
}
void buck_boost_init()
{
    pwm_config();
    adc_config();

  	pid_func.reset(&gPID_CurrentOutLoop);
    gPID_CurrentOutLoop.T       = 0.40f;//PID控制周期，单位100us
    gPID_CurrentOutLoop.Kp      = 15.00f;
    gPID_CurrentOutLoop.Ti      = 0.3f;
    gPID_CurrentOutLoop.Td      = 0.01f;
    gPID_CurrentOutLoop.Ek_Dead = 0.01f;
    gPID_CurrentOutLoop.OutMin  = -0.80f * DP_PWM_PER;//最小
    gPID_CurrentOutLoop.OutMax  = 0.80f * DP_PWM_PER;//最大
    pid_func.init(&gPID_CurrentOutLoop);
    
    
    pid_func.reset(&gPID_ACVOutLoop);
    gPID_ACVOutLoop.T       = 50000.0f;//PID控制周期，单位100us
    gPID_ACVOutLoop.Kp      = 0.00001f;      
    gPID_ACVOutLoop.Ti      = 2.0f;
    gPID_ACVOutLoop.Td      = 0.01f;
    gPID_ACVOutLoop.Ek_Dead = 0.01f;
    gPID_ACVOutLoop.OutMin  = 10.0f;//最小电压
    gPID_ACVOutLoop.OutMax  = 24.0f;//最大电压
    pid_func.init(&gPID_ACVOutLoop); 
}



void power_ctrl_ACcurrent()
{
    gPID_CurrentOutLoop.Ref=i_acout*_SQRT2*SIN_TIME;
    gPID_CurrentOutLoop.Ref=i_in3.Value;
    pid_func.calc(&gPID_CurrentOutLoop);
    set_spwm(gPID_CurrentOutLoop.Output);
    
}
void power_ctrl_ACvoltage50Hz()
{
    gPID_ACVOutLoop.Ref=v_inverter_out;
    gPID_ACVOutLoop.Fdb=vp_inverter/_SQRT2;
    pid_func.calc(&gPID_ACVOutLoop);
    
}
void power_ctrl_ACvoltage20K()
{
    set_spwm(SIN_TIME*gPID_ACVOutLoop.Output);
}
void power_ctrl_ac_openloop()
{
    set_spwm(SIN_TIME*TIMER_CAR(TIMER1)*0.8f);
}

void TIMER7_UP_TIMER12_IRQHandler()//20K
{
    if(timer_flag_get(TIMER7,TIMER_FLAG_UP) == SET )
    {
        timer_flag_clear(TIMER7,TIMER_FLAG_UP);
        adc_value_process();    
        cnt_spwm++;
        if(cnt_spwm==400) cnt_spwm=0;
        if(aci_loop_en==1) power_ctrl_ACcurrent();
        if(acv_openloop_en==1) power_ctrl_ac_openloop();
        if(acv_loop_en==1) power_ctrl_ACvoltage20K();
    }
}

void TIMER0_BRK_TIMER8_IRQHandler()//20K
{
    if(timer_flag_get(TIMER8,TIMER_FLAG_UP) == SET )
    {
        timer_flag_clear(TIMER8,TIMER_FLAG_UP);        
    }
}
void TIMER7_BRK_TIMER11_IRQHandler()//50Hz
{
    if(timer_flag_get(TIMER11,TIMER_FLAG_UP) == SET )
    {
        timer_flag_clear(TIMER11,TIMER_FLAG_UP);
        if(acv_loop_en==1) power_ctrl_ACvoltage50Hz();
    }
}

/*ZCD中断*/
void EXTI3_IRQHandler()
{
    if(exti_interrupt_flag_get(EXTI_3) == SET)
    {
        exti_interrupt_flag_clear(EXTI_3);
        cnt_spwm=0;
    }
}


void set_spwm(float pwm)//通道2对应输出为正极输出
{
    if(pwm>0)
    {
        timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_2,(uint16_t)pwm);
        timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_3,0);
    }
    else
    {
        pwm=-pwm;
        timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_2,0);
        timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_3,(uint16_t)pwm);   
    }
}

void adc_value_process()
{
    lpf_v_in1.Input=(adc0_value[0]*v_in1.Coeff)+v_in1.Offset;
    lpf_v_in2.Input=(adc0_value[1]*v_in2.Coeff)+v_in2.Offset;
    lpf_v_in3.Input=(adc0_value[2]*v_in3.Coeff)+v_in3.Offset;
    lpf_v_in4.Input=(adc0_value[3]-(float)2048)*v_in4.Coeff+v_in4.Offset;
    lpf_i_in1.Input=(adc0_value[4]-(float)2048)*i_in1.Coeff+i_in1.Offset;
    lpf_i_in2.Input=(adc0_value[5]-(float)2048)*i_in2.Coeff+i_in2.Offset;
    lpf_i_in3.Input=(adc0_value[6]-(float)2048)*i_in3.Coeff+i_in3.Offset;
    
    low_filter_calc(&lpf_v_in1);
    low_filter_calc(&lpf_v_in2);
    low_filter_calc(&lpf_v_in3);
    low_filter_calc(&lpf_v_in4);
    low_filter_calc(&lpf_i_in1);
    low_filter_calc(&lpf_i_in2);
    low_filter_calc(&lpf_i_in3);
    
    v_in1.Value=lpf_v_in1.Output;
    v_in2.Value=lpf_v_in2.Output;
    v_in3.Value=lpf_v_in3.Output;
    v_in4.Value=lpf_v_in4.Output;
    i_in1.Value=lpf_i_in1.Output;
    i_in2.Value=lpf_i_in2.Output;
    i_in3.Value=lpf_i_in3.Output;
    
}

void power_info_init()
{
    lpf_v_in1.Fc  = 5e3; //截止频率为5KHZ
    lpf_v_in1.Fs  = 12.5e3;//采样频率为1.25KHZ
    low_filter_init(&lpf_v_in1);   
    
    lpf_v_in2.Fc  = 5e3; //截止频率为2KHZ
    lpf_v_in2.Fs  = 12.5e3;//采样频率为25KHZ
    low_filter_init(&lpf_v_in2);
    
    lpf_v_in3.Fc  = 5e3; //截止频率为5KHZ
    lpf_v_in3.Fs  = 12.5e3;//采样频率为1.25KHZ
    low_filter_init(&lpf_v_in3);   
    
    lpf_v_in4.Fc  = 5e3; //截止频率为2KHZ
    lpf_v_in4.Fs  = 12.5e3;//采样频率为25KHZ
    low_filter_init(&lpf_v_in4);
    
    lpf_i_in1.Fc  = 2e3; //截止频率为2KHZ
    lpf_i_in1.Fs  = 12.5e3;//采样频率为25KHZ
    low_filter_init(&lpf_i_in1);
    
    lpf_i_in2.Fc  = 5e3; //截止频率为2KHZ
    lpf_i_in2.Fs  = 12.5e3;//采样频率为25KHZ
    low_filter_init(&lpf_i_in2);
    
    lpf_i_in3.Fc  = 5e3; //截止频率为2KHZ
    lpf_i_in3.Fs  = 12.5e3;//采样频率为25KHZ
    low_filter_init(&lpf_i_in3);
    
}
