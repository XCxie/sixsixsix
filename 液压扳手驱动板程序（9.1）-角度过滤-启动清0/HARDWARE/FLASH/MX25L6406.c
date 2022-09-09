#include "MX25L6406.h"
#include "Hydraulic_control.h"
#include "usart.h"	
#include "Data_process.h"
#include "hmi_user_uart.h"	
#include "TASK.h"
u8 Data_Storage = 0;
u32  Bolt_data[211] ={39,57,79,141,226,340,481,679,890,1183,1866,2805,4101,5609,7605,10047,
							12943,16354,20319,24860,127,184,255,451,728,1091,1344,1897,2488,3303,3871,5822,8514,
							11644,15786,20853,126,182,251,446,719,1079,1526,2154,2825,3749,5913,8892,13004,17783,
							21821,28816,37127,46909,58284,71308,145,209,287,509,821,1232,1744,2540,3227,4283,6758,10162,14862,20324,
							24109,31850,41035,51847,64419,78720,69,98,137,176,225,314,441,588,735,980,1176,1519,1764,2254,2744,3528,4018,
							4998,5684,6468,7350,8143,8820,10584,13720,16366,19894,98,137,206,296,333,470,637,882,1127,1470,1764,2352,2744,3430,4116,5149,5978,
							7448,8526,9800,10780,12250,13720,16170,20090,24990,29890,137,206,284,402,539,686,1029,1225,1470,1764,2156,2744,3136,3920,4704,5978,7742,
							8820,10780,12642,14700,18130,22050,24500,29204,34300,40670,165,247,341,569,765,981,1472,1962,2060,2453,2943,3826,4415,5592,6573,8437,10791,
							13230,16170,19796,23520,27636,32144,38220,44884,53704,64876,225,363,480,676,911,1176,1764,2352,2450,2940,3626,4606,5390,6664,8330,10290,13230,
							16170,19796,24000,28400,33516,39200,46500,55664,66836,79870};

/*  
  *   CS       PA4
  *   SCLK     PA5
  *   MISO     PA6                 
  *   MOSI     PA7                 
*/  

/*************************************************************
8M:1024*8K 128��,2048����,32768ҳ
ҳ:256B
����:4K,
��:64K

**************************************************************/
void MX25L6406_Init(void)
{
	/*��ʼ����SPI��GPIO�ṹ��*/  
	 SPI_InitTypeDef  SPI_InitStructure;  
	 GPIO_InitTypeDef GPIO_InitStructure;  
		 
	 RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1,  ENABLE); /*��RCC_APB2ENB��ʹ��SPI1ʱ��(λ14)*/  
	 RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,  ENABLE);/*��Ϊ��SPI1��ص�4�����ź�GPIOA��*/  
                                                                                                                                                                                                                                                /*�أ�GPIOBʱ��(λ3)��������ڻ��� */  
																																																													 /*ȷ��Ҫ��Ҫ��������ʱ��ȷ��              */  
	/*������һ���Ǳ���ģ���ΪCS���ǵ���GPIO��ʹ�õģ�2011-01-30����*/  
																																																												
	/*����SPI_FLASH_CLK(PA5)��SPI_FLASH_MISO(PA6)��SPI_FLASH_MOSI(PA7)*/  
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;  
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;                    /*���ù�������ʽ���*/  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;  
	GPIO_Init( GPIOA, &GPIO_InitStructure);  
		
	/*��������SPI_FLASH_CS(PA2)*/  
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;  
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;   /*����ʽ���*/  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;  
	GPIO_Init( GPIOA, &GPIO_InitStructure);  

    MX25L6406_CS_H;             /*��ѡflash*/  


	/* SPI1���� ������2010-01-13*/  
	/* ע��:  ��SPI_NSS_Softģʽ�£�SSIλ������NSS������(PA4)�ĵ�ƽ�� 
		*            ��SSM=1ʱ�ͷ���NSS���ţ�NSS���ſ�������GPIO��*/  
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   /*˫��˫��ȫ˫��BIDI MODE=0*/  
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                                 /*SSIλΪ1��MSTRλΪ1*/  
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                              /*SPI���ͽ���8λ֡�ṹ*/  
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;                                     /*CPOL=1,CPHA=1��ģʽ3*/  
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;  
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                                          /*�ڲ�NSS�ź���SSIλ���ƣ�SSM=1*/  
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;    /*������Ԥ��ƵֵΪ4*/  
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                                       /*���ݴ����MSBλ��ʼ*/  
	SPI_InitStructure.SPI_CRCPolynomial = 7;                                                      /*��λĬ��ֵ*/  
	SPI_Init(SPI1, &SPI_InitStructure);  

	SPI_Cmd(SPI1,ENABLE);       /*ʹ��SPI2*/  
}

/*����:       SPI����һ���ֽ� 
  *����:       send_data:   �����͵��ֽ� 
  *����:       ��*/  
u8 SPI1_ReadWriteByte(u8 send_data)  
{  
    /*���Busyλ��SPI��SR�е�λ7��SPIͨ���Ƿ�Ϊæ��ֱ����æ����*/  
    //while( SET==SPI_I2S_GetFlagStatus(SPI_SELECT, SPI_I2S_FLAG_BSY));  
      
    /*���TXEλ��SPI��SR�е�λ1�����ͻ������Ƿ�Ϊ�գ�ֱ��������*/  
    while( RESET==SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));  
  
    SPI_I2S_SendData(SPI1, send_data);                        /*����һ���ֽ�*/  
      
    /*�������ݺ��ٽ���һ���ֽ�*/  
    while( RESET==SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) );  
    return( SPI_I2S_ReceiveData(SPI1) );  
      
}  


//дʹ��ָ�0x06��
void MX25L6406_Write_EN(void)
{
	//CS����
	MX25L6406_CS_L;
	//����ָ��0x06
	SPI1_ReadWriteByte(0x06);
	//CS����
	MX25L6406_CS_H;
}
//��״̬�Ĵ���ָ�0x05��
void MX25L6406_Read_status(void)
{
	u8 data;
	//CS����
	MX25L6406_CS_L;
	//����ָ��0x05
	SPI1_ReadWriteByte(0x05);
	//�������ղ����BUSYλ��״̬��ֱ����æ��0��
	do
	{
		data = SPI1_ReadWriteByte(0x00);//����һ����Ч���ݣ�Ŀ���ǽ���
	}while(data);
	//CS����
	MX25L6406_CS_H;
}

//������ָ�0x03��	
void MX25L6406_Read(u8 *data,u32 addr,u32 n)
{
	u32 i;
	//CS����
	MX25L6406_CS_L;
	//����ָ��0x03
	SPI1_ReadWriteByte(0x03);
	//����24λ��ַ���û���Ҫ�Ӹõ�ַ��ʼ��ȡ���ݵĵ�ַ��,�ȷ��͸�λ
	SPI1_ReadWriteByte(addr>>16);
	SPI1_ReadWriteByte(addr>>8);
	SPI1_ReadWriteByte(addr);
	//����n�ν��գ���ȡ������
	for(i=0;i<n;i++)
	{
		*data = SPI1_ReadWriteByte(0x00);
		data++;
	}
	//CS����
	MX25L6406_CS_H;
}
//ҳ��̣�д���ݣ�ָ�0x02��
void MX25L6406_Write_data(u32 addr,u8 data)
{
	//дʹ��
	MX25L6406_Write_EN();
	//CS����
	MX25L6406_CS_L;
	//����ָ��0x02
	SPI1_ReadWriteByte(0x02);
	//����24λ��ַ���û���Ҫ�Ӹõ�ַ��ʼд�����ݵĵ�ַ��
	SPI1_ReadWriteByte(addr>>16);
	SPI1_ReadWriteByte(addr>>8);
	SPI1_ReadWriteByte(addr);
	//�������ͣ�д�룩һ������
	SPI1_ReadWriteByte(data);
	//CS����
	MX25L6406_CS_H;
	//��ȡBUSYλ�ж������Ƿ����
	MX25L6406_Read_status();
}
//��������ָ�0x20��
//�����ָ�0xD8��
//����оƬ����ָ�0xC7��
void MX25L6406_Erase(u32 addr,u8 cmd)
{
	//дʹ��
	MX25L6406_Write_EN();
	//CS����
	MX25L6406_CS_L;
	//����ָ��
	SPI1_ReadWriteByte(cmd);
	if(cmd==Sector || cmd==Block)
	{
		//����24λ��ַ���û���Ҫ�Ӹõ�ַ��ʼд�����ݵĵ�ַ��
		SPI1_ReadWriteByte(addr>>16);
		SPI1_ReadWriteByte(addr>>8);
		SPI1_ReadWriteByte(addr);
	}
	//CS����
	MX25L6406_CS_H;
	//��ȡBUSYλ�ж������Ƿ����
	MX25L6406_Read_status();
}


//д���ֽ�
void MX25L6406_Write(u8 *ftrBuffer,u32 addr, u32 ftrLen)
{
	u32 i=0;
	//дʹ��
	MX25L6406_Write_EN();
	//CS����
	MX25L6406_CS_L;
	//����ָ��0x02
	SPI1_ReadWriteByte(0x02);
	//����24λ��ַ���û���Ҫ�Ӹõ�ַ��ʼд�����ݵĵ�ַ��
	SPI1_ReadWriteByte(addr>>16);
	SPI1_ReadWriteByte(addr>>8);
	SPI1_ReadWriteByte(addr);
	//�������ͣ�д�룩һ������
	for(i=0;i<ftrLen;i++)
	SPI1_ReadWriteByte(ftrBuffer[i]);
	//CS����
	MX25L6406_CS_H;
	//��ȡBUSYλ�ж������Ƿ����
	MX25L6406_Read_status();
}

void MX25L6406_WriteU16(u16 data,u32 addr)
{
	u8 a,b;
	a=data >> 8;
	b=data & 0xff;
	MX25L6406_Write_data(addr,a);
	MX25L6406_Write_data(addr+1,b);
}

u16 MX25L6406_ReadU16(u32 addr)
{
	u8 a[5];
	MX25L6406_Read(a,addr,2);

	return ((a[0]<<8)+a[1]);
}

//32λ���ݶ�д
void MX25L6406_WriteU32(u32 data,u32 addr)
{
    u8 buf[10];
    buf[0] = (u8)(data >>24);
    buf[1] = (u8)(data >>16);
    buf[2] = (u8)(data >>8);
    buf[3] = (u8)data;
    
    MX25L6406_Write(buf,addr,4);
    
}

u32 MX25L6406_ReadU32(u32 addr)
{
    u8 buf[10];
    u32 data ;
    MX25L6406_Read(buf,addr,4);
    data =  ((u32)buf[0] << 24) | ((u32)buf[1] << 16) | ((u32)buf[2] << 8) | ((u32)buf[3]); 
    return data;
}

u8 Parameter_Storage_Status = 0;  //�Ƿ�Ϊ��һ�δ洢
void  Get_Flash_Parameters(void)
{
       
    MX25L6406_Read(&Parameter_Storage_Status,13*4096,1);
    //printf("%d\r\n",Parameter_Storage_Status);
    if(Parameter_Storage_Status == 255)    //    ��flashδд���ʱ����Ĭ�ϲ���д��
    {
        MX25L6406_Erase(0*4096,0x20);
        MX25L6406_Erase(1*4096,0x20);
        MX25L6406_Erase(2*4096,0x20);
        MX25L6406_Erase(3*4096,0x20);
        MX25L6406_Erase(4*4096,0x20);
        MX25L6406_Erase(5*4096,0x20);
        MX25L6406_Erase(6*4096,0x20);
        MX25L6406_Erase(7*4096,0x20);
        MX25L6406_Erase(8*4096,0x20);
        MX25L6406_Erase(9*4096,0x20);
        MX25L6406_Erase(10*4096,0x20);
        MX25L6406_Erase(11*4096,0x20);
        MX25L6406_Erase(12*4096,0x20);    
        MX25L6406_WriteU32(SpannerDate.p_SpannerCoefficient,0*4096);
        MX25L6406_WriteU32(SpannerDate.p_UserTargetTorque,1*4096);    
        MX25L6406_WriteU32(SpannerDate.p_BoltTorque,2*4096);   
        MX25L6406_WriteU32(SpannerDate.p_GearTorque,3*4096);    
        MX25L6406_WriteU32(SpannerDate.p_TargetAngle,4*4096);  
        MX25L6406_WriteU32(SpannerDate.p_TargetSensor,5*4096);  
        MX25L6406_WriteU32(SpannerDate.p_TargetUsageNumber,6*4096);
        MX25L6406_Write(&SpannerDate.p_Mode,7*4096,1); 
        MX25L6406_WriteU32(Usage_Counter,8*4096);
        MX25L6406_Write((u8*)&Unlock_Password,9*4096,8);
        MX25L6406_Write((u8*)&Password,10*4096,8);  
        MX25L6406_Write(&Passwordflag,11*4096,1);
        MX25L6406_Write(&SpannerDate.p_OilReturnTime,12*4096,1);  
        Parameter_Storage_Status = 1;       //��־λ�����ı䲢����flash         
        MX25L6406_Write(&Parameter_Storage_Status,13*4096,1);    //13����     ��Ŵ洢״̬��־λ 
    }else
    {
        //p_SpannerCoefficient
        SpannerDate.p_SpannerCoefficient = MX25L6406_ReadU32(0*4096);
        //p_UserTargetTorque
        SpannerDate.p_UserTargetTorque = MX25L6406_ReadU32(1*4096);
        //p_BoltTorque
        SpannerDate.p_BoltTorque = MX25L6406_ReadU32(2*4096);
        //p_GearTorque
        SpannerDate.p_GearTorque = MX25L6406_ReadU32(3*4096);
        //p_TargetAngle
        SpannerDate.p_TargetAngle = MX25L6406_ReadU32(4*4096);
        //p_TargetSensor
        SpannerDate.p_TargetSensor = MX25L6406_ReadU32(5*4096);
        //p_TargetUsageNumber
        SpannerDate.p_TargetUsageNumber = MX25L6406_ReadU32(6*4096);
        if(SpannerDate.p_TargetUsageNumber >= 2000000)
        {
            SpannerDate.p_TargetUsageNumber = 2000000;
        }
        //p_Mode
        MX25L6406_Read(&SpannerDate.p_Mode,7*4096,1);
        //Usage_Counter
        Usage_Counter = MX25L6406_ReadU32(8*4096);
        if(Usage_Counter >= 2000000)
        {
            Usage_Counter = 2000000;
        }
        //Unlock_Password
        MX25L6406_Read((u8*)&Unlock_Password,9*4096,8);
        //Password
        MX25L6406_Read((u8*)&Password,10*4096,8);
        //Passwordflag
        MX25L6406_Read(&Passwordflag,11*4096,1);
        //p_OilReturnTime
        MX25L6406_Read(&SpannerDate.p_OilReturnTime,12*4096,1); 
#if 0
        printf("p_SpannerCoefficient:%d\r\n",SpannerDate.p_SpannerCoefficient);
        printf("p_UserTargetTorque:%d\r\n",SpannerDate.p_UserTargetTorque);
        printf("p_BoltTorque:%d\r\n",SpannerDate.p_BoltTorque);
        printf("p_GearTorque:%d\r\n",SpannerDate.p_GearTorque);
        printf("p_TargetAngle:%d\r\n",SpannerDate.p_TargetAngle);
        printf("p_TargetSensor:%d\r\n",SpannerDate.p_TargetSensor);
        printf("p_TargetUsageNumber:%d\r\n",SpannerDate.p_TargetUsageNumber);
        printf("p_Mode:%d\r\n",SpannerDate.p_Mode);
        printf("Unlock_Password:%s\r\n",Unlock_Password);
        printf("Password:%s\r\n",Password);
        printf("Usage_Counter:%d\r\n",Usage_Counter);
        printf("Passwordflag:%d\r\n",Passwordflag);
        printf("SpannerDate.p_OilReturnTime:%d\r\n",SpannerDate.p_OilReturnTime);
#endif
    }
       

}



void Flash_Model_parameters(u8* Storage)
{
    switch(*Storage)
    {
        case 1:
            MX25L6406_Erase(0*4096,0x20);
            MX25L6406_WriteU32(SpannerDate.p_SpannerCoefficient,0*4096);
            *Storage = 0;
            break;
        case 2:
            MX25L6406_Erase(1*4096,0x20);
            MX25L6406_WriteU32(SpannerDate.p_UserTargetTorque,1*4096); 
            *Storage = 0;
            break;
        case 3:
            MX25L6406_Erase(2*4096,0x20);
            MX25L6406_WriteU32(SpannerDate.p_BoltTorque,2*4096);
            *Storage = 0;
            break;
        case 4:
            MX25L6406_Erase(3*4096,0x20);
            MX25L6406_WriteU32(SpannerDate.p_GearTorque,3*4096);
            *Storage = 0;
            break;
        case 5:
            MX25L6406_Erase(4*4096,0x20);
            MX25L6406_WriteU32(SpannerDate.p_TargetAngle,4*4096);
            *Storage = 0;
            break;
        case 6:
            MX25L6406_Erase(5*4096,0x20);
            MX25L6406_WriteU32(SpannerDate.p_TargetSensor,5*4096); 
            *Storage = 0;
            break;
        case 7:
            MX25L6406_Erase(6*4096,0x20);
            MX25L6406_WriteU32(SpannerDate.p_TargetUsageNumber,6*4096);
            *Storage = 0;
            break;
        case 8:
            MX25L6406_Erase(7*4096,0x20);
            MX25L6406_Write(&SpannerDate.p_Mode,7*4096,1);   
            *Storage = 0;
            break;
        case 9:
            MX25L6406_Erase(8*4096,0x20);
            MX25L6406_WriteU32(Usage_Counter,8*4096);
            *Storage = 0;
            break;
        case 10:
            MX25L6406_Erase(9*4096,0x20);
            MX25L6406_Erase(11*4096,0x20);
            MX25L6406_Write((u8*)&Unlock_Password,9*4096,8);
            MX25L6406_Write(&Passwordflag,11*4096,1);
            *Storage = 0;
            break;
        case 11:
            MX25L6406_Erase(10*4096,0x20);
            MX25L6406_Erase(11*4096,0x20);
            MX25L6406_Write((u8*)&Password,10*4096,8);  
            MX25L6406_Write(&Passwordflag,11*4096,1);
            *Storage = 0;
            break;
        case 12:
            MX25L6406_Erase(12*4096,0x20);
            MX25L6406_Write(&SpannerDate.p_OilReturnTime,12*4096,1);   
            *Storage = 0;
            break;
    }
}

