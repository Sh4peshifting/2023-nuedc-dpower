#include "power_adc.h"

volatile uint32_t adc0_value[6];

void adc_gpio_config()
{
    rcu_periph_clock_enable(RCU_GPIOA);
    /* config the GPIO as analog mode, for ADC */
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_6);
}

void adc_config()
{
    adc_gpio_config();
    rcu_periph_clock_enable(RCU_ADC0);
    rcu_periph_clock_enable(RCU_DMA1);
    
    /* ADC_DMA_channel configuration */
    dma_single_data_parameter_struct dma_single_data_parameter;

    /* ADC DMA_channel configuration */
    dma_deinit(DMA1, DMA_CH0);

    /* initialize DMA single data mode */
    dma_single_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_single_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_single_data_parameter.memory0_addr = (uint32_t)(adc0_value);
    dma_single_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_single_data_parameter.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
    dma_single_data_parameter.direction = DMA_PERIPH_TO_MEMORY;
    dma_single_data_parameter.number = 4;
    dma_single_data_parameter.priority = DMA_PRIORITY_HIGH;
    dma_single_data_mode_init(DMA1, DMA_CH0, &dma_single_data_parameter);
    dma_channel_subperipheral_select(DMA1, DMA_CH0, DMA_SUBPERI0);

    /* enable DMA circulation mode */
    dma_circulation_enable(DMA1, DMA_CH0);

    /* enable DMA channel */
    dma_channel_enable(DMA1, DMA_CH0);
    
    adc_clock_config(ADC_ADCCK_HCLK_DIV6);
     /* ADC mode config */
    adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
    /* ADC continous function enable */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);

    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 4);
    /* ADC routine channel config */
    adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_1, ADC_SAMPLETIME_56);
    adc_regular_channel_config(ADC0, 1, ADC_CHANNEL_2, ADC_SAMPLETIME_56);
    adc_regular_channel_config(ADC0, 2, ADC_CHANNEL_5, ADC_SAMPLETIME_56);
    adc_regular_channel_config(ADC0, 3, ADC_CHANNEL_6, ADC_SAMPLETIME_56);
    /* ADC trigger config */
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_T7_TRGO);
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, EXTERNAL_TRIGGER_RISING);
    /* ADC discontinuous mode */
    //adc_discontinuous_mode_config(ADC0, ADC_REGULAR_CHANNEL, 1);
    /* ADC DMA function enable */
    adc_dma_request_after_last_enable(ADC0);
    adc_dma_mode_enable(ADC0);

    /* enable ADC interface */
    adc_enable(ADC0);
    /* wait for ADC stability */
    delay_1ms(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
    
    
}
