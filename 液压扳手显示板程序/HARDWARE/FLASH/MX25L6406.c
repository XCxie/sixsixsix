#include "MX25L6406.h"
#include "TASK.h"
#include "Hydraulic_process.h" 
#define  FLASH_INFO  0
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
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;    /*������Ԥ��ƵֵΪ2*/  
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

//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)  		   
u8 SPI_FLASH_BUF[4096];
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    

	secpos=WriteAddr/4096;//������ַ 0~511 for w25x16
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   

	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		MX25L6406_Read(SPI_FLASH_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			MX25L6406_Erase(secpos*4096,0x20);//�����������
			for(i=0;i<secremain;i++)	   //����
			{
				SPI_FLASH_BUF[i+secoff]=pBuffer[i];	  
			}
                SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);//д����������  

		}else SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
		   	NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	};	 	 
}

//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		MX25L6406_Write(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
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
//16λ���ݶ�д
void MX25L6406_WriteU16(u16 data,u32 addr)
{
	u8 buf[2] = {0};
	buf[0]=data >> 8;
	buf[1]=data & 0xff;
	MX25L6406_Write(buf,addr,2);
}

u16 MX25L6406_ReadU16(u32 addr)
{
	u8 buf[5] = {0};
    u16 data;
    MX25L6406_Read(buf,addr,2);
    data = (buf[0]<<8)|buf[1];
	return data;
}

//32λ���ݶ�д
void MX25L6406_WriteU32(u32 data,u32 addr)
{
    u8 buf[4] = {0};
    buf[0] = (u8)(data >>24);
    buf[1] = (u8)(data >>16);
    buf[2] = (u8)(data >>8);
    buf[3] = (u8)data; 
    MX25L6406_Write(buf,addr,4);
    
}

u32 MX25L6406_ReadU32(u32 addr)
{
    u8 buf[4] = {0};
    u32 data ;
    MX25L6406_Read(buf,addr,4);
    data =  ((u32)buf[0] << 24) | ((u32)buf[1] << 16) | ((u32)buf[2] << 8) | ((u32)buf[3]); 
    return data;
}
u8 Parameter_Storage_Status = 0;  //�Ƿ�Ϊ��һ�δ洢
//u8 RecordDatabuf[120] = {0};
//��ȡ����
void  Get_Flash_Parameters(void)
{
    u16 i,j;
    u32 temp;
    u8 buf[100] = {0};
    u8 G_level[7] = {1,2,3,4,5,6,7};
    u32 G_data[7] = {100000,200000,300000,400000,500000,600000,700000};/*     ��λĬ�ϲ���    */
    
    MX25L6406_Read(&Parameter_Storage_Status,0*4096,1);
    printf("%d\r\n",Parameter_Storage_Status);
    if(Parameter_Storage_Status == 255)    //    ��flashδд���ʱ����Ĭ�ϲ���д��
    {    
        MX25L6406_Erase(1*4096,0x20);    //����   
        MX25L6406_Erase(13*4096,0x20);   //����
        MX25L6406_Erase(14*4096,0x20);   //����   
        MX25L6406_Erase(15*4096,0x20);   //����
        MX25L6406_Erase(16*4096,0x20);   //���� 
        MX25L6406_Erase(17*4096,0x20);   //����         
            
        SPI_Flash_Write(&Operator_name_length,10*4096,1);
        SPI_Flash_Write(&Equipment_Number_length,2*4096,1);
        SPI_Flash_Write(&Device_name_length,3*4096,1);
        SPI_Flash_Write(&Factory_Number_length,4*4096,1);
        SPI_Flash_Write((u8*)&Spanner_Number,5*4096,2);
        MX25L6406_WriteU16(Max_Angle,1*5120); //�������ƽǶȵ�flash 
        SPI_Flash_Write(&Gear_length,8*4096,1);       
        for(i = 0;i < Gear_length;i++)
        {  
            Gears_data[i].Levels = G_level[i];
            Gears_data[i].parameter = G_data[i];
            SPI_Flash_Write((u8*)&Gears_data[i],8*4096+2+i*sizeof(Simple1),sizeof(Simple1));   //������ֲ���
        }
        for(i = 0;i < 37;i++)
        {  
            SPI_Flash_Write((u8*)&BoltType,11*4096+i*sizeof(Bolt),sizeof(Bolt));
        }
                
        MX25L6406_WriteU32(TargetTorque_value,13*4096); 
        SPI_Flash_Write((u8*)&Set_Target_Gear,12*4096+20,1);        
        MX25L6406_WriteU16(Set_Target_Angle,14*4096); 
        MX25L6406_WriteU32(Set_SensorTorque,15*4096);
        MX25L6406_WriteU16(current_screen_id,16*4096); 
        MX25L6406_WriteU32(RecorddataCount,17*4096); 
        SPI_Flash_Write((u8*)&Set_Language_Type,12*4096+45,1);
        SPI_Flash_Write((u8*)&Set_Chart_Standard_Type,12*4096+50,1);
        SPI_Flash_Write((u8*)&Set_ProductStandard_Type,12*4096+55,1);
        SPI_Flash_Write((u8*)&Set_Torque_Unit,12*4096+60,1);
        SPI_Flash_Write((u8*)&Set_Pressure_Unit_Type,12*4096+75,1);
        SPI_Flash_Write((u8*)&Set_Bolt_Diameter,12*4096+80,1);
        SPI_Flash_Write((u8*)&Set_Bolt_Garder,12*4096+85,1);
        Parameter_Storage_Status = 1;       //��־λ�����ı䲢����flash         
        SPI_Flash_Write(&Parameter_Storage_Status,0*4096,1);    //0����     ��Ŵ洢״̬��־λ 
    }
    else
    {
        //������Ա����
        MX25L6406_Read(&Operator_name_length,10*4096,1);
        MX25L6406_Read(Operator_name,10*4096+1,Operator_name_length);
        printf("%s\r\n",Operator_name);
        
        //�û��豸���
        MX25L6406_Read(&Equipment_Number_length,2*4096,1);
        MX25L6406_Read(Set_Equipment_Number,2*4096+1,Equipment_Number_length);
        printf("%s\r\n",Set_Equipment_Number);
        
        //�û��豸����
        MX25L6406_Read(&Device_name_length,3*4096,1);  
        MX25L6406_Read(Device_name,3*4096+1,Device_name_length);
        printf("%s\r\n",Device_name);

        //���߳������
        MX25L6406_Read(&Factory_Number_length,4*4096,1);   
        MX25L6406_Read(Set_Factory_Number,4*4096+1,Factory_Number_length);
        printf("%s\r\n",Set_Factory_Number);

        //����ϵ��
        Spanner_Number = MX25L6406_ReadU16(5*4096);
//        printf("Spanner_Number:%d\r\n",Spanner_Number);
        for(i = 0;i < Spanner_Number;i++)
        {        
            MX25L6406_Read((u8*)&SpannerType[i],6*4096+i*sizeof(Spanner),sizeof(Spanner)); //��ȡ��λ��Ӧ���� 
#if FLASH_INFO            
            printf("Spanner_Number:%d\r\n",i);
            printf("�����ͺ�:%s\r\n",SpannerType[i].type);
            printf("����ϵ��:%d\r\n",SpannerType[i].parameter);
            printf("���������ֵ:%d\r\n",SpannerType[i].Sensor_parameter);        
            printf("Ӣ��ֱ����:%d\r\n",SpannerType[i].Diameter_Imperial);
            printf("Ӣ�Ƶȼ���:%d\r\n",SpannerType[i].Grade_Imperial);
            printf("����ֱ����:%d\r\n",SpannerType[i].Diameter_Metric);    
            printf("���Ƶȼ���:%d\r\n",SpannerType[i].Grade_Metric);
            printf("����ʱ��:%d\r\n",SpannerType[i].Delay_time);
#endif
        } 

        //��λ�Ƕ�ֵ
        Max_Angle = MX25L6406_ReadU16(1*5120);
//        printf("Max_Angle:%d\r\n",Max_Angle);

        //��λ
        MX25L6406_Read(&Gear_length,8*4096,1);
        for(i = 0;i < Gear_length;i++)
        {       
            MX25L6406_Read((u8*)&Gears_data[i],8*4096+2+i*sizeof(Simple1),sizeof(Simple1)); //��ȡ��λ��Ӧ����
//            printf("%d\r\n",Gears_data[i].Levels);      
//            printf("%d\r\n",Gears_data[i].parameter);            
        }
        
        TargetTorque_value = MX25L6406_ReadU32(13*4096);
        MX25L6406_Read((u8*)&Set_Target_Gear,12*4096+20,1); 
        Set_Target_Angle = MX25L6406_ReadU16(14*4096);
        Set_SensorTorque = MX25L6406_ReadU32(15*4096);
        current_screen_id = MX25L6406_ReadU16(16*4096);
        RecorddataCount = MX25L6406_ReadU32(17*4096);
        MX25L6406_Read((u8*)&Set_Language_Type,12*4096+45,1); 
        MX25L6406_Read((u8*)&Set_Chart_Standard_Type,12*4096+50,1); 
        MX25L6406_Read((u8*)&Set_ProductStandard_Type,12*4096+55,1); 
        MX25L6406_Read((u8*)&Set_Torque_Unit,12*4096+60,1); 
        MX25L6406_Read((u8*)&Set_Pressure_Unit_Type,12*4096+75,1); 
        MX25L6406_Read((u8*)&Set_Bolt_Diameter,12*4096+80,1);
        MX25L6406_Read((u8*)&Set_Bolt_Garder,12*4096+85,1);
#if FLASH_INFO
        printf("TargetTorque_value:%d\r\n",TargetTorque_value);
        printf("Set_Target_Gear:%d\r\n",Set_Target_Gear);
        printf("Set_Target_Angle:%d\r\n",Set_Target_Angle);
        printf("Set_SensorTorque:%d\r\n",Set_SensorTorque);
        printf("current_screen_id:%d\r\n",current_screen_id);
        printf("Set_Language_Type:%d\r\n",Set_Language_Type);
        printf("Set_Chart_Standard_Type:%d\r\n",Set_Chart_Standard_Type);
        printf("Set_ProductStandard_Type:%d\r\n",Set_ProductStandard_Type);
        printf("Set_Torque_Unit:%d\r\n",Set_Torque_Unit);
        printf("Set_Pressure_Unit_Type:%d\r\n",Set_Pressure_Unit_Type);
        printf("Set_Bolt_Diameter:%d\r\n",Set_Bolt_Diameter);
        printf("Set_Bolt_Garder:%d\r\n",Set_Bolt_Garder);
        printf("RecorddataCount:%d\r\n",RecorddataCount);
#endif
        
        
    }
}
//u16 Flash_count = 0;
//�洢����
void Flash_Model_parameters(u8* Storage)
{
    u8  i;
     u8 AutoTemp = 0;
    u32 AutoTimes = 0;
    switch(*Storage)
    {
        case 1:
            //Ť�����ô洢
            MX25L6406_Erase(5*4096,0x20);   //����         
            MX25L6406_WriteU16(Spanner_Number,5*4096); //������ָ�����flash
            memset(&BoltType,0,sizeof(Bolt)); 
            for(i = 0;i < Spanner_Number;i++)
            {   
                OSTimeDly(2);
                SPI_Flash_Write((u8*)&SpannerType[i],6*4096+i*sizeof(Spanner),sizeof(Spanner));
                SPI_Flash_Write((u8*)&BoltType,11*4096+i*sizeof(Bolt),sizeof(Bolt)); 
            }
            *Storage = 0;
            break;
        case 2:
            //������Ŵ洢
            MX25L6406_Erase(4*4096,0x20);   //����         ��4���� 
            MX25L6406_Write(&Factory_Number_length,4*4096,1); //������ָ�����flash 
            MX25L6406_Write(Set_Factory_Number,4*4096+1,Factory_Number_length);   //����������
            *Storage = 0;
            break;
        case 3:
            //�û��豸���ƴ洢
            MX25L6406_Erase(3*4096,0x20);   //����         ��3���� 
            MX25L6406_Write(&Device_name_length,3*4096,1); //������ָ�����flash 
            MX25L6406_Write(Device_name,3*4096+1,Device_name_length);   //�����û��豸����
            *Storage = 0;
            break;
        case 4:
            //�豸��Ŵ洢
            MX25L6406_Erase(2*4096,0x20);   //����         ��2����
            MX25L6406_Write(&Equipment_Number_length,2*4096,1); //������ָ�����flash 
            MX25L6406_Write(Set_Equipment_Number,2*4096+1,Equipment_Number_length);   //�����豸���
            *Storage = 0;
            break;
        case 5:
            //������Ա���ƴ洢
            MX25L6406_Erase(10*4096,0x20);   //����         ��0����
            MX25L6406_Write(&Operator_name_length,10*4096,1); //���������Ա���ȵ�flash 
            MX25L6406_Write(Operator_name,10*4096+1,Operator_name_length);   //���������Ա����
            *Storage = 0;
            break;
        case 6:
            //�Ƕ����ô洢
            MX25L6406_Erase(1*4096,0x20);   //����         ��1����
            MX25L6406_WriteU16(Max_Angle,1*5120); //�������ƽǶȵ�flash
            *Storage = 0;
            break;
        case 7:
            //��λ���ô洢
            MX25L6406_Erase(8*4096,0x20);   //����        
            MX25L6406_Write(&Gear_length,8*4096,1); //���������flash 
            for(i = 0;i < Gear_length;i++)
            { 
                OSTimeDly(2);
                MX25L6406_Write((u8*)&Gears_data[i],8*4096+2+i*sizeof(Simple1),sizeof(Simple1));   //�����������
            }
            *Storage = 0;
            break;
        case 8:
            //���������ô洢
            for(i = 0;i < Spanner_Number;i++)
            { 
                 OSTimeDly(2);
                 SPI_Flash_Write((u8*)&SpannerType[i],6*4096+i*sizeof(Spanner),sizeof(Spanner));
            }
            *Storage = 0;
        break;
        case 9:     
            if(RecorddataCount > 70000)
            {
                RecorddataCount = 0;  //����70000�����ݴ��¼�¼
            }
            MX25L6406_Erase(17*4096,0x20);   //����         
            MX25L6406_WriteU32(RecorddataCount,17*4096); 
            if(DriveControl_status == 1)
            { 
                if(Set_Control_Mode ==4)
                {
                    SPI_Flash_Write(Databuff,20*4096+(RecorddataCount-1)*109,109);
                    OSTimeDly(4);
                }
                else
                {
                    AutoTemp = AutoNumber;
    //                printf( "AutoTemp:%d\r\n",AutoTemp );
    //                printf( "RecorddataCount:%d\r\n",RecorddataCount );
                    AutoTimes = RecorddataCount - AutoTemp;

                    for(i = 0;i< AutoTemp;i++)
                    {
                        Set_ArrayCount(AutoDatabuff[i],AutoTimes+i);
                        SPI_Flash_Write(AutoDatabuff[i],20*4096+(AutoTimes+i)*109,109); 
                        OSTimeDly(4);
    //                    printf( "AUTO\r\n" );
                    } 
                    AutoNumber = 0;  
                }                    
            }
            else
            {
                SPI_Flash_Write(Databuff,20*4096+(RecorddataCount-1)*109,109);
                OSTimeDly(4);
//                 printf( "HAND\r\n" );
            }
            *Storage = 0;
        break;
        case 10:
            //��˨���ô洢
            for(i = 0;i < Spanner_Number;i++)//������˨ֱ���ȼ���
            {   
                OSTimeDly(2);
                SPI_Flash_Write((u8*)&SpannerType[i],6*4096+i*sizeof(Spanner),sizeof(Spanner));
            }
            SPI_Flash_Write((u8*)&BoltType,11*4096+Blot_addressNumber*sizeof(Bolt),sizeof(Bolt));  //�洢��˨Ť��    2200 
            *Storage = 0;
        break;
        case 11:
            MX25L6406_Erase(13*4096,0x20);   //����         
            MX25L6406_WriteU32(TargetTorque_value,13*4096); 
            *Storage = 0;
        break;
        case 14:
            SPI_Flash_Write((u8*)&Set_Target_Gear,12*4096+20,1);
            *Storage = 0;
        break;
        case 15:
            MX25L6406_Erase(14*4096,0x20);   //����         
            MX25L6406_WriteU16(Set_Target_Angle,14*4096); 
            *Storage = 0;
        break;
        case 16:
            MX25L6406_Erase(15*4096,0x20);   //����         
            MX25L6406_WriteU32(Set_SensorTorque,15*4096); 
            *Storage = 0;
        break;  
        case 18:
            MX25L6406_Erase(16*4096,0x20);   //����         
            MX25L6406_WriteU16(current_screen_id,16*4096); 
            *Storage = 0;
        break;
        case 19:
            SPI_Flash_Write((u8*)&Set_Language_Type,12*4096+45,1);
            *Storage = 0;
        break;
        case 20:
            SPI_Flash_Write((u8*)&Set_Chart_Standard_Type,12*4096+50,1);
            *Storage = 0;
        break;
        case 21:
            SPI_Flash_Write((u8*)&Set_ProductStandard_Type,12*4096+55,1);
            *Storage = 0;
        break;
        case 22:
            SPI_Flash_Write((u8*)&Set_Torque_Unit,12*4096+60,1);
            *Storage = 0;
        break;
        case 25:
            SPI_Flash_Write((u8*)&Set_Pressure_Unit_Type,12*4096+75,1);
            *Storage = 0;
        break;
        case 26:
            SPI_Flash_Write((u8*)&Set_Bolt_Diameter,12*4096+80,1);
            *Storage = 0;
        break;
        case 27:
            SPI_Flash_Write((u8*)&Set_Bolt_Garder,12*4096+85,1);
            *Storage = 0;
        break;       
    }

       
}
u32 RecorddataCount = 0;   //��¼����
u8 Databuff[120] = {0};
u8 Automatic_Recording_times  = 0;
u8 DriveControl_status = 0;
/*! 
*  \brief     ��¼ʹ�����ݣ�U�̵�������λ��������
*/
void Data_Record_Storage(u8 *RecordDatabuf,u32 DataCount,u8 length)
{   
    u32 data = 0; 
    u32 Torque_data = 0;
    u32 Target_Torque_data = 0;
    u32 TorqueDifferenceValue = 0;
    u16 value = 0;
    u8 i;
    float Record_PressValue = 0;
    u16 Record_AngleValue = 0;
    memset(RecordDatabuf,0,length); //������� 
    Record_PressValue = Collect_Pressure;
    Record_AngleValue = Collect_Angle; //
    //��¼���
    RecordDatabuf[0] = (DataCount+1)>>24;
    RecordDatabuf[1] = (DataCount+1)>>16;
    RecordDatabuf[2] = (DataCount+1)>>8;
    RecordDatabuf[3] = (DataCount+1);  
    //�����ͺ�����10�ֽ� 
    strncpy((char*)&RecordDatabuf[4],(char*)SpannerType[Set_ProductStandard_Type].type,10);  
    //���߳������8�ֽ�
    strncpy((char*)&RecordDatabuf[14],(char*)Set_Factory_Number,8);
    //����7�ֽ�    8
    RecordDatabuf[22] = calendar.w_year>>8;
    RecordDatabuf[23] = calendar.w_year;
    RecordDatabuf[24] = calendar.w_month;
    RecordDatabuf[25] = calendar.w_date;
    RecordDatabuf[26] = calendar.hour;
    RecordDatabuf[27] = calendar.min;
    RecordDatabuf[28] = calendar.sec;
    //�û��豸����16�ֽ�  
    strncpy((char*)&RecordDatabuf[29],(char*)Device_name,16);
    //�û��豸���8�ֽ�   
    strncpy((char*)&RecordDatabuf[45],(char*)Set_Equipment_Number,8);
    //������Ա16�ֽ�    
    strncpy((char*)&RecordDatabuf[53],(char*)Operator_name,16);
    //ѹ����λ
    RecordDatabuf[69] = Set_Pressure_Unit_Type;
    //Ť�ص�λ
    RecordDatabuf[70] = Set_Torque_Unit;
   //��Ӧѹ��2�ֽ�
//  value = Get_TargetPressure(TargetTorque_value,Spanner_Coefficien,Set_Pressure_Unit_Type);//Ŀ��Ť�ض�Ӧ��ѹ��
    value  = Get_Pressure(Record_PressValue,Set_Pressure_Unit_Type);//ʵʱѹ��
    RecordDatabuf[71] = value>>8;
    RecordDatabuf[72] = value;
    switch(Set_Control_Mode)        //ѡ��ģʽ
    {
        case 1:                                                                 //Ť��ģʽ             
           //Ŀ��Ť��4�ֽ�  
            Target_Torque_data = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);
            RecordDatabuf[73] = Target_Torque_data>>24;
            RecordDatabuf[74] = Target_Torque_data>>16;
            RecordDatabuf[75] = Target_Torque_data>>8;
            RecordDatabuf[76] = Target_Torque_data;
            //ʵ��Ť��4�ֽ�
            Torque_data = Get_ActualTorque(Set_Torque_Unit,Record_PressValue,Spanner_Coefficien);
            RecordDatabuf[77] = Torque_data>>24;
            RecordDatabuf[78] = Torque_data>>16;
            RecordDatabuf[79] = Torque_data>>8;
            RecordDatabuf[80] = Torque_data;
            //ѡ��ģʽ1�ֽ�    92
            RecordDatabuf[106] = Set_Control_Mode;
            //������ʽ           0  �ֶ�     1  �Զ�
            RecordDatabuf[107] = DriveControl_status;
            //š�����1�ֽ�      0  δš��    1  š�� 
            TorqueDifferenceValue = Get_ActualTorque(Set_Torque_Unit,Record_PressValue *0.03,Spanner_Coefficien);  //Ť�ء�3%��ֵ
            if(DriveControl_status == 1)
            {
                if(Torque_data > Target_Torque_data - TorqueDifferenceValue &&  Torque_data < Target_Torque_data + TorqueDifferenceValue)RecordDatabuf[108] = 1;   
                else RecordDatabuf[108] = 0;
            }
            else
            {
                RecordDatabuf[108] = 1;
            }
        break;
        case 2:                                                                     //��˨ģʽ
           //Ŀ��Ť��4�ֽ� 
            Target_Torque_data = Get_TargetTorque(Set_Torque_Unit,BoltCoefficient_value); 
            RecordDatabuf[73] = Target_Torque_data>>24;
            RecordDatabuf[74] = Target_Torque_data>>16;
            RecordDatabuf[75] = Target_Torque_data>>8;
            RecordDatabuf[76] = Target_Torque_data;
            //ʵ��Ť��4�ֽ�         
            Torque_data = Get_ActualTorque(Set_Torque_Unit,Record_PressValue,Spanner_Coefficien);
            RecordDatabuf[77] = Torque_data>>24;
            RecordDatabuf[78] = Torque_data>>16;
            RecordDatabuf[79] = Torque_data>>8;
            RecordDatabuf[80] = Torque_data;
            //��˨
            switch(Set_Chart_Standard_Type)
            {
            case Imperial :              
                strncpy((char*)&RecordDatabuf[81],(char*)BoltType.Imperial_Diameter[Set_Bolt_Diameter],10); //��˨���10�ֽ�    63
                strncpy((char*)&RecordDatabuf[91],(char*)BoltType.Imperial_Grade[Set_Bolt_Garder],10);//��˨ǿ�ȵȼ�10�ֽ�   73
            break;
            case Metric :  
                strncpy((char*)&RecordDatabuf[81],(char*)BoltType.Metric_Diameter[Set_Bolt_Diameter],10);                 
                strncpy((char*)&RecordDatabuf[91],(char*)BoltType.Metric_Grade[Set_Bolt_Garder],10);
            break;
            } 
            //ѡ��ģʽ1�ֽ�    92
            RecordDatabuf[106] = Set_Control_Mode;
            //������ʽ
            RecordDatabuf[107] = DriveControl_status;
            //š�����1�ֽ�   95
            TorqueDifferenceValue = Get_ActualTorque(Set_Torque_Unit,Record_PressValue *0.03,Spanner_Coefficien);  //Ť�ء�3%��ֵ
            if(DriveControl_status == 1)
            {
                if(Torque_data > Target_Torque_data - TorqueDifferenceValue &&  Torque_data < Target_Torque_data + TorqueDifferenceValue)RecordDatabuf[108] = 1;   
                else RecordDatabuf[108] = 0;
            }
            else
            {
                RecordDatabuf[108] = 1;
            }
        break;
        case 3:                                                                         //��λģʽ
           //Ŀ��Ť��4�ֽ�  
            Target_Torque_data = Get_GearTorque(Set_Torque_Unit,GearTorque_value);               
            RecordDatabuf[73] = Target_Torque_data>>24;
            RecordDatabuf[74] = Target_Torque_data>>16;
            RecordDatabuf[75] = Target_Torque_data>>8;
            RecordDatabuf[76] = Target_Torque_data;
            //ʵ��Ť��4�ֽ�      
            Torque_data = Get_ActualTorque(Set_Torque_Unit,Record_PressValue,Spanner_Coefficien);
            RecordDatabuf[77] = Torque_data>>24;
            RecordDatabuf[78] = Torque_data>>16;
            RecordDatabuf[79] = Torque_data>>8;
            RecordDatabuf[80] = Torque_data;
            //��λ1�ֽ�        87
            RecordDatabuf[105] = Gears_data[Set_Target_Gear].Levels;
            //ѡ��ģʽ1�ֽ�    92
            RecordDatabuf[106] = Set_Control_Mode;
            //������ʽ
            RecordDatabuf[107] = DriveControl_status;
            //š�����1�ֽ�   95
            TorqueDifferenceValue = Get_ActualTorque(Set_Torque_Unit,Record_PressValue *0.03,Spanner_Coefficien);  //Ť�ء�3%��ֵ
            if(DriveControl_status == 1)
            {
                if(Torque_data > Target_Torque_data - TorqueDifferenceValue &&  Torque_data < Target_Torque_data + TorqueDifferenceValue)RecordDatabuf[108] = 1;   
                else RecordDatabuf[108] = 0;
            }
            else
            {
                RecordDatabuf[108] = 1;
            }
        break;
        case 4:                                                                             //�Ƕ�ģʽ
           //Ŀ��Ť��4�ֽ�             
            Target_Torque_data = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);        
            RecordDatabuf[73] = Target_Torque_data>>24;
            RecordDatabuf[74] = Target_Torque_data>>16;
            RecordDatabuf[75] = Target_Torque_data>>8;
            RecordDatabuf[76] = Target_Torque_data;
            //ʵ��Ť��4�ֽ�    
            Torque_data = Get_ActualTorque(Set_Torque_Unit,Record_PressValue,Spanner_Coefficien);
            RecordDatabuf[77] = Torque_data>>24;
            RecordDatabuf[78] = Torque_data>>16;
            RecordDatabuf[79] = Torque_data>>8;
            RecordDatabuf[80] = Torque_data;
            //Ŀ��Ƕ�2�ֽ�                    
            RecordDatabuf[101] = Set_Target_Angle>>8;
            RecordDatabuf[102] = Set_Target_Angle;
            //ʵ�ʽǶ�2�ֽ�      
            RecordDatabuf[103] = Record_AngleValue>>8;
            RecordDatabuf[104] = Record_AngleValue;
            //ѡ��ģʽ1�ֽ�    92
            RecordDatabuf[106] = Set_Control_Mode;
            //������ʽ
            RecordDatabuf[107] = DriveControl_status;
            //š�����1�ֽ�   95
            TorqueDifferenceValue = Get_ActualTorque(Set_Torque_Unit,Record_PressValue *0.03,Spanner_Coefficien);  //Ť�ء�3%��ֵ
            if(DriveControl_status == 1)
            {  
                if(Record_AngleValue >=Set_Target_Angle)RecordDatabuf[108] = 1;  
                else RecordDatabuf[108] = 0;
            }
            else
            {
                RecordDatabuf[108] = 1;
            }
        break;
        case 5:                                                                     //������ģʽ
           //Ŀ��Ť��4�ֽ�
            Target_Torque_data = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);  
            RecordDatabuf[73] = Target_Torque_data>>24;
            RecordDatabuf[74] = Target_Torque_data>>16;
            RecordDatabuf[75] = Target_Torque_data>>8;
            RecordDatabuf[76] = Target_Torque_data;
            //ʵ��Ť��4�ֽ�    
            Torque_data = Get_ActualTorque(Set_Torque_Unit,Record_PressValue,Spanner_Coefficien);
            RecordDatabuf[77] = Torque_data>>24;
            RecordDatabuf[78] = Torque_data>>16;
            RecordDatabuf[79] = Torque_data>>8;
            RecordDatabuf[80] = Torque_data;
            //ѡ��ģʽ1�ֽ�    92
            RecordDatabuf[106] = Set_Control_Mode;
            //������ʽ
            RecordDatabuf[107] = DriveControl_status;
            //š�����1�ֽ�   95
            TorqueDifferenceValue = Get_ActualTorque(Set_Torque_Unit,Record_PressValue *0.03,Spanner_Coefficien);  //Ť�ء�3%��ֵ
            if(DriveControl_status == 1)
            {
                if(Torque_data > Target_Torque_data - TorqueDifferenceValue &&  Torque_data < Target_Torque_data + TorqueDifferenceValue)RecordDatabuf[108] = 1;   
                else RecordDatabuf[108] = 0;
            }
            else
            {
                RecordDatabuf[108] = 1;
            }
        break;
    }
#if 0
    //��ӡ��ǰ����
    for(i = 0;i < 109;i++)
    {
         printf("%c",RecordDatabuf[i]);
        
    }
     printf("\r\n");
#endif

}
