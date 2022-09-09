 #include "adc.h"
/*
1·ADC����DMA����
����Adc_Init()������ʼ���豸��ADCѭ������ 
���ݱ�DMA���˵�ADC_Value[ADC_CH_NUM]������
�˲����������ADC_AfterFilter[ADC_CH_NUM]��
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

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1,ENABLE );	  //ʹ��ADC1ͨ��ʱ��

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

    //PA1-PA2 ��Ϊģ��ͨ����������                         
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);		

    /* DMA channel1 configuration */
    DMA_DeInit(DMA1_Channel1);
    /*����DMA�������ַ,��Ϊ���ת������ļĴ���*/
    DMA_InitStructure.DMA_PeripheralBaseAddr =(u32)&(ADC1->DR);
    /*�����ڴ����ַ*/
    DMA_InitStructure.DMA_MemoryBaseAddr =(u32)ADC_Value;
    /*����AD������Ϊ���ݴ������Դ*/
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    /*ָ��DMAͨ����DMA����Ĵ�С,����Ҫ���ټ����ڴ�ռ䣬��ADC_CH_NUM��������*/
    DMA_InitStructure.DMA_BufferSize = 2*ADC_CH_NUM;
    /*�趨�Ĵ�����ַ�̶�*/
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    /*�趨�ڴ��ַ�ݼӣ���ÿ��DMA���ǽ�������Ĵ����е�ֵ�������ٵ��ڴ�ռ���*/
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    /*�趨�������ݿ�� ����*/	
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    /*�趨�ڴ�ĵĿ�� ����*/
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    /*�趨DMA������ѭ������ģʽ*/
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    /*�趨DMAѡ����ͨ��������ȼ�*/
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    /* Enable DMA channel1 */
    DMA_Cmd(DMA1_Channel1, ENABLE);



    ADC_DeInit(ADC1);  //��λADC1 

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
    ADC_InitStructure.ADC_ScanConvMode = ENABLE ;	//ģ��ת��������ɨ��ģʽ
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE ;	//ģ��ת������������ת��ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 2;	//˳����й���ת����ADCͨ������Ŀ
    ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

    /*ADCͨ�����*/
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);

    ADC_DMACmd(ADC1, ENABLE);

    ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1

    ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  

    while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����

    ADC_StartCalibration(ADC1);	 //����ADУ׼

    while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת���������� 
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





















