#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"

/*
16·ADC����DMA����
����Adc_Init()������ʼ���豸��ADCѭ������ 
���ݱ�DMA���˵�ADC_Value[ADC_CH_NUM]������
�˲����������ADC_AfterFilter[ADC_CH_NUM]��
*/

#define ADC_CH_NUM 2     //ADCͨ����Ŀ 16��ͨ��

extern u16 ADC_Value[ADC_CH_NUM];				 //ADC����ԭʼ����
extern u16 ADC_AfterFilter[ADC_CH_NUM];  //ADC���������˲��������

void Adc_Init(void);										//ADC�豸��ʼ��
u16 ADC_Filter(uint8_t ADC_Channel);										//ADCԭʼ�����˲�
#endif 
