#include "power_uart.h"

uint8_t 	uart0_buff[20],uart3_buff[26];// 接收缓冲区
uint8_t 	uart0_buff_len = 0,uart3_buff_len = 0;// 接收数据长度
uint8_t		uart0_complete_flag = 0,uart3_complete_flag = 0; // 接收数据完成标志位

void usart0_gpio_config(uint32_t band_rate)
{
	/* 开启时钟 */
	rcu_periph_clock_enable(RCU_USART0);
	rcu_periph_clock_enable(RCU_GPIOB);
	/* 配置GPIO复用功能 */
    gpio_af_set(GPIOB,GPIO_AF_7,GPIO_PIN_6);
    gpio_af_set(GPIOB,GPIO_AF_7,GPIO_PIN_7);	
	
	/* 配置GPIO的模式 */
	/* 配置TX RX为复用模式 上拉模式 */
	gpio_mode_set(GPIOB,GPIO_MODE_AF,GPIO_PUPD_PULLUP,GPIO_PIN_6);
	gpio_mode_set(GPIOB,GPIO_MODE_AF,GPIO_PUPD_PULLUP,GPIO_PIN_7);
	
	/* 配置TX为推挽输出 50MHZ */
	gpio_output_options_set(GPIOB,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_6);
	gpio_output_options_set(GPIOB,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_7);

	/* 配置串口的参数 */
	usart_deinit(USART0);                                 // 复位串口
	usart_baudrate_set(USART0,band_rate);                 // 设置波特率
	usart_parity_config(USART0,USART_PM_NONE);            // 没有校验位
	usart_word_length_set(USART0,USART_WL_8BIT);          // 8位数据位
	usart_stop_bit_set(USART0,USART_STB_1BIT);     			 // 1位停止位

  /* 使能串口 */
	usart_enable(USART0);                                 // 使能串口
	usart_transmit_config(USART0,USART_TRANSMIT_ENABLE);  // 使能串口发送
	usart_receive_config(USART0,USART_RECEIVE_ENABLE);    // 使能串口接收
	
	/* 中断配置 */
	nvic_irq_enable(USART0_IRQn, 2, 2); 									 // 配置中断优先级
    usart_interrupt_enable(USART0,USART_INT_RBNE);				 // 读数据缓冲区非空中断和溢出错误中断
	usart_interrupt_enable(USART0,USART_INT_IDLE);				 // 空闲检测中断
}


void usart_send_data(uint8_t ucch)
{
	usart_data_transmit(USART0,(uint8_t)ucch);							 // 发送数据
	while(RESET == usart_flag_get(USART0,USART_FLAG_TBE));  // 等待发送数据缓冲区标志置位
}



void usart_send_string(uint8_t *ucstr,uint8_t length)
{
	while(length--)        // 地址为空或者值为空跳出
	{
	  usart_send_data(*ucstr++);  // 发送单个字符
	}
}


int fputc(int ch, FILE *f)
{
     usart_send_data(ch);
     // 等待发送数据缓冲区标志置位
     return ch;
}

/*串口中断服务函数*/
void USART0_IRQHandler()
{
    if(usart_interrupt_flag_get(USART0,USART_INT_FLAG_RBNE) == SET)   // 接收缓冲区不为空
	{
		uart0_buff[uart0_buff_len++] = usart_data_receive(USART0);// 把接收到的数据放到缓冲区中
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
    if(usart_interrupt_flag_get(UART3,USART_INT_FLAG_RBNE) == SET||uart3_complete_flag == 0)   // 接收缓冲区不为空
	{
		uart3_buff[uart3_buff_len++] = usart_data_receive(UART3);// 把接收到的数据放到缓冲区中
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
	/* 开启时钟 */
	rcu_periph_clock_enable(RCU_GPIOC);   
	rcu_periph_clock_enable(RCU_UART3);
	
	/* 配置GPIO复用功能 */
    gpio_af_set(GPIOC,GPIO_AF_8,GPIO_PIN_10);
    gpio_af_set(GPIOC,GPIO_AF_8,GPIO_PIN_11);	
	
	/* 配置GPIO的模式 */
	/* 配置TX RX为复用模式 上拉模式 */
	gpio_mode_set(GPIOC,GPIO_MODE_AF,GPIO_PUPD_PULLUP,GPIO_PIN_10);
	gpio_mode_set(GPIOC,GPIO_MODE_AF,GPIO_PUPD_PULLUP,GPIO_PIN_11);
	
	/* 配置TX为推挽输出 50MHZ */
	gpio_output_options_set(GPIOC,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_10);
	gpio_output_options_set(GPIOC,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_11);

	/* 配置串口的参数 */
	usart_deinit(UART3);                                 // 复位串口
	usart_baudrate_set(UART3,band_rate);                 // 设置波特率
	usart_parity_config(UART3,USART_PM_NONE);            // 没有校验位
	usart_word_length_set(UART3,USART_WL_8BIT);          // 8位数据位
	usart_stop_bit_set(UART3,USART_STB_1BIT);     			 // 1位停止位

  /* 使能串口 */
	usart_enable(UART3);                                 // 使能串口
	//usart_transmit_config(UART3,USART_TRANSMIT_ENABLE);  // 使能串口发送
	usart_receive_config(UART3,USART_RECEIVE_ENABLE);    // 使能串口接收
	
	/* 中断配置 */
	nvic_irq_enable(UART3_IRQn, 2, 2); 									 // 配置中断优先级
    usart_interrupt_enable(UART3,USART_INT_RBNE);				 // 读数据缓冲区非空中断和溢出错误中断
	usart_interrupt_enable(UART3,USART_INT_IDLE);				 // 空闲检测中断
}
