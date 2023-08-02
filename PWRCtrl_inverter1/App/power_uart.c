#include "power_uart.h"

uint8_t 	uart0_buff[20],uart3_buff[26];// ���ջ�����
uint8_t 	uart0_buff_len = 0,uart3_buff_len = 0;// �������ݳ���
uint8_t		uart0_complete_flag = 0,uart3_complete_flag = 0; // ����������ɱ�־λ

void usart0_gpio_config(uint32_t band_rate)
{
	/* ����ʱ�� */
	rcu_periph_clock_enable(RCU_USART0);
	rcu_periph_clock_enable(RCU_GPIOB);
	/* ����GPIO���ù��� */
    gpio_af_set(GPIOB,GPIO_AF_7,GPIO_PIN_6);
    gpio_af_set(GPIOB,GPIO_AF_7,GPIO_PIN_7);	
	
	/* ����GPIO��ģʽ */
	/* ����TX RXΪ����ģʽ ����ģʽ */
	gpio_mode_set(GPIOB,GPIO_MODE_AF,GPIO_PUPD_PULLUP,GPIO_PIN_6);
	gpio_mode_set(GPIOB,GPIO_MODE_AF,GPIO_PUPD_PULLUP,GPIO_PIN_7);
	
	/* ����TXΪ������� 50MHZ */
	gpio_output_options_set(GPIOB,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_6);
	gpio_output_options_set(GPIOB,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_7);

	/* ���ô��ڵĲ��� */
	usart_deinit(USART0);                                 // ��λ����
	usart_baudrate_set(USART0,band_rate);                 // ���ò�����
	usart_parity_config(USART0,USART_PM_NONE);            // û��У��λ
	usart_word_length_set(USART0,USART_WL_8BIT);          // 8λ����λ
	usart_stop_bit_set(USART0,USART_STB_1BIT);     			 // 1λֹͣλ

  /* ʹ�ܴ��� */
	usart_enable(USART0);                                 // ʹ�ܴ���
	usart_transmit_config(USART0,USART_TRANSMIT_ENABLE);  // ʹ�ܴ��ڷ���
	usart_receive_config(USART0,USART_RECEIVE_ENABLE);    // ʹ�ܴ��ڽ���
	
	/* �ж����� */
	nvic_irq_enable(USART0_IRQn, 2, 2); 									 // �����ж����ȼ�
    usart_interrupt_enable(USART0,USART_INT_RBNE);				 // �����ݻ������ǿ��жϺ���������ж�
	usart_interrupt_enable(USART0,USART_INT_IDLE);				 // ���м���ж�
}


void usart_send_data(uint8_t ucch)
{
	usart_data_transmit(USART0,(uint8_t)ucch);							 // ��������
	while(RESET == usart_flag_get(USART0,USART_FLAG_TBE));  // �ȴ��������ݻ�������־��λ
}



void usart_send_string(uint8_t *ucstr,uint8_t length)
{
	while(length--)        // ��ַΪ�ջ���ֵΪ������
	{
	  usart_send_data(*ucstr++);  // ���͵����ַ�
	}
}


int fputc(int ch, FILE *f)
{
     usart_send_data(ch);
     // �ȴ��������ݻ�������־��λ
     return ch;
}

/*�����жϷ�����*/
void USART0_IRQHandler()
{
    if(usart_interrupt_flag_get(USART0,USART_INT_FLAG_RBNE) == SET)   // ���ջ�������Ϊ��
	{
		uart0_buff[uart0_buff_len++] = usart_data_receive(USART0);// �ѽ��յ������ݷŵ���������
	}
    if(usart_interrupt_flag_get(USART0,USART_INT_FLAG_IDLE) == SET)	
	{
		usart_data_receive(USART0);	
        uart0_complete_flag = 1;
        uart0_buff_len = 0;
	}
    
}

void UART3_IRQHandler()
{
    if(usart_interrupt_flag_get(UART3,USART_INT_FLAG_RBNE) == SET||uart3_complete_flag == 0)   // ���ջ�������Ϊ��
	{
		uart3_buff[uart3_buff_len++] = usart_data_receive(UART3);// �ѽ��յ������ݷŵ���������
	}
    if(usart_interrupt_flag_get(UART3,USART_INT_FLAG_IDLE) == SET)	
	{
		usart_data_receive(UART3);	
        uart3_complete_flag = 1;
        uart3_buff_len = 0;
	}
    
}

void uart3_gpio_config(uint32_t band_rate)
{
	/* ����ʱ�� */
	rcu_periph_clock_enable(RCU_GPIOC);   
	rcu_periph_clock_enable(RCU_UART3);
	
	/* ����GPIO���ù��� */
    gpio_af_set(GPIOC,GPIO_AF_8,GPIO_PIN_10);
    gpio_af_set(GPIOC,GPIO_AF_8,GPIO_PIN_11);	
	
	/* ����GPIO��ģʽ */
	/* ����TX RXΪ����ģʽ ����ģʽ */
	gpio_mode_set(GPIOC,GPIO_MODE_AF,GPIO_PUPD_PULLUP,GPIO_PIN_10);
	gpio_mode_set(GPIOC,GPIO_MODE_AF,GPIO_PUPD_PULLUP,GPIO_PIN_11);
	
	/* ����TXΪ������� 50MHZ */
	gpio_output_options_set(GPIOC,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_10);
	gpio_output_options_set(GPIOC,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_11);

	/* ���ô��ڵĲ��� */
	usart_deinit(UART3);                                 // ��λ����
	usart_baudrate_set(UART3,band_rate);                 // ���ò�����
	usart_parity_config(UART3,USART_PM_NONE);            // û��У��λ
	usart_word_length_set(UART3,USART_WL_8BIT);          // 8λ����λ
	usart_stop_bit_set(UART3,USART_STB_1BIT);     			 // 1λֹͣλ

  /* ʹ�ܴ��� */
	usart_enable(UART3);                                 // ʹ�ܴ���
	//usart_transmit_config(UART3,USART_TRANSMIT_ENABLE);  // ʹ�ܴ��ڷ���
	usart_receive_config(UART3,USART_RECEIVE_ENABLE);    // ʹ�ܴ��ڽ���
	
	/* �ж����� */
	nvic_irq_enable(UART3_IRQn, 2, 2); 									 // �����ж����ȼ�
    usart_interrupt_enable(UART3,USART_INT_RBNE);				 // �����ݻ������ǿ��жϺ���������ж�
	usart_interrupt_enable(UART3,USART_INT_IDLE);				 // ���м���ж�
}
