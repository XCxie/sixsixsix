#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"

/*
16·ADC����DMA����
����Adc_Init()������ʼ���豸��ADCѭ������ 
���ݱ�DMA���˵�ADC_Value[ADC_CH_NUM]������
�˲����������ADC_AfterFilter[ADC_CH_NUM]��
*/


void Adc_Init(void);										//ADC�豸��ʼ��
//u16 ADC_Filter(uint8_t ADC_Channel, u8 times);				//ADCԭʼ�����˲�
u16 ADC_Filter(u8 ADC_Channel);
#endif 
