#include "ads8688.h"
#include "delay.h"
#include "iwdg.h"
/**********************************************************************************************************
*	�� �� ��: ADS8688_IO_Init
*	����˵��: ADS8688 IO�ڳ�ʼ��
*	��    ��: ��
*	�� �� ֵ: ��
**********************************************************************************************************/
void ADS8688_IO_Init(void)	   //ADS8688 IO�ڳ�ʼ��
{
	GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ�ܶ˿�ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ�ܶ˿�ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5;			//PB3ʱ�ӣ�PB5���MOSI 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);//PB4����MISO
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//��JATG���Խӿ�
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//CS
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;; 		 	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
    GPIO_Init(GPIOD, &GPIO_InitStructure);

	CS_H;
   // RST_H;	
	CLK_L;
	delay_ms(5);
}
/**********************************************************************************************************
*	�� �� ��: ADS8688_Init
*	����˵��: ADS8688��ʼ��
*	��    ��: ��
*	�� �� ֵ: ��
**********************************************************************************************************/
void ADS8688_Init(uint8_t ch_en)	   //ADS8688��ʼ��
{
	ADS8688_IO_Init();
	SOFT_RESET(); //��λ
	delay_ms(25);
    Set_Auto_Scan_Sequence(ch_en);//ʹ��ͨ��
	ADS8688_WriteReg(CH_PD,~ch_en);//ͨ���˳��͹���״̬ ͨ���ϵ�
	delay_ms(50);
	while(ADS8688_ReadReg(AUTO_SEQ_EN)!=ch_en)//�Ƿ�ʹ�ܳɹ�
	{
		Set_Auto_Scan_Sequence(ch_en);//ʹ��ͨ��
		ADS8688_WriteReg(CH_PD,~ch_en);//ͨ���˳��͹���״̬ ͨ���ϵ�
		delay_ms(50);
	}		
}

/**********************************************************************************************************
*	�� �� ��: ADS8688_SPI_Read8Bit
*	����˵��: ��SPI���߶�8��bit���ݡ� ����CS���ơ�
*	��    ��: ��
*	�� �� ֵ: ��
**********************************************************************************************************/
uint8_t ADS8688_SPI_Read8Bit(void)
{
    uint8_t i=0;
    uint8_t read=0;
  for(i=0;i<8;i++)
  {
    read<<=1;
    CLK_H;
		//delay_us(1);
    if(READ_SDO) read++;
    CLK_L;
		//delay_us(1);
  }
  return read;
}
/**********************************************************************************************************
*	�� �� ��: ADS8688_SPI_Write8Bit
*	����˵��: ��SPI����д8��bit���ݡ� ����CS���ơ�
*	��    ��: data : ����
*	�� �� ֵ: ��
**********************************************************************************************************/
void ADS8688_SPI_Write8Bit(uint8_t data)
{
    uint8_t i=0;
    CS_L;
  for(i=0;i<8;i++)
  {
    if(data&0x80)	SDI_H;
    else	SDI_L;
		data<<=1;
    CLK_H;
		//delay_us(1);
    CLK_L;
		//delay_us(1);
  }
}
/**********************************************************************************************************
*	�� �� ��: ADS8688_WriteCmd
*	����˵��: д����Ĵ���
*	��    ��:  cmd : ����
*	�� �� ֵ: ��
**********************************************************************************************************/
void ADS8688_WriteCmd(uint16_t cmd)//дADS8688����Ĵ���
{
    CS_L;
    ADS8688_SPI_Write8Bit(cmd>>8 & 0XFF);
    ADS8688_SPI_Write8Bit(cmd & 0XFF);
    ADS8688_SPI_Write8Bit(0X00);
    ADS8688_SPI_Write8Bit(0X00);
    CS_H;
}

/*
*********************************************************************************************************
*	�� �� ��: SOFT_RESET
*	����˵��: �����λ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SOFT_RESET(void)//�����λ
{
    ADS8688_WriteCmd(RST);
}

/**********************************************************************************************************
*	�� �� ��: AUTO_RST_Mode
*	����˵��: �Զ�ģʽ
*	��    ��: ��
*	�� �� ֵ: ��
**********************************************************************************************************/
void AUTO_RST_Mode(void)//�Զ�ɨ��ģʽ
{
    ADS8688_WriteCmd(AUTO_RST);
}
/**********************************************************************************************************
*	�� �� ��: MAN_CH_Mode
*	����˵��: �ֶ�ģʽͨ��ѡ��
*	��    ��:  ch : Ҫѡ���ͨ��
*	�� �� ֵ: ��
**********************************************************************************************************/
void MAN_CH_Mode(uint16_t ch)//�ֶ�ģʽ
{
    ADS8688_WriteCmd(ch);	
}

/**********************************************************************************************************
*	�� �� ��: ADS8688_ReadReg
*	����˵��: ��ָ���ļĴ���
*	��    ��:  addr : �Ĵ�����ַ
*	�� �� ֵ: data : �����ļĴ���ֵ��
**********************************************************************************************************/
uint8_t ADS8688_ReadReg(uint8_t addr)
{
  uint8_t data = 0;
  CS_L;
  ADS8688_SPI_Write8Bit(addr<<1);
  data = ADS8688_SPI_Read8Bit();
  data = ADS8688_SPI_Read8Bit();
  CS_H;
  return data;
}
/**********************************************************************************************************
*	�� �� ��: ADS8688_WriteReg
*	����˵��: дָ���ļĴ���
*	��    ��:  addr : �Ĵ�����ַ
*			  		data : �Ĵ���ֵ
*	�� �� ֵ: ��
**********************************************************************************************************/
void ADS8688_WriteReg(uint8_t addr,uint8_t data)
{
  CS_L;
  ADS8688_SPI_Write8Bit(addr<<1| 0X01);
  ADS8688_SPI_Write8Bit(data);
  CS_H;
}
/**********************************************************************************************************
*	�� �� ��: Set_Auto_Scan_Sequence
*	����˵��: �����Զ�ɨ������ͨ��
*	��    ��:  seq����Ҫ�����Զ�ɨ������ʹ�ܼĴ�����ֵ
*	�� �� ֵ: ��
**********************************************************************************************************/
void Set_Auto_Scan_Sequence(uint8_t seq)	
{
	ADS8688_WriteReg(AUTO_SEQ_EN, seq);
}
/**********************************************************************************************************
*	�� �� ��: Set_CH_Range
*	����˵��: ����ָ��ͨ��������Χ
*	��    ��:  ch : ͨ��
*			  		range : ������Χ
*	�� �� ֵ: ��
**********************************************************************************************************/
void Set_CH_Range(uint8_t ch,uint8_t range)
{
  ADS8688_WriteReg(ch,range);
}

/**********************************************************************************************************
*	�� �� ��: Get_AUTO_RST_Mode_Data
*	����˵��: ��ȡ�Զ�ɨ��ģʽADֵ
*	��    ��:data : ����ָ��
*			  	chnum : ͨ������
*	�� �� ֵ: ��
**********************************************************************************************************/
void Get_AUTO_RST_Mode_Data(uint16_t *data, uint8_t chnum)
{
  uint8_t i=0,datal=0,datah=0;
  for (i=0; i<chnum; i++)
  {
    CS_L;
    ADS8688_SPI_Write8Bit(0X00);
    ADS8688_SPI_Write8Bit(0X00);
    datah = ADS8688_SPI_Read8Bit();
    datal = ADS8688_SPI_Read8Bit();
    //OSTimeDly(1); 
    //delay_us(1);
    CS_H;
    *(data+i) = datah<<8 | datal;
  }
}

/**********************************************************************************************************
*	�� �� ��: Get_MAN_CH_Mode_Data
*	����˵��: ��ȡ�ֶ�ģʽADֵ
*	��    ��: ��
*	�� �� ֵ: ��
**********************************************************************************************************/
uint16_t Get_MAN_CH_Mode_Data(void)
{
  u8 datah=0,datal=0;
  CS_L;
  ADS8688_SPI_Write8Bit(0X00);
  ADS8688_SPI_Write8Bit(0X00);
  datah = ADS8688_SPI_Read8Bit();
  datal = ADS8688_SPI_Read8Bit();
  CS_H;
  return (datah<<8 | datal);
}



u32 ADS8688_Filter(u8 ADC_Channel,u8 times)
{
    static u32 Press_buffer[50] = {0};
    static u32 Press_Databuff[50] = {0};
    static u32 Angle_buffer[50] = {0};
    static u32 Angle_Databuff[50] = {0};
    u32 AD_data = 0;
    uint16_t value[2] = {0};
    u32 ave_value[2];
    Get_AUTO_RST_Mode_Data(value,2);//�Զ�ɨ��ģʽ�ɼ�
    switch(ADC_Channel)
    {
        case 0:
            ave_value[0] = (value[0]* CONST_2_5V_LSB_mV);
            AD_data = filter_mode_extract_middle_value(Press_buffer,Press_Databuff,ave_value[0],times);
            break;
        case 1:
            ave_value[1] = (value[1]* CONST_2_5V_LSB_mV);
            AD_data = filter_mode_extract_middle_value(Angle_buffer,Angle_Databuff,ave_value[1],times);
            break;
        default:
            break;
    }
    return  AD_data;
}

/*==============================================================================
* ��    �ƣ� Voltage_Filter
* ��    �ܣ� ��ȡ�м�ֵ
             filterBuf ------ �����û���    
             sourceBuf ------ �ɼ����滺��
             dat ------------ �ɼ�����
* ��ڲ����� ��
* ���ڲ����� ��
==============================================================================*/
u32 filter_mode_extract_middle_value(u32 *calculateBuf,u32 *sourceBuf,u32 dat,int fliterTimes)
{
  int i,j,m;
  u32 temp;
  u32 sum=0;
  if(fliterTimes<3) fliterTimes=3;
  for(i=0;i<fliterTimes-1;i++)
  {
    sourceBuf[i]=sourceBuf[i+1]; //�������Ƹ���
  }
  sourceBuf[fliterTimes-1]=dat;     //�������һλ����
  for(i=0;i<fliterTimes;i++)
  {
     calculateBuf[i]=sourceBuf[i];//�������ݽ���ð������
  }
  for(j=0;j<fliterTimes-1;j++)
  {
   for(m=0;m<fliterTimes-j;m++)
   {
    if(calculateBuf[m]>calculateBuf[m+1])
     {
      temp = calculateBuf[m];
      calculateBuf[m] = calculateBuf[m+1];
      calculateBuf[m+1] = temp;
     }
   }
  }
  for(i=1;i<fliterTimes-1;i++)
  {
     sum += calculateBuf[i];
  }
  sum= sum/(fliterTimes-2);
  return sum;
}
