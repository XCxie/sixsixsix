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
8M:1024*8K 128块,2048扇区,32768页
页:256B
扇区:4K,
块:64K

**************************************************************/
void MX25L6406_Init(void)
{
	/*初始化的SPI，GPIO结构体*/  
	 SPI_InitTypeDef  SPI_InitStructure;  
	 GPIO_InitTypeDef GPIO_InitStructure;  
		 
	 RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1,  ENABLE); /*在RCC_APB2ENB中使能SPI1时钟(位14)*/  
	 RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,  ENABLE);/*因为与SPI1相关的4个引脚和GPIOA相*/  
																																																											/*关，GPIOB时钟(位3)，这句现在还不 */  
																																																													 /*确定要不要，待调试时再确定              */  
	/*上面这一句是必须的，因为CS脚是当做GPIO来使用的，2011-01-30调试*/  
																																																												
	/*配置SPI_FLASH_CLK(PA5)，SPI_FLASH_MISO(PA6)，SPI_FLASH_MOSI(PA7)*/  
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;  
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;                    /*复用功能推挽式输出*/  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;  
	GPIO_Init( GPIOA, &GPIO_InitStructure);  
		
	/*配置输入SPI_FLASH_CS(PA2)*/  
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;  
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;   /*推挽式输出*/  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;  
	GPIO_Init( GPIOA, &GPIO_InitStructure);  

    MX25L6406_CS_H;             /*不选flash*/  


	/* SPI1配置 增加于2010-01-13*/  
	/* 注意:  在SPI_NSS_Soft模式下，SSI位决定了NSS引脚上(PA4)的电平， 
		*            而SSM=1时释放了NSS引脚，NSS引脚可以用作GPIO口*/  
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   /*双线双向全双工BIDI MODE=0*/  
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                                 /*SSI位为1，MSTR位为1*/  
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                              /*SPI发送接收8位帧结构*/  
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;                                     /*CPOL=1,CPHA=1，模式3*/  
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;  
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                                          /*内部NSS信号由SSI位控制，SSM=1*/  
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;    /*波特率预分频值为2*/  
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                                       /*数据传输从MSB位开始*/  
	SPI_InitStructure.SPI_CRCPolynomial = 7;                                                      /*复位默认值*/  
	SPI_Init(SPI1, &SPI_InitStructure);  

	SPI_Cmd(SPI1,ENABLE);       /*使能SPI2*/  
}

/*功能:       SPI发送一个字节 
  *参数:       send_data:   待发送的字节 
  *返回:       无*/  
u8 SPI1_ReadWriteByte(u8 send_data)  
{  
    /*检查Busy位，SPI的SR中的位7，SPI通信是否为忙，直到不忙跳出*/  
    //while( SET==SPI_I2S_GetFlagStatus(SPI_SELECT, SPI_I2S_FLAG_BSY));  
      
    /*检查TXE位，SPI的SR中的位1，发送缓冲器是否为空，直到空跳出*/  
    while( RESET==SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));  
  
    SPI_I2S_SendData(SPI1, send_data);                        /*发送一个字节*/  
      
    /*发送数据后再接收一个字节*/  
    while( RESET==SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) );  
    return( SPI_I2S_ReceiveData(SPI1) );  
      
}  

//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)  		   
u8 SPI_FLASH_BUF[4096];
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    

	secpos=WriteAddr/4096;//扇区地址 0~511 for w25x16
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   

	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		MX25L6406_Read(SPI_FLASH_BUF,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			MX25L6406_Erase(secpos*4096,0x20);//擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制
			{
				SPI_FLASH_BUF[i+secoff]=pBuffer[i];	  
			}
                SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);//写入整个扇区  

		}else SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumByteToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	pBuffer+=secremain;  //指针偏移
			WriteAddr+=secremain;//写地址偏移	   
		   	NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>4096)secremain=4096;	//下一个扇区还是写不完
			else secremain=NumByteToWrite;			//下一个扇区可以写完了
		}	 
	};	 	 
}

//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		MX25L6406_Write(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	};	    
} 


//写使能指令（0x06）
void MX25L6406_Write_EN(void)
{
	//CS拉低
	MX25L6406_CS_L;
	//发送指令0x06
	SPI1_ReadWriteByte(0x06);
	//CS拉高
	MX25L6406_CS_H;
}
//读状态寄存器指令（0x05）
void MX25L6406_Read_status(void)
{
	u8 data;
	//CS拉低
	MX25L6406_CS_L;
	//发送指令0x05
	SPI1_ReadWriteByte(0x05);
	//反复接收并检测BUSY位的状态，直到不忙（0）
	do
	{
		data = SPI1_ReadWriteByte(0x00);//发送一个无效数据，目的是接收
	}while(data);
	//CS拉高
	MX25L6406_CS_H;
}

//读数据指令（0x03）	
void MX25L6406_Read(u8 *data,u32 addr,u32 n)
{
	u32 i;
	//CS拉低
	MX25L6406_CS_L;
	//发送指令0x03
	SPI1_ReadWriteByte(0x03);
	//发送24位地址（用户想要从该地址开始读取数据的地址）,先发送高位
	SPI1_ReadWriteByte(addr>>16);
	SPI1_ReadWriteByte(addr>>8);
	SPI1_ReadWriteByte(addr);
	//主机n次接收（读取）数据
	for(i=0;i<n;i++)
	{
		*data = SPI1_ReadWriteByte(0x00);
		data++;
	}
	//CS拉高
	MX25L6406_CS_H;
}
//页编程（写数据）指令（0x02）
void MX25L6406_Write_data(u32 addr,u8 data)
{
	//写使能
	MX25L6406_Write_EN();
	//CS拉低
	MX25L6406_CS_L;
	//发送指令0x02
	SPI1_ReadWriteByte(0x02);
	//发送24位地址（用户想要从该地址开始写入数据的地址）
	SPI1_ReadWriteByte(addr>>16);
	SPI1_ReadWriteByte(addr>>8);
	SPI1_ReadWriteByte(addr);
	//主机发送（写入）一个数据
	SPI1_ReadWriteByte(data);
	//CS拉高
	MX25L6406_CS_H;
	//读取BUSY位判定操作是否完毕
	MX25L6406_Read_status();
}
//扇区擦除指令（0x20）
//块擦除指令（0xD8）
//整个芯片擦除指令（0xC7）
void MX25L6406_Erase(u32 addr,u8 cmd)
{
	//写使能
	MX25L6406_Write_EN();
	//CS拉低
	MX25L6406_CS_L;
	//发送指令
	SPI1_ReadWriteByte(cmd);
	if(cmd==Sector || cmd==Block)
	{
		//发送24位地址（用户想要从该地址开始写入数据的地址）
		SPI1_ReadWriteByte(addr>>16);
		SPI1_ReadWriteByte(addr>>8);
		SPI1_ReadWriteByte(addr);
	}
	//CS拉高
	MX25L6406_CS_H;
	//读取BUSY位判定操作是否完毕
	MX25L6406_Read_status();
}


//写多字节
void MX25L6406_Write(u8 *ftrBuffer,u32 addr, u32 ftrLen)
{
	u32 i=0;
	//写使能
	MX25L6406_Write_EN();
	//CS拉低
	MX25L6406_CS_L;
	//发送指令0x02
	SPI1_ReadWriteByte(0x02);
	//发送24位地址（用户想要从该地址开始写入数据的地址）
	SPI1_ReadWriteByte(addr>>16);
	SPI1_ReadWriteByte(addr>>8);
	SPI1_ReadWriteByte(addr);
	//主机发送（写入）一个数据
	for(i=0;i<ftrLen;i++)
	SPI1_ReadWriteByte(ftrBuffer[i]);
	//CS拉高
	MX25L6406_CS_H;
	//读取BUSY位判定操作是否完毕
	MX25L6406_Read_status();
}
//16位数据读写
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

//32位数据读写
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
u8 Parameter_Storage_Status = 0;  //是否为第一次存储
//u8 RecordDatabuf[120] = {0};
//读取参数
void  Get_Flash_Parameters(void)
{
    u16 i,j;
    u32 temp;
    u8 buf[100] = {0};
    u8 G_level[7] = {1,2,3,4,5,6,7};
    u32 G_data[7] = {100000,200000,300000,400000,500000,600000,700000};/*     档位默认参数    */
    
    MX25L6406_Read(&Parameter_Storage_Status,0*4096,1);
    printf("%d\r\n",Parameter_Storage_Status);
    if(Parameter_Storage_Status == 255)    //    当flash未写入过时，将默认参数写入
    {    
        MX25L6406_Erase(1*4096,0x20);    //擦除   
        MX25L6406_Erase(13*4096,0x20);   //擦除
        MX25L6406_Erase(14*4096,0x20);   //擦除   
        MX25L6406_Erase(15*4096,0x20);   //擦除
        MX25L6406_Erase(16*4096,0x20);   //擦除 
        MX25L6406_Erase(17*4096,0x20);   //擦除         
            
        SPI_Flash_Write(&Operator_name_length,10*4096,1);
        SPI_Flash_Write(&Equipment_Number_length,2*4096,1);
        SPI_Flash_Write(&Device_name_length,3*4096,1);
        SPI_Flash_Write(&Factory_Number_length,4*4096,1);
        SPI_Flash_Write((u8*)&Spanner_Number,5*4096,2);
        MX25L6406_WriteU16(Max_Angle,1*5120); //保存限制角度到flash 
        SPI_Flash_Write(&Gear_length,8*4096,1);       
        for(i = 0;i < Gear_length;i++)
        {  
            Gears_data[i].Levels = G_level[i];
            Gears_data[i].parameter = G_data[i];
            SPI_Flash_Write((u8*)&Gears_data[i],8*4096+2+i*sizeof(Simple1),sizeof(Simple1));   //保存扳手参数
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
        Parameter_Storage_Status = 1;       //标志位发生改变并存入flash         
        SPI_Flash_Write(&Parameter_Storage_Status,0*4096,1);    //0扇区     存放存储状态标志位 
    }
    else
    {
        //操作人员名称
        MX25L6406_Read(&Operator_name_length,10*4096,1);
        MX25L6406_Read(Operator_name,10*4096+1,Operator_name_length);
        printf("%s\r\n",Operator_name);
        
        //用户设备编号
        MX25L6406_Read(&Equipment_Number_length,2*4096,1);
        MX25L6406_Read(Set_Equipment_Number,2*4096+1,Equipment_Number_length);
        printf("%s\r\n",Set_Equipment_Number);
        
        //用户设备名称
        MX25L6406_Read(&Device_name_length,3*4096,1);  
        MX25L6406_Read(Device_name,3*4096+1,Device_name_length);
        printf("%s\r\n",Device_name);

        //工具出厂编号
        MX25L6406_Read(&Factory_Number_length,4*4096,1);   
        MX25L6406_Read(Set_Factory_Number,4*4096+1,Factory_Number_length);
        printf("%s\r\n",Set_Factory_Number);

        //扳手系数
        Spanner_Number = MX25L6406_ReadU16(5*4096);
//        printf("Spanner_Number:%d\r\n",Spanner_Number);
        for(i = 0;i < Spanner_Number;i++)
        {        
            MX25L6406_Read((u8*)&SpannerType[i],6*4096+i*sizeof(Spanner),sizeof(Spanner)); //获取档位对应参数 
#if FLASH_INFO            
            printf("Spanner_Number:%d\r\n",i);
            printf("扳手型号:%s\r\n",SpannerType[i].type);
            printf("扳手系数:%d\r\n",SpannerType[i].parameter);
            printf("传感器最大值:%d\r\n",SpannerType[i].Sensor_parameter);        
            printf("英制直径数:%d\r\n",SpannerType[i].Diameter_Imperial);
            printf("英制等级数:%d\r\n",SpannerType[i].Grade_Imperial);
            printf("公制直径数:%d\r\n",SpannerType[i].Diameter_Metric);    
            printf("公制等级数:%d\r\n",SpannerType[i].Grade_Metric);
            printf("回油时间:%d\r\n",SpannerType[i].Delay_time);
#endif
        } 

        //限位角度值
        Max_Angle = MX25L6406_ReadU16(1*5120);
//        printf("Max_Angle:%d\r\n",Max_Angle);

        //档位
        MX25L6406_Read(&Gear_length,8*4096,1);
        for(i = 0;i < Gear_length;i++)
        {       
            MX25L6406_Read((u8*)&Gears_data[i],8*4096+2+i*sizeof(Simple1),sizeof(Simple1)); //获取档位对应参数
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
//存储参数
void Flash_Model_parameters(u8* Storage)
{
    u8  i;
     u8 AutoTemp = 0;
    u32 AutoTimes = 0;
    switch(*Storage)
    {
        case 1:
            //扭矩设置存储
            MX25L6406_Erase(5*4096,0x20);   //擦除         
            MX25L6406_WriteU16(Spanner_Number,5*4096); //保存扳手个数到flash
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
            //出厂编号存储
            MX25L6406_Erase(4*4096,0x20);   //擦除         第4扇区 
            MX25L6406_Write(&Factory_Number_length,4*4096,1); //保存扳手个数到flash 
            MX25L6406_Write(Set_Factory_Number,4*4096+1,Factory_Number_length);   //保存出厂编号
            *Storage = 0;
            break;
        case 3:
            //用户设备名称存储
            MX25L6406_Erase(3*4096,0x20);   //擦除         第3扇区 
            MX25L6406_Write(&Device_name_length,3*4096,1); //保存扳手个数到flash 
            MX25L6406_Write(Device_name,3*4096+1,Device_name_length);   //设置用户设备名称
            *Storage = 0;
            break;
        case 4:
            //设备编号存储
            MX25L6406_Erase(2*4096,0x20);   //擦除         第2扇区
            MX25L6406_Write(&Equipment_Number_length,2*4096,1); //保存扳手个数到flash 
            MX25L6406_Write(Set_Equipment_Number,2*4096+1,Equipment_Number_length);   //保存设备编号
            *Storage = 0;
            break;
        case 5:
            //操作人员名称存储
            MX25L6406_Erase(10*4096,0x20);   //擦除         第0扇区
            MX25L6406_Write(&Operator_name_length,10*4096,1); //保存操作人员长度到flash 
            MX25L6406_Write(Operator_name,10*4096+1,Operator_name_length);   //保存操作人员名称
            *Storage = 0;
            break;
        case 6:
            //角度设置存储
            MX25L6406_Erase(1*4096,0x20);   //擦除         第1扇区
            MX25L6406_WriteU16(Max_Angle,1*5120); //保存限制角度到flash
            *Storage = 0;
            break;
        case 7:
            //档位设置存储
            MX25L6406_Erase(8*4096,0x20);   //擦除        
            MX25L6406_Write(&Gear_length,8*4096,1); //保存个数到flash 
            for(i = 0;i < Gear_length;i++)
            { 
                OSTimeDly(2);
                MX25L6406_Write((u8*)&Gears_data[i],8*4096+2+i*sizeof(Simple1),sizeof(Simple1));   //保存扳手名称
            }
            *Storage = 0;
            break;
        case 8:
            //传感器设置存储
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
                RecorddataCount = 0;  //到达70000条数据从新记录
            }
            MX25L6406_Erase(17*4096,0x20);   //擦除         
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
            //螺栓设置存储
            for(i = 0;i < Spanner_Number;i++)//保存螺栓直径等级数
            {   
                OSTimeDly(2);
                SPI_Flash_Write((u8*)&SpannerType[i],6*4096+i*sizeof(Spanner),sizeof(Spanner));
            }
            SPI_Flash_Write((u8*)&BoltType,11*4096+Blot_addressNumber*sizeof(Bolt),sizeof(Bolt));  //存储螺栓扭矩    2200 
            *Storage = 0;
        break;
        case 11:
            MX25L6406_Erase(13*4096,0x20);   //擦除         
            MX25L6406_WriteU32(TargetTorque_value,13*4096); 
            *Storage = 0;
        break;
        case 14:
            SPI_Flash_Write((u8*)&Set_Target_Gear,12*4096+20,1);
            *Storage = 0;
        break;
        case 15:
            MX25L6406_Erase(14*4096,0x20);   //擦除         
            MX25L6406_WriteU16(Set_Target_Angle,14*4096); 
            *Storage = 0;
        break;
        case 16:
            MX25L6406_Erase(15*4096,0x20);   //擦除         
            MX25L6406_WriteU32(Set_SensorTorque,15*4096); 
            *Storage = 0;
        break;  
        case 18:
            MX25L6406_Erase(16*4096,0x20);   //擦除         
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
u32 RecorddataCount = 0;   //记录次数
u8 Databuff[120] = {0};
u8 Automatic_Recording_times  = 0;
u8 DriveControl_status = 0;
/*! 
*  \brief     记录使用数据（U盘导出或上位机导出）
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
    memset(RecordDatabuf,0,length); //清空数据 
    Record_PressValue = Collect_Pressure;
    Record_AngleValue = Collect_Angle; //
    //记录序号
    RecordDatabuf[0] = (DataCount+1)>>24;
    RecordDatabuf[1] = (DataCount+1)>>16;
    RecordDatabuf[2] = (DataCount+1)>>8;
    RecordDatabuf[3] = (DataCount+1);  
    //工具型号名称10字节 
    strncpy((char*)&RecordDatabuf[4],(char*)SpannerType[Set_ProductStandard_Type].type,10);  
    //工具出厂编号8字节
    strncpy((char*)&RecordDatabuf[14],(char*)Set_Factory_Number,8);
    //日期7字节    8
    RecordDatabuf[22] = calendar.w_year>>8;
    RecordDatabuf[23] = calendar.w_year;
    RecordDatabuf[24] = calendar.w_month;
    RecordDatabuf[25] = calendar.w_date;
    RecordDatabuf[26] = calendar.hour;
    RecordDatabuf[27] = calendar.min;
    RecordDatabuf[28] = calendar.sec;
    //用户设备名称16字节  
    strncpy((char*)&RecordDatabuf[29],(char*)Device_name,16);
    //用户设备编号8字节   
    strncpy((char*)&RecordDatabuf[45],(char*)Set_Equipment_Number,8);
    //操作人员16字节    
    strncpy((char*)&RecordDatabuf[53],(char*)Operator_name,16);
    //压力单位
    RecordDatabuf[69] = Set_Pressure_Unit_Type;
    //扭矩单位
    RecordDatabuf[70] = Set_Torque_Unit;
   //对应压力2字节
//  value = Get_TargetPressure(TargetTorque_value,Spanner_Coefficien,Set_Pressure_Unit_Type);//目标扭矩对应的压力
    value  = Get_Pressure(Record_PressValue,Set_Pressure_Unit_Type);//实时压力
    RecordDatabuf[71] = value>>8;
    RecordDatabuf[72] = value;
    switch(Set_Control_Mode)        //选定模式
    {
        case 1:                                                                 //扭矩模式             
           //目标扭矩4字节  
            Target_Torque_data = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);
            RecordDatabuf[73] = Target_Torque_data>>24;
            RecordDatabuf[74] = Target_Torque_data>>16;
            RecordDatabuf[75] = Target_Torque_data>>8;
            RecordDatabuf[76] = Target_Torque_data;
            //实际扭矩4字节
            Torque_data = Get_ActualTorque(Set_Torque_Unit,Record_PressValue,Spanner_Coefficien);
            RecordDatabuf[77] = Torque_data>>24;
            RecordDatabuf[78] = Torque_data>>16;
            RecordDatabuf[79] = Torque_data>>8;
            RecordDatabuf[80] = Torque_data;
            //选定模式1字节    92
            RecordDatabuf[106] = Set_Control_Mode;
            //工作方式           0  手动     1  自动
            RecordDatabuf[107] = DriveControl_status;
            //拧紧结果1字节      0  未拧紧    1  拧紧 
            TorqueDifferenceValue = Get_ActualTorque(Set_Torque_Unit,Record_PressValue *0.03,Spanner_Coefficien);  //扭矩±3%差值
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
        case 2:                                                                     //螺栓模式
           //目标扭矩4字节 
            Target_Torque_data = Get_TargetTorque(Set_Torque_Unit,BoltCoefficient_value); 
            RecordDatabuf[73] = Target_Torque_data>>24;
            RecordDatabuf[74] = Target_Torque_data>>16;
            RecordDatabuf[75] = Target_Torque_data>>8;
            RecordDatabuf[76] = Target_Torque_data;
            //实际扭矩4字节         
            Torque_data = Get_ActualTorque(Set_Torque_Unit,Record_PressValue,Spanner_Coefficien);
            RecordDatabuf[77] = Torque_data>>24;
            RecordDatabuf[78] = Torque_data>>16;
            RecordDatabuf[79] = Torque_data>>8;
            RecordDatabuf[80] = Torque_data;
            //螺栓
            switch(Set_Chart_Standard_Type)
            {
            case Imperial :              
                strncpy((char*)&RecordDatabuf[81],(char*)BoltType.Imperial_Diameter[Set_Bolt_Diameter],10); //螺栓规格10字节    63
                strncpy((char*)&RecordDatabuf[91],(char*)BoltType.Imperial_Grade[Set_Bolt_Garder],10);//螺栓强度等级10字节   73
            break;
            case Metric :  
                strncpy((char*)&RecordDatabuf[81],(char*)BoltType.Metric_Diameter[Set_Bolt_Diameter],10);                 
                strncpy((char*)&RecordDatabuf[91],(char*)BoltType.Metric_Grade[Set_Bolt_Garder],10);
            break;
            } 
            //选定模式1字节    92
            RecordDatabuf[106] = Set_Control_Mode;
            //工作方式
            RecordDatabuf[107] = DriveControl_status;
            //拧紧结果1字节   95
            TorqueDifferenceValue = Get_ActualTorque(Set_Torque_Unit,Record_PressValue *0.03,Spanner_Coefficien);  //扭矩±3%差值
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
        case 3:                                                                         //档位模式
           //目标扭矩4字节  
            Target_Torque_data = Get_GearTorque(Set_Torque_Unit,GearTorque_value);               
            RecordDatabuf[73] = Target_Torque_data>>24;
            RecordDatabuf[74] = Target_Torque_data>>16;
            RecordDatabuf[75] = Target_Torque_data>>8;
            RecordDatabuf[76] = Target_Torque_data;
            //实际扭矩4字节      
            Torque_data = Get_ActualTorque(Set_Torque_Unit,Record_PressValue,Spanner_Coefficien);
            RecordDatabuf[77] = Torque_data>>24;
            RecordDatabuf[78] = Torque_data>>16;
            RecordDatabuf[79] = Torque_data>>8;
            RecordDatabuf[80] = Torque_data;
            //档位1字节        87
            RecordDatabuf[105] = Gears_data[Set_Target_Gear].Levels;
            //选定模式1字节    92
            RecordDatabuf[106] = Set_Control_Mode;
            //工作方式
            RecordDatabuf[107] = DriveControl_status;
            //拧紧结果1字节   95
            TorqueDifferenceValue = Get_ActualTorque(Set_Torque_Unit,Record_PressValue *0.03,Spanner_Coefficien);  //扭矩±3%差值
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
        case 4:                                                                             //角度模式
           //目标扭矩4字节             
            Target_Torque_data = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);        
            RecordDatabuf[73] = Target_Torque_data>>24;
            RecordDatabuf[74] = Target_Torque_data>>16;
            RecordDatabuf[75] = Target_Torque_data>>8;
            RecordDatabuf[76] = Target_Torque_data;
            //实际扭矩4字节    
            Torque_data = Get_ActualTorque(Set_Torque_Unit,Record_PressValue,Spanner_Coefficien);
            RecordDatabuf[77] = Torque_data>>24;
            RecordDatabuf[78] = Torque_data>>16;
            RecordDatabuf[79] = Torque_data>>8;
            RecordDatabuf[80] = Torque_data;
            //目标角度2字节                    
            RecordDatabuf[101] = Set_Target_Angle>>8;
            RecordDatabuf[102] = Set_Target_Angle;
            //实际角度2字节      
            RecordDatabuf[103] = Record_AngleValue>>8;
            RecordDatabuf[104] = Record_AngleValue;
            //选定模式1字节    92
            RecordDatabuf[106] = Set_Control_Mode;
            //工作方式
            RecordDatabuf[107] = DriveControl_status;
            //拧紧结果1字节   95
            TorqueDifferenceValue = Get_ActualTorque(Set_Torque_Unit,Record_PressValue *0.03,Spanner_Coefficien);  //扭矩±3%差值
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
        case 5:                                                                     //传感器模式
           //目标扭矩4字节
            Target_Torque_data = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);  
            RecordDatabuf[73] = Target_Torque_data>>24;
            RecordDatabuf[74] = Target_Torque_data>>16;
            RecordDatabuf[75] = Target_Torque_data>>8;
            RecordDatabuf[76] = Target_Torque_data;
            //实际扭矩4字节    
            Torque_data = Get_ActualTorque(Set_Torque_Unit,Record_PressValue,Spanner_Coefficien);
            RecordDatabuf[77] = Torque_data>>24;
            RecordDatabuf[78] = Torque_data>>16;
            RecordDatabuf[79] = Torque_data>>8;
            RecordDatabuf[80] = Torque_data;
            //选定模式1字节    92
            RecordDatabuf[106] = Set_Control_Mode;
            //工作方式
            RecordDatabuf[107] = DriveControl_status;
            //拧紧结果1字节   95
            TorqueDifferenceValue = Get_ActualTorque(Set_Torque_Unit,Record_PressValue *0.03,Spanner_Coefficien);  //扭矩±3%差值
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
    //打印当前数据
    for(i = 0;i < 109;i++)
    {
         printf("%c",RecordDatabuf[i]);
        
    }
     printf("\r\n");
#endif

}
