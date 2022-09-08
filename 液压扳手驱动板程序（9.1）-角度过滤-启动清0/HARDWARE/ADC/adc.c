 #include "adc.h"
/*
1路ADC采样DMA传输
调用Adc_Init()函数初始化设备，ADC循环采样 
数据被DMA搬运到ADC_Value[ADC_CH_NUM]数组中
滤波后的数据在ADC_AfterFilter[ADC_CH_NUM]中
*/
	   		   	
#define ADC_CH_NUM 30     
u16 ADC_Value[2*ADC_CH_NUM];					
u16 ADC_AfterFilter[ADC_CH_NUM] = {0};		
//#define ADC_CH_NUM 2 
//u16 ADC_Value[ADC_CH_NUM] = {0};					
//u16 ADC_AfterFilter[ADC_CH_NUM] = {0};	

void  Adc_Init(void)
{ 	
    ADC_InitTypeDef ADC_InitStructure; 
    DMA_InitTypeDef  DMA_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1,ENABLE );	  //使能ADC1通道时钟

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

    //PA1-PA2 作为模拟通道输入引脚                         
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
    GPIO_Init(GPIOA, &GPIO_InitStructure);		

    /* DMA channel1 configuration */
    DMA_DeInit(DMA1_Channel1);
    /*定义DMA外设基地址,即为存放转换结果的寄存器*/
    DMA_InitStructure.DMA_PeripheralBaseAddr =(u32)&(ADC1->DR);
    /*定义内存基地址*/
    DMA_InitStructure.DMA_MemoryBaseAddr =(u32)ADC_Value;
    /*定义AD外设作为数据传输的来源*/
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    /*指定DMA通道的DMA缓存的大小,即需要开辟几个内存空间，由ADC_CH_NUM这个宏决定*/
    DMA_InitStructure.DMA_BufferSize = 2*ADC_CH_NUM;
    /*设定寄存器地址固定*/
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    /*设定内存地址递加，即每次DMA都是将该外设寄存器中的值传到开辟的内存空间中*/
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    /*设定外设数据宽度 半字*/	
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    /*设定内存的的宽度 半字*/
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    /*设定DMA工作再循环缓存模式*/
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    /*设定DMA选定的通道软件优先级*/
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    /* Enable DMA channel1 */
    DMA_Cmd(DMA1_Channel1, ENABLE);



    ADC_DeInit(ADC1);  //复位ADC1 

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE ;	//模数转换工作在扫描模式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE ;	//模数转换工作在连续转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 2;	//顺序进行规则转换的ADC通道的数目
    ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

    /*ADC通道编号*/
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);

    ADC_DMACmd(ADC1, ENABLE);

    ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1

    ADC_ResetCalibration(ADC1);	//使能复位校准  

    while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束

    ADC_StartCalibration(ADC1);	 //开启AD校准

    while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能 
}				  
	

u16 ADC_Filter(u8 ADC_Channel)
{
	u32 sum=0;
    u8 i;
	for(i=0;i<ADC_CH_NUM;i++)
	{
		sum += ADC_Value[2*i+ADC_Channel];
	}
	return sum/ADC_CH_NUM;
}





















