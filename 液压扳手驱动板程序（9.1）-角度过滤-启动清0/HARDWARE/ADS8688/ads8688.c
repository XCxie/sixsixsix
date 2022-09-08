#include "ads8688.h"
#include "delay.h"
#include "iwdg.h"
/**********************************************************************************************************
*	函 数 名: ADS8688_IO_Init
*	功能说明: ADS8688 IO口初始化
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
void ADS8688_IO_Init(void)	   //ADS8688 IO口初始化
{
	GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5;			//PB3时钟，PB5输出MOSI 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);//PB4输入MISO
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//关JATG调试接口
	
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
*	函 数 名: ADS8688_Init
*	功能说明: ADS8688初始化
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
void ADS8688_Init(uint8_t ch_en)	   //ADS8688初始化
{
	ADS8688_IO_Init();
	SOFT_RESET(); //复位
	delay_ms(25);
    Set_Auto_Scan_Sequence(ch_en);//使能通道
	ADS8688_WriteReg(CH_PD,~ch_en);//通道退出低功耗状态 通道上电
	delay_ms(50);
	while(ADS8688_ReadReg(AUTO_SEQ_EN)!=ch_en)//是否使能成功
	{
		Set_Auto_Scan_Sequence(ch_en);//使能通道
		ADS8688_WriteReg(CH_PD,~ch_en);//通道退出低功耗状态 通道上电
		delay_ms(50);
	}		
}

/**********************************************************************************************************
*	函 数 名: ADS8688_SPI_Read8Bit
*	功能说明: 从SPI总线读8个bit数据。 不带CS控制。
*	形    参: 无
*	返 回 值: 无
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
*	函 数 名: ADS8688_SPI_Write8Bit
*	功能说明: 向SPI总线写8个bit数据。 不带CS控制。
*	形    参: data : 数据
*	返 回 值: 无
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
*	函 数 名: ADS8688_WriteCmd
*	功能说明: 写命令寄存器
*	形    参:  cmd : 命令
*	返 回 值: 无
**********************************************************************************************************/
void ADS8688_WriteCmd(uint16_t cmd)//写ADS8688命令寄存器
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
*	函 数 名: SOFT_RESET
*	功能说明: 软件复位
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void SOFT_RESET(void)//软件复位
{
    ADS8688_WriteCmd(RST);
}

/**********************************************************************************************************
*	函 数 名: AUTO_RST_Mode
*	功能说明: 自动模式
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
void AUTO_RST_Mode(void)//自动扫描模式
{
    ADS8688_WriteCmd(AUTO_RST);
}
/**********************************************************************************************************
*	函 数 名: MAN_CH_Mode
*	功能说明: 手动模式通道选择
*	形    参:  ch : 要选择的通道
*	返 回 值: 无
**********************************************************************************************************/
void MAN_CH_Mode(uint16_t ch)//手动模式
{
    ADS8688_WriteCmd(ch);	
}

/**********************************************************************************************************
*	函 数 名: ADS8688_ReadReg
*	功能说明: 读指定的寄存器
*	形    参:  addr : 寄存器地址
*	返 回 值: data : 读到的寄存器值。
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
*	函 数 名: ADS8688_WriteReg
*	功能说明: 写指定的寄存器
*	形    参:  addr : 寄存器地址
*			  		data : 寄存器值
*	返 回 值: 无
**********************************************************************************************************/
void ADS8688_WriteReg(uint8_t addr,uint8_t data)
{
  CS_L;
  ADS8688_SPI_Write8Bit(addr<<1| 0X01);
  ADS8688_SPI_Write8Bit(data);
  CS_H;
}
/**********************************************************************************************************
*	函 数 名: Set_Auto_Scan_Sequence
*	功能说明: 设置自动扫描序列通道
*	形    参:  seq：需要设置自动扫描序列使能寄存器的值
*	返 回 值: 无
**********************************************************************************************************/
void Set_Auto_Scan_Sequence(uint8_t seq)	
{
	ADS8688_WriteReg(AUTO_SEQ_EN, seq);
}
/**********************************************************************************************************
*	函 数 名: Set_CH_Range
*	功能说明: 设置指定通道测量范围
*	形    参:  ch : 通道
*			  		range : 测量范围
*	返 回 值: 无
**********************************************************************************************************/
void Set_CH_Range(uint8_t ch,uint8_t range)
{
  ADS8688_WriteReg(ch,range);
}

/**********************************************************************************************************
*	函 数 名: Get_AUTO_RST_Mode_Data
*	功能说明: 读取自动扫描模式AD值
*	形    参:data : 数据指针
*			  	chnum : 通道个数
*	返 回 值: 无
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
*	函 数 名: Get_MAN_CH_Mode_Data
*	功能说明: 读取手动模式AD值
*	形    参: 无
*	返 回 值: 无
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
    Get_AUTO_RST_Mode_Data(value,2);//自动扫描模式采集
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
* 名    称： Voltage_Filter
* 功    能： 抽取中间值
             filterBuf ------ 过程用缓存    
             sourceBuf ------ 采集保存缓存
             dat ------------ 采集数据
* 入口参数： 无
* 出口参数： 无
==============================================================================*/
u32 filter_mode_extract_middle_value(u32 *calculateBuf,u32 *sourceBuf,u32 dat,int fliterTimes)
{
  int i,j,m;
  u32 temp;
  u32 sum=0;
  if(fliterTimes<3) fliterTimes=3;
  for(i=0;i<fliterTimes-1;i++)
  {
    sourceBuf[i]=sourceBuf[i+1]; //数据左移更新
  }
  sourceBuf[fliterTimes-1]=dat;     //更新最后一位数据
  for(i=0;i<fliterTimes;i++)
  {
     calculateBuf[i]=sourceBuf[i];//复制数据进行冒泡排序
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
