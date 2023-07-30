#include "power_pwm.h"

void pwm_gpio_config()
{
     /* 使能时钟 */
	rcu_periph_clock_enable(RCU_GPIOB);
	/* 配置GPIO的模式 */
	gpio_mode_set(GPIOB,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11);
	/* 配置GPIO的输出 */
	gpio_output_options_set(GPIOB,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11);
	/* 配置GPIO的复用 */
	gpio_af_set(GPIOB,GPIO_AF_1,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11);	
    
}
void pwm_config()
{

    pwm_gpio_config();
    timer_parameter_struct timere_initpara;                 // 定义定时器结构体
	timer_oc_parameter_struct timer_ocintpara;					    //定时器比较输出结构体
	
	rcu_periph_clock_enable(RCU_TIMER1);
    rcu_periph_clock_enable(RCU_TIMER7);
    
	rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);		// 配置定时器时钟
	
	/* 配置定时器参数 */
	timer_deinit(TIMER1);														// 复位定时器
	timere_initpara.prescaler = 0;                   		// 时钟预分频值     
	timere_initpara.alignedmode = TIMER_COUNTER_CENTER_UP;      	// 中央对齐                
	timere_initpara.counterdirection = TIMER_COUNTER_UP;    // 向上计数    	                    
	timere_initpara.period = 1199;      									  // 周期 
	timere_initpara.clockdivision = TIMER_CKDIV_DIV1;    	  // 分频因子
	timer_init(TIMER1,&timere_initpara);							// 初始化定时器
	
    
	/* 配置输出结构体 */
	timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;   																	  // 有效电平的极性
	timer_ocintpara.outputstate = TIMER_CCX_ENABLE;																						// 配置比较输出模式状态 也就是使能PWM输出到端口
	timer_channel_output_config(TIMER1,TIMER_CH_0,&timer_ocintpara);
    timer_channel_output_config(TIMER1,TIMER_CH_1,&timer_ocintpara);
    
	/* 配置占空比 */
    /*buck ch*/
	timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_0,0);// 配置定时器通道输出脉冲值
	timer_channel_output_mode_config(TIMER1,TIMER_CH_0,TIMER_OC_MODE_PWM1);	// 配置定时器通道输出比较模式
	timer_channel_output_shadow_config(TIMER1,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);// 配置定时器通道输出影子寄存器
    /*boost ch*/
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,0);									// 配置定时器通道输出脉冲值
	timer_channel_output_mode_config(TIMER1,TIMER_CH_1,TIMER_OC_MODE_PWM0);				// 配置定时器通道输出比较模式
	timer_channel_output_shadow_config(TIMER1,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);// 配置定时器通道输出影子寄存器
    
    timer_master_slave_mode_config(TIMER1,TIMER_MASTER_SLAVE_MODE_ENABLE);
    timer_master_output_trigger_source_select(TIMER1,TIMER_TRI_OUT_SRC_UPDATE);//启动trgo输出
	
	timer_auto_reload_shadow_enable(TIMER1);
    
    /*配置从定时器TIMER7*/
    timer_struct_para_init(&timere_initpara);
    
    timer_deinit(TIMER7);														// 复位定时器
	timere_initpara.prescaler = 0;                   		// 时钟预分频值   PSC_CLK= 200MHZ / 200 = 1MHZ       
	timere_initpara.alignedmode = TIMER_COUNTER_EDGE;      	// 边缘对齐                
	timere_initpara.counterdirection = TIMER_COUNTER_UP;    // 向上计数    	                    
	timere_initpara.period = 15;      									  // 周期    T = 10000 * 1MHZ = 10ms  f = 100HZ   
	timere_initpara.clockdivision = TIMER_CKDIV_DIV1;    	  // 分频因子
	timere_initpara.repetitioncounter = 0;                  // 重复计数器 0-255  
	timer_init(TIMER7,&timere_initpara);							// 初始化定时器
    
    timer_slave_mode_select(TIMER7,TIMER_SLAVE_MODE_EXTERNAL0);
    timer_input_trigger_source_select(TIMER7,TIMER_SMCFG_TRGSEL_ITI1);
    
    timer_master_slave_mode_config(TIMER7,TIMER_MASTER_SLAVE_MODE_ENABLE);
    timer_master_output_trigger_source_select(TIMER7,TIMER_TRI_OUT_SRC_UPDATE);//启动trgo输出
    
    /*中断*/
    nvic_irq_enable(TIMER7_UP_TIMER12_IRQn, 0, 0);
    timer_interrupt_enable(TIMER7,TIMER_INT_UP);
    
	 /* 使能定时器 */
	timer_enable(TIMER1);
    timer_enable(TIMER7);
}
void pwm_config2()
{
    rcu_periph_clock_enable(RCU_TIMER7);
    
    timer_parameter_struct timere_initpara;                 // 定义定时器结构体
	timer_oc_parameter_struct timer_ocintpara;					    //定时器比较输出结构体
    /* 使能时钟 */
	rcu_periph_clock_enable(RCU_GPIOC);
	/* 配置GPIO的模式 */
	gpio_mode_set(GPIOC,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_6|GPIO_PIN_7);
	/* 配置GPIO的输出 */
	gpio_output_options_set(GPIOC,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_6|GPIO_PIN_7);
	/* 配置GPIO的复用 */
	gpio_af_set(GPIOC,GPIO_AF_3,GPIO_PIN_6|GPIO_PIN_7);
	
    timer_struct_para_init(&timere_initpara);
    
    timer_deinit(TIMER7);														// 复位定时器
	timere_initpara.prescaler = 0;                   		// 时钟预分频值   PSC_CLK= 200MHZ / 200 = 1MHZ       
	timere_initpara.alignedmode = TIMER_COUNTER_EDGE;      	// 边缘对齐                
	timere_initpara.counterdirection = TIMER_COUNTER_UP;    // 向上计数    	                    
	timere_initpara.period = 11999;      									  // 周期    T = 10000 * 1MHZ = 10ms  f = 100HZ   
	timere_initpara.clockdivision = TIMER_CKDIV_DIV1;    	  // 分频因子
	timere_initpara.repetitioncounter = 1;                  // 重复计数器 0-255  
	timer_init(TIMER7,&timere_initpara);							// 初始化定时器
    
    timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;   																	  // 有效电平的极性
	timer_ocintpara.outputstate = TIMER_CCX_ENABLE;																						// 配置比较输出模式状态 也就是使能PWM输出到端口
	timer_channel_output_config(TIMER7,TIMER_CH_0,&timer_ocintpara);
    timer_channel_output_config(TIMER7,TIMER_CH_1,&timer_ocintpara);
    
    timer_channel_output_pulse_value_config(TIMER7,TIMER_CH_0,0);// 配置定时器通道输出脉冲值
	timer_channel_output_mode_config(TIMER7,TIMER_CH_0,TIMER_OC_MODE_PWM1);	// 配置定时器通道输出比较模式
	timer_channel_output_shadow_config(TIMER7,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);// 配置定时器通道输出影子寄存器

    timer_channel_output_pulse_value_config(TIMER7,TIMER_CH_1,0);									// 配置定时器通道输出脉冲值
	timer_channel_output_mode_config(TIMER7,TIMER_CH_1,TIMER_OC_MODE_PWM1);				// 配置定时器通道输出比较模式
	timer_channel_output_shadow_config(TIMER7,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);// 配置定时器通道输出影子寄存器
    
    timer_primary_output_config(TIMER7,ENABLE);
	timer_auto_reload_shadow_enable(TIMER7);
    /*中断*/
    nvic_irq_enable(TIMER7_UP_TIMER12_IRQn, 0, 0);
    timer_interrupt_enable(TIMER7,TIMER_INT_UP);
    
    timer_enable(TIMER7);
    
}

void timer2_pwm_config()
{
    rcu_periph_clock_enable(RCU_GPIOC);
	/* 配置GPIO的模式 */
	gpio_mode_set(GPIOC,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_6|GPIO_PIN_7);
	/* 配置GPIO的输出 */
	gpio_output_options_set(GPIOC,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_6|GPIO_PIN_7);
	/* 配置GPIO的复用 */
	gpio_af_set(GPIOC,GPIO_AF_2,GPIO_PIN_6|GPIO_PIN_7);
    
    rcu_periph_clock_enable(RCU_TIMER2);
    
    timer_parameter_struct timere_initpara;                 // 定义定时器结构体
	timer_oc_parameter_struct timer_ocintpara;					    //定时器比较输出结构体
    
    timer_deinit(TIMER2);														// 复位定时器
	timere_initpara.prescaler = 0;                   		// 时钟预分频值   PSC_CLK= 200MHZ / 200 = 1MHZ       
	timere_initpara.alignedmode = TIMER_COUNTER_EDGE;      	// 边缘对齐                
	timere_initpara.counterdirection = TIMER_COUNTER_UP;    // 向上计数    	                    
	timere_initpara.period = 5999;      									  // 周期    T = 10000 * 1MHZ = 10ms  f = 100HZ   
	timere_initpara.clockdivision = TIMER_CKDIV_DIV1;    	  // 分频因子
	//timere_initpara.repetitioncounter = 0;                  // 重复计数器 0-255  
	timer_init(TIMER2,&timere_initpara);							// 初始化定时器
    
    timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;   																	  // 有效电平的极性
	timer_ocintpara.outputstate = TIMER_CCX_ENABLE;																						// 配置比较输出模式状态 也就是使能PWM输出到端口
	timer_channel_output_config(TIMER2,TIMER_CH_0,&timer_ocintpara);
    timer_channel_output_config(TIMER2,TIMER_CH_1,&timer_ocintpara);
    
    timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_0,0);// 配置定时器通道输出脉冲值
	timer_channel_output_mode_config(TIMER2,TIMER_CH_0,TIMER_OC_MODE_PWM1);	// 配置定时器通道输出比较模式
	timer_channel_output_shadow_config(TIMER2,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);// 配置定时器通道输出影子寄存器

    timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_1,0);									// 配置定时器通道输出脉冲值
	timer_channel_output_mode_config(TIMER2,TIMER_CH_1,TIMER_OC_MODE_PWM1);				// 配置定时器通道输出比较模式
	timer_channel_output_shadow_config(TIMER2,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);// 配置定时器通道输出影子寄存器
    
    timer_auto_reload_shadow_enable(TIMER2);
    
    //timer_primary_output_config(TIMER7,ENABLE);
    
    //nvic_irq_enable(TIMER7_UP_TIMER12_IRQn, 0, 0);
    //timer_interrupt_enable(TIMER7,TIMER_INT_UP);
    
    timer_enable(TIMER2);
    
}