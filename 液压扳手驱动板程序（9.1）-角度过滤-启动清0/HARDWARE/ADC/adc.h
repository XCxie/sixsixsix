#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"

/*
16路ADC采样DMA传输
调用Adc_Init()函数初始化设备，ADC循环采样 
数据被DMA搬运到ADC_Value[ADC_CH_NUM]数组中
滤波后的数据在ADC_AfterFilter[ADC_CH_NUM]中
*/


void Adc_Init(void);										//ADC设备初始化
//u16 ADC_Filter(uint8_t ADC_Channel, u8 times);				//ADC原始数据滤波
u16 ADC_Filter(u8 ADC_Channel);
#endif 
