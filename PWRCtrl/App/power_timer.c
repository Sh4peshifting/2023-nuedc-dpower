#include "power_timer.h"

void timer8_int_init()
{
    rcu_periph_clock_enable(RCU_TIMER3);
    
    timer_parameter_struct timere_initpara; 
    
    timer_deinit(TIMER8);														// ��λ��ʱ��
	timere_initpara.prescaler = 0;                   		// ʱ��Ԥ��Ƶֵ   PSC_CLK= 200MHZ / 200 = 1MHZ       
	timere_initpara.alignedmode = TIMER_COUNTER_EDGE;      	// ��Ե����                
	timere_initpara.counterdirection = TIMER_COUNTER_UP;    // ���ϼ���    	                    
	timere_initpara.period = 23999;      									  // ����    T = 10000 * 1MHZ = 10ms  f = 100HZ   
	timere_initpara.clockdivision = TIMER_CKDIV_DIV1;    	  // ��Ƶ����
	//timere_initpara.repetitioncounter = 0;                  // �ظ������� 0-255  
	timer_init(TIMER8,&timere_initpara);							// ��ʼ����ʱ��
    
    nvic_irq_enable(TIMER3_IRQn, 0, 1);
    timer_interrupt_enable(TIMER8,TIMER_INT_UP);
    
    timer_enable(TIMER8);
}
