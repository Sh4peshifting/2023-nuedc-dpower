#include "power_pwm.h"

void pwm_gpio_config()
{
     /* ʹ��ʱ�� */
	rcu_periph_clock_enable(RCU_GPIOB);
	/* ����GPIO��ģʽ */
	gpio_mode_set(GPIOB,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11);
	/* ����GPIO����� */
	gpio_output_options_set(GPIOB,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11);
	/* ����GPIO�ĸ��� */
	gpio_af_set(GPIOB,GPIO_AF_1,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11);	
    
}
void pwm_config()
{

    pwm_gpio_config();
    timer_parameter_struct timere_initpara;                 // ���嶨ʱ���ṹ��
	timer_oc_parameter_struct timer_ocintpara;					    //��ʱ���Ƚ�����ṹ��
	
	rcu_periph_clock_enable(RCU_TIMER1);
    rcu_periph_clock_enable(RCU_TIMER7);
    
	rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);		// ���ö�ʱ��ʱ��
	
	/* ���ö�ʱ������ */
	timer_deinit(TIMER1);														// ��λ��ʱ��
	timere_initpara.prescaler = 0;                   		// ʱ��Ԥ��Ƶֵ     
	timere_initpara.alignedmode = TIMER_COUNTER_CENTER_UP;      	// �������                
	timere_initpara.counterdirection = TIMER_COUNTER_UP;    // ���ϼ���    	                    
	timere_initpara.period = 1199;      									  // ���� 
	timere_initpara.clockdivision = TIMER_CKDIV_DIV1;    	  // ��Ƶ����
	timer_init(TIMER1,&timere_initpara);							// ��ʼ����ʱ��
	
    
	/* ��������ṹ�� */
	timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;   																	  // ��Ч��ƽ�ļ���
	timer_ocintpara.outputstate = TIMER_CCX_ENABLE;																						// ���ñȽ����ģʽ״̬ Ҳ����ʹ��PWM������˿�
	timer_channel_output_config(TIMER1,TIMER_CH_0,&timer_ocintpara);
    timer_channel_output_config(TIMER1,TIMER_CH_1,&timer_ocintpara);
    
	/* ����ռ�ձ� */
    /*buck ch*/
	timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_0,0);// ���ö�ʱ��ͨ���������ֵ
	timer_channel_output_mode_config(TIMER1,TIMER_CH_0,TIMER_OC_MODE_PWM1);	// ���ö�ʱ��ͨ������Ƚ�ģʽ
	timer_channel_output_shadow_config(TIMER1,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);// ���ö�ʱ��ͨ�����Ӱ�ӼĴ���
    /*boost ch*/
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,0);									// ���ö�ʱ��ͨ���������ֵ
	timer_channel_output_mode_config(TIMER1,TIMER_CH_1,TIMER_OC_MODE_PWM0);				// ���ö�ʱ��ͨ������Ƚ�ģʽ
	timer_channel_output_shadow_config(TIMER1,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);// ���ö�ʱ��ͨ�����Ӱ�ӼĴ���
    
    timer_master_slave_mode_config(TIMER1,TIMER_MASTER_SLAVE_MODE_ENABLE);
    timer_master_output_trigger_source_select(TIMER1,TIMER_TRI_OUT_SRC_UPDATE);//����trgo���
	
	timer_auto_reload_shadow_enable(TIMER1);
    
    /*���ôӶ�ʱ��TIMER7*/
    timer_struct_para_init(&timere_initpara);
    
    timer_deinit(TIMER7);														// ��λ��ʱ��
	timere_initpara.prescaler = 0;                   		// ʱ��Ԥ��Ƶֵ   PSC_CLK= 200MHZ / 200 = 1MHZ       
	timere_initpara.alignedmode = TIMER_COUNTER_EDGE;      	// ��Ե����                
	timere_initpara.counterdirection = TIMER_COUNTER_UP;    // ���ϼ���    	                    
	timere_initpara.period = 15;      									  // ����    T = 10000 * 1MHZ = 10ms  f = 100HZ   
	timere_initpara.clockdivision = TIMER_CKDIV_DIV1;    	  // ��Ƶ����
	timere_initpara.repetitioncounter = 0;                  // �ظ������� 0-255  
	timer_init(TIMER7,&timere_initpara);							// ��ʼ����ʱ��
    
    timer_slave_mode_select(TIMER7,TIMER_SLAVE_MODE_EXTERNAL0);
    timer_input_trigger_source_select(TIMER7,TIMER_SMCFG_TRGSEL_ITI1);
    
    timer_master_slave_mode_config(TIMER7,TIMER_MASTER_SLAVE_MODE_ENABLE);
    timer_master_output_trigger_source_select(TIMER7,TIMER_TRI_OUT_SRC_UPDATE);//����trgo���
    
    /*�ж�*/
    nvic_irq_enable(TIMER7_UP_TIMER12_IRQn, 0, 0);
    timer_interrupt_enable(TIMER7,TIMER_INT_UP);
    
	 /* ʹ�ܶ�ʱ�� */
	timer_enable(TIMER1);
    timer_enable(TIMER7);
}
void pwm_config2()
{
    rcu_periph_clock_enable(RCU_TIMER7);
    
    timer_parameter_struct timere_initpara;                 // ���嶨ʱ���ṹ��
	timer_oc_parameter_struct timer_ocintpara;					    //��ʱ���Ƚ�����ṹ��
    /* ʹ��ʱ�� */
	rcu_periph_clock_enable(RCU_GPIOC);
	/* ����GPIO��ģʽ */
	gpio_mode_set(GPIOC,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_6|GPIO_PIN_7);
	/* ����GPIO����� */
	gpio_output_options_set(GPIOC,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_6|GPIO_PIN_7);
	/* ����GPIO�ĸ��� */
	gpio_af_set(GPIOC,GPIO_AF_3,GPIO_PIN_6|GPIO_PIN_7);
	
    timer_struct_para_init(&timere_initpara);
    
    timer_deinit(TIMER7);														// ��λ��ʱ��
	timere_initpara.prescaler = 0;                   		// ʱ��Ԥ��Ƶֵ   PSC_CLK= 200MHZ / 200 = 1MHZ       
	timere_initpara.alignedmode = TIMER_COUNTER_EDGE;      	// ��Ե����                
	timere_initpara.counterdirection = TIMER_COUNTER_UP;    // ���ϼ���    	                    
	timere_initpara.period = 11999;      									  // ����    T = 10000 * 1MHZ = 10ms  f = 100HZ   
	timere_initpara.clockdivision = TIMER_CKDIV_DIV1;    	  // ��Ƶ����
	timere_initpara.repetitioncounter = 1;                  // �ظ������� 0-255  
	timer_init(TIMER7,&timere_initpara);							// ��ʼ����ʱ��
    
    timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;   																	  // ��Ч��ƽ�ļ���
	timer_ocintpara.outputstate = TIMER_CCX_ENABLE;																						// ���ñȽ����ģʽ״̬ Ҳ����ʹ��PWM������˿�
	timer_channel_output_config(TIMER7,TIMER_CH_0,&timer_ocintpara);
    timer_channel_output_config(TIMER7,TIMER_CH_1,&timer_ocintpara);
    
    timer_channel_output_pulse_value_config(TIMER7,TIMER_CH_0,0);// ���ö�ʱ��ͨ���������ֵ
	timer_channel_output_mode_config(TIMER7,TIMER_CH_0,TIMER_OC_MODE_PWM1);	// ���ö�ʱ��ͨ������Ƚ�ģʽ
	timer_channel_output_shadow_config(TIMER7,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);// ���ö�ʱ��ͨ�����Ӱ�ӼĴ���

    timer_channel_output_pulse_value_config(TIMER7,TIMER_CH_1,0);									// ���ö�ʱ��ͨ���������ֵ
	timer_channel_output_mode_config(TIMER7,TIMER_CH_1,TIMER_OC_MODE_PWM1);				// ���ö�ʱ��ͨ������Ƚ�ģʽ
	timer_channel_output_shadow_config(TIMER7,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);// ���ö�ʱ��ͨ�����Ӱ�ӼĴ���
    
    timer_primary_output_config(TIMER7,ENABLE);
	timer_auto_reload_shadow_enable(TIMER7);
    /*�ж�*/
    nvic_irq_enable(TIMER7_UP_TIMER12_IRQn, 0, 0);
    timer_interrupt_enable(TIMER7,TIMER_INT_UP);
    
    timer_enable(TIMER7);
    
}

void timer2_pwm_config()
{
    rcu_periph_clock_enable(RCU_GPIOC);
	/* ����GPIO��ģʽ */
	gpio_mode_set(GPIOC,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_6|GPIO_PIN_7);
	/* ����GPIO����� */
	gpio_output_options_set(GPIOC,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_6|GPIO_PIN_7);
	/* ����GPIO�ĸ��� */
	gpio_af_set(GPIOC,GPIO_AF_2,GPIO_PIN_6|GPIO_PIN_7);
    
    rcu_periph_clock_enable(RCU_TIMER2);
    
    timer_parameter_struct timere_initpara;                 // ���嶨ʱ���ṹ��
	timer_oc_parameter_struct timer_ocintpara;					    //��ʱ���Ƚ�����ṹ��
    
    timer_deinit(TIMER2);														// ��λ��ʱ��
	timere_initpara.prescaler = 0;                   		// ʱ��Ԥ��Ƶֵ   PSC_CLK= 200MHZ / 200 = 1MHZ       
	timere_initpara.alignedmode = TIMER_COUNTER_EDGE;      	// ��Ե����                
	timere_initpara.counterdirection = TIMER_COUNTER_UP;    // ���ϼ���    	                    
	timere_initpara.period = 5999;      									  // ����    T = 10000 * 1MHZ = 10ms  f = 100HZ   
	timere_initpara.clockdivision = TIMER_CKDIV_DIV1;    	  // ��Ƶ����
	//timere_initpara.repetitioncounter = 0;                  // �ظ������� 0-255  
	timer_init(TIMER2,&timere_initpara);							// ��ʼ����ʱ��
    
    timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;   																	  // ��Ч��ƽ�ļ���
	timer_ocintpara.outputstate = TIMER_CCX_ENABLE;																						// ���ñȽ����ģʽ״̬ Ҳ����ʹ��PWM������˿�
	timer_channel_output_config(TIMER2,TIMER_CH_0,&timer_ocintpara);
    timer_channel_output_config(TIMER2,TIMER_CH_1,&timer_ocintpara);
    
    timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_0,0);// ���ö�ʱ��ͨ���������ֵ
	timer_channel_output_mode_config(TIMER2,TIMER_CH_0,TIMER_OC_MODE_PWM1);	// ���ö�ʱ��ͨ������Ƚ�ģʽ
	timer_channel_output_shadow_config(TIMER2,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);// ���ö�ʱ��ͨ�����Ӱ�ӼĴ���

    timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_1,0);									// ���ö�ʱ��ͨ���������ֵ
	timer_channel_output_mode_config(TIMER2,TIMER_CH_1,TIMER_OC_MODE_PWM1);				// ���ö�ʱ��ͨ������Ƚ�ģʽ
	timer_channel_output_shadow_config(TIMER2,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);// ���ö�ʱ��ͨ�����Ӱ�ӼĴ���
    
    timer_auto_reload_shadow_enable(TIMER2);
    
    //timer_primary_output_config(TIMER7,ENABLE);
    
    //nvic_irq_enable(TIMER7_UP_TIMER12_IRQn, 0, 0);
    //timer_interrupt_enable(TIMER7,TIMER_INT_UP);
    
    timer_enable(TIMER2);
    
}