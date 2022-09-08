#include "CH374DRV.H"
#include "CH374HFM.H"
#define CH374_INT_WIRE   GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)
#define CH374_SPI_SCS_H GPIO_SetBits(GPIOB,GPIO_Pin_12)
#define CH374_SPI_SCS_L GPIO_ResetBits(GPIOB,GPIO_Pin_12)
/*
  *   CS       PB12   
  *   INT      PC6  
  *   SCLK     PB13 
  *   MISO     PB14                 
  *   MOSI     PB15                 
*/  

void SPI2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;  
	SPI_InitTypeDef  SPI_InitStructure;
    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);   
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	           //INT
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;  
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_14|GPIO_Pin_13;  /* mosi miso clk */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
                                     
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;                      /* CS */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    CH374_SPI_SCS_H;            //CS拉高

	SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;/**< 两线全双工 */
	SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;                /**< 主机 */                                
	SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;                /**< 8位 */  
	SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;                  /**< 空闲为高电平 */
	SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;                 /**< 上升沿捕获（空闲为高电平时，第1个边沿为下降沿） */      
	SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;                   /**< NSS信号由软件管理 */ 
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;        /**< 72M / 4 = 18M */               
	SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;               /**< 数据传输从MSB位开始 */
	SPI_InitStructure.SPI_CRCPolynomial     = 7;                              /**< CRC7 */  
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE); 	
	
}
//SPI收发一个字节
UINT8 SPI3_TransByte(UINT8 TxData)
{		 			 
	while( RESET==SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)); 		//等待发送区空 
	SPI_I2S_SendData(SPI2, TxData); 	 	  		    //发送一个byte  
 	while( RESET==SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) );		//等待接收完一个byte  
 	return( SPI_I2S_ReceiveData(SPI2) );          		    //返回收到的数据				    
}

void Spi374Start( UINT8 addr, UINT8 cmd )  /* SPI开始 */
{
	CH374_SPI_SCS_L;    /* SPI片选有效 */
	SPI3_TransByte( addr );
	SPI3_TransByte( cmd );
}
void Spi374Stop( void )  /* SPI结束 */
{
	CH374_SPI_SCS_H;	 /* SPI片选无效 */
}
//提供给库调用的函数
UINT8 CH374_READ_REGISTER( UINT8 mAddr )  /* 外部定义的被CH374程序库调用的子程序,从指定寄存器读取数据 */
{
	UINT8	d;
	Spi374Start( mAddr, CMD_SPI_374READ );
	d = SPI3_TransByte( 0xFF );
	Spi374Stop( );
	return( d );
}

void CH374_WRITE_REGISTER( UINT8 mAddr, UINT8 mData )           /* 外部定义的被CH374程序库调用的子程序,向指定寄存器写入数据 */
{
	Spi374Start( mAddr, CMD_SPI_374WRITE );
	SPI3_TransByte( mData );
	Spi374Stop( );
}

void CH374_READ_BLOCK( UINT8 mAddr, UINT8 mLen, PUINT8 mBuf )   /* 外部定义的被CH374程序库调用的子程序,从指定起始地址读出数据块 */
{
	Spi374Start( mAddr, CMD_SPI_374READ );
	while ( mLen -- ) *mBuf++ = SPI3_TransByte( 0xFF );
	Spi374Stop( );
}

void CH374_WRITE_BLOCK( UINT8 mAddr, UINT8 mLen, PUINT8 mBuf )  /* 外部定义的被CH374程序库调用的子程序,向指定起始地址写入数据块 */
{
	Spi374Start( mAddr, CMD_SPI_374WRITE );
	while ( mLen -- ) SPI3_TransByte( *mBuf++ );
	Spi374Stop( );
}

PUINT8	CH374_READ_BLOCK64( UINT8 mAddr, PUINT8 mBuf )         /* 外部定义的被CH374程序库调用的子程序,从双缓冲区读出64字节的数据块,返回当前地址 */
{
	UINT8	i;
	Spi374Start( mAddr, CMD_SPI_374READ );
	for ( i = CH374_BLOCK_SIZE; i != 0; i -- ) *mBuf++ = SPI3_TransByte( 0xFF );
	Spi374Stop( );
	return( mBuf );
}

PUINT8	CH374_WRITE_BLOCK64( UINT8 mAddr, PUINT8 mBuf )       /* 外部定义的被CH374程序库调用的子程序,向双缓冲区写入64字节的数据块,返回当前地址 */
{
	UINT8	i;
	Spi374Start( mAddr, CMD_SPI_374WRITE );
	for ( i = CH374_BLOCK_SIZE; i != 0; i -- ) SPI3_TransByte( *mBuf++ );
	Spi374Stop( );
	return( mBuf );
}

void CH374_WRITE_BLOCK_C( UINT8 mLen, PUINT8C mBuf )          /* 外部定义的被CH374程序库调用的子程序,向RAM_HOST_TRAN写入常量型数据块 */
{
	Spi374Start( RAM_HOST_TRAN, CMD_SPI_374WRITE );
	do {
		SPI3_TransByte( *mBuf );
		mBuf ++;
	} while ( -- mLen );
	Spi374Stop( );
}
//CH374接口初始化
void CH374_Init( void )
{
	SPI2_Init();
	
	delay_ms(50);                                         /* 延时一下 */
	//读写寄存器测试
	CH374_WRITE_REGISTER( REG_USB_ADDR,  0x5A ) ;
	if( CH374_READ_REGISTER( REG_USB_ADDR )!=0x5A )
	{
		printf("Comm Erro\n") ;
	}else
    {
        printf("Comm OK\n") ;
    }
   
}

/* 检查操作状态,如果错误则显示错误代码并停机 */
void	mStopIfError( u8 iError )
{
	if ( iError == ERR_SUCCESS ) return;                        /* 操作成功 */
	printf( "Error: %02X\n", (UINT16)iError );                  /* 显示错误 */
}




/*---------------------------------------用户函数---------------------------------------------------------*/


/****************************************************************************
函数功能：在旧字符串中插入逗号形成新字符串
函数参数：uint8_t*pDest：带有逗号的新字符串
          uint8_t*pSrc：没有逗号的旧字符串
          uint8_t arrLen[]：在旧字符串中插入的逗号的位置
	        uint8_t count：位置数组的个数
	
函数返回值：无
**************************************************************************/	
	
	
void BeCommaStr(uint8_t*pDest,uint8_t*pSrc,uint8_t arrLen[],uint8_t count)
	 {
	   uint16_t i = 0;
		  uint16_t j = 0;
		  uint16_t k = 0;
		 uint16_t len = 0;
		  uint16_t len1 = 0;

for(k=0;k<count;k++)                //   4  12   4  12  
  {	 
	for(i=len,j=len1;j<=arrLen[k]-1+len1;j++)
	  {
		 pDest[i++] = pSrc[j];
		}
	   pDest[i] = ',';
		 len1 = j;              //4 ,16,20        
		 len=i+1; 
				 
	}	
				
 }
     
 
 
/*******************************************************************

函数功能：向U盘文件写入字符串
函数参数：uint8_t* pSrc：待写入的字符串
          uint8_t size： 每次写入U盘字节的长度
          uint8_t len：待写入字符串的长度
函数返回值：无

********************************************************************/


 void formationStr(uint8_t* pSrc,uint8_t size,uint8_t len)
  {
	  uint8_t j = 0;
		uint8_t i = 0;
		uint8_t count = 0;
	 for(j=0;j<len/size;j++)
     {
        memcpy(mCmdParam.ByteWrite.mByteBuffer,(const char*)&pSrc[count],size);
        mCmdParam.ByteWrite.mByteCount = size; 
        i = CH374ByteWrite( );                                /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
	      mStopIfError( i );
		    count += size;	 
		 
     }
    memcpy(mCmdParam.ByteWrite.mByteBuffer,(const char*)&pSrc[count],len%16);
     mCmdParam.ByteWrite.mByteCount =len%16; 
     i = CH374ByteWrite( );                                /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
	   mStopIfError( i );
	
    memcpy(mCmdParam.ByteWrite.mByteBuffer,(const char*)"\r\n",2);
     mCmdParam.ByteWrite.mByteCount = 2; 
     i = CH374ByteWrite( );                                /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
	   mStopIfError( i );
	} 
 
 
    

 void StructToStr(char buf[],char databuf[]) 
{
    uint8_t   TorqueModebuf[15]  ={0x54,0x6F,0x72,0x71,0x75,0x65,0x2F,0xC5,0xA4,0xBE,0xD8};  //   Torque/扭矩
    uint8_t   BoltModebuf[15] = {0x42,0x6F,0x6C,0x74,0x2F,0xC2,0xDD,0xCB,0xA8};              //  Bolt/螺栓
    uint8_t   GearModebuf[15] = {0x47,0x65,0x61,0x72,0x2F,0xB5,0xB2,0xCE,0xBB};             //  Gear/档位
    uint8_t   AngelModebuf[15] = {0x41,0x6E,0x67,0x6C,0x65,0x2F,0xBD,0xC7,0xB6,0xC8};       //  Angle/角度
    uint8_t   SenorModebuf[15] = {0x53,0x65,0x6E,0x73,0x6F,0x72,0x2F,0xB4,0xAB,0xB8,0xD0,0xC6,0xF7}; //  Sensor/传感器
    uint8_t   AutoMode[15] = {0xD7,0xD4,0xB6,0xAF,0x2F,0x41,0x55,0x54,0x4F}; //  自动/AUTO
    uint8_t   HandMode[15] = {0xCA,0xD6,0xB6,0xAF,0x2F,0x48,0x41,0x4E,0x44}; //  手动/HAND
    uint8_t   strRight[2] = {0xA1,0xC1};  //√
    uint8_t   strWrong[2] = {0xA1,0xCC};  //×
    //单位
    u8 TorqueUintN[4] = "N.m";
    u8 TorqueUintF[7] = "Ft.lbs";
    u8 PressureUintB[4] = "Bar";
    u8 PressureUintM[4] = "Mpa";
    u8 PressureUintP[4] = "Psi";
    char str[30]  = {0};
    char i = 0;
    char len = 0;
    char count = 0;
	u32 DataU32 = 0;
	u16 DataU16 = 0;
    u8 DataU8 = 0;
    

    DataU32 = PTR2U32(&databuf[0]);    //序号
    sprintf(str,"%d",DataU32);              
    len = strlen(str);
    for(i=0;i<len;i++)
    {
        buf[count++] = str[i];
    }
    buf[count++] = ','; 
    strncpy(&buf[count],&databuf[4],10);    //工具型号 
    count = strlen(buf);
    buf[count++] = ',';
    strncpy(&buf[count],&databuf[14],8);	 //出厂编号
    count = strlen(buf);
    buf[count++] = ',';
    memset(str,0,sizeof(str));         //年                         
    DataU16 = PTR2U16(&databuf[22]);
    sprintf(str,"%d",DataU16);
    len = strlen(str);
    for(i=0;i<len;i++)
    {
      buf[count++] = str[i];
    }
    buf[count++] = '.';	 
    memset(str,0,sizeof(str));     	//月                        
    DataU8 = databuf[24];
    sprintf(str,"%d",DataU8);
    len = strlen(str);
    for(i=0;i<len;i++)
    {
        buf[count++] = str[i];
    }
    buf[count++] = '.';
    memset(str,0,sizeof(str));      //日 
    DataU8 = databuf[25];    
    sprintf(str,"%d",DataU8);
    len = strlen(str);
    for(i=0;i<len;i++)
    {
        buf[count++] = str[i];
    }
    buf[count++] = '-';
    memset(str,0,sizeof(str));    //时	
    DataU8 = databuf[26];     
    sprintf(str,"%d",DataU8);
    len = strlen(str);
    for(i=0;i<len;i++)
    {
        buf[count++] = str[i];
    }
    buf[count++] = ':';
    memset(str,0,sizeof(str));     //分
    DataU8 = databuf[27]; 
    sprintf(str,"%d",DataU8);
    len = strlen(str);
    for(i=0;i<len;i++)
    {
    buf[count++] = str[i];
    }
    buf[count++] = ':';
    memset(str,0,sizeof(str));   	//秒                          
    DataU8 = databuf[28]; 
    sprintf(str,"%d",DataU8);
    len = strlen(str);
    for(i=0;i<len;i++)
    {
        buf[count++] = str[i];
    }
    buf[count++] = ',';
    strncpy(&buf[count],&databuf[29],16);		//设备名称 	
    count = strlen(buf);                
    buf[count++] = ',';		   
    strncpy(&buf[count],&databuf[45],8);    //设备编号  
    count = strlen(buf);             
    buf[count++] = ',';	       
    strncpy(&buf[count],&databuf[53],16) ;    //操作人员	
    count = strlen(buf);  
    buf[count++] = ',';	
    memset(str,0,sizeof(str));        //对应压力                         
    DataU16 = PTR2U16(&databuf[71]);
    sprintf(str,"%d",DataU16);
    len = strlen(str);
    for(i=0;i<len;i++)
    {
      buf[count++] = str[i];
    }
    buf[count++] = 0x20;
    switch(databuf[69])  //压力单位
    {
        case 0:
            strncpy((char*)&buf[count],(char*)PressureUintB,sizeof(PressureUintB));     
            count = strlen(buf);  
            break;
        case 1:
            strncpy((char*)&buf[count],(char*)PressureUintM,sizeof(PressureUintM));     
            count = strlen(buf);
            break;
        case 2:              
            strncpy((char*)&buf[count],(char*)PressureUintP,sizeof(PressureUintP));     
            count = strlen(buf);
            break;
    }
    buf[count++] = ',';	
    memset(str,0,sizeof(str));     //目标扭矩	
    DataU32 = PTR2U32(&databuf[73]);
    sprintf(str,"%d",DataU32);
    len = strlen(str);
    for(i=0;i<len;i++)
    {
        buf[count++] = str[i];
    }
    buf[count++] = 0x20;
    switch(databuf[70])   //扭矩单位
    {
        case 0:
            strncpy((char*)&buf[count],(char*)TorqueUintN,sizeof(TorqueUintN));     
            count = strlen(buf);  
            break;
        case 1:
            strncpy((char*)&buf[count],(char*)TorqueUintF,sizeof(TorqueUintF));     
            count = strlen(buf);
            break;
    }
    buf[count++] = ',';				
    memset(str,0,sizeof(str));     //实际扭矩
    DataU32 = PTR2U32(&databuf[77]);    
    sprintf(str,"%d",DataU32);
    len = strlen(str);
    for(i=0;i<len;i++)
    {
        buf[count++] = str[i];
    }
    buf[count++] = 0x20;
    switch(databuf[70])   //扭矩单位
    {
        case 0:
            strncpy((char*)&buf[count],(char*)TorqueUintN,sizeof(TorqueUintN));     
            count = strlen(buf);  
            break;
        case 1:
            strncpy((char*)&buf[count],(char*)TorqueUintF,sizeof(TorqueUintF));     
            count = strlen(buf);
            break;
    }
    buf[count++] = ',';	
/*----------------------------------模式划分-----------------------------------------------------*/
    switch(databuf[106])  
    {
        case 1:                             //扭矩模式 
            buf[count++] = 0x20;
            //螺栓直径          
            buf[count++] = ',';	
            //螺栓等级      
            buf[count++] = ',';
            //目标角度     
            buf[count++] = ',';	   		
            //实际角度	     
            buf[count++] = ',';	 			
            //挡位	
            buf[count++] = ',';		
            //选定模式
            strncpy((char*)&buf[count],(char*)TorqueModebuf,sizeof(TorqueModebuf));          
            count = strlen(buf);
            buf[count++] = ',';	 
            break;
/*---------------------------------------------------------------------------------------------------------------------------------------*/            
        case 2:                              //螺栓模式
            buf[count++] = 0x20;
            //螺栓直径  
            strncpy((char*)&buf[count],&databuf[81],10); 
            count = strlen(buf);
            buf[count++] = ',';	
            //螺栓等级     
            strncpy((char*)&buf[count],&databuf[91],10);
            count = strlen(buf);            
            buf[count++] = ',';
            //目标角度     
            buf[count++] = ',';	   		
            //实际角度	     
            buf[count++] = ',';	 			
            //挡位	
            buf[count++] = ',';		
            //选定模式
            strncpy((char*)&buf[count],(char*)BoltModebuf,sizeof(BoltModebuf));          
            count = strlen(buf);
            buf[count++] = ',';	            
            break;
/*----------------------------------------------------------------------------------------------------------------*/            
        case 3:                             //档位模式  
            buf[count++] = 0x20;	        
            //螺栓直径  
            buf[count++] = ',';	
            //螺栓等级     
            buf[count++] = ',';
            //目标角度            
            buf[count++] = ',';	   		
            //实际角度	     
            buf[count++] = ',';	 			
            //挡位	
            memset(str,0,sizeof(str));     
            DataU8 = databuf[105];    
            sprintf(str,"%d",DataU8);
            len = strlen(str);
            for(i=0;i<len;i++)
            {
                buf[count++] = str[i];
            }  
            buf[count++] = ',';		
            //选定模式
            strncpy((char*)&buf[count],(char*)GearModebuf,sizeof(GearModebuf));          
            count = strlen(buf);
            buf[count++] = ',';	
            break;
/*-------------------------------------------------------------------------------------------------------------*/
        case 4:                             //角度模式 
            buf[count++] = 0x20;            
            //螺栓直径  
            buf[count++] = ',';	
            //螺栓等级                
            buf[count++] = ',';
            //目标角度   
            memset(str,0,sizeof(str));   	
            DataU16 = PTR2U16(&databuf[101]);
            sprintf(str,"%d",DataU16);
            len = strlen(str);
            for(i=0;i<len;i++)
            {
                buf[count++] = str[i];
            }
            buf[count++] = ',';	   		
            //实际角度	
            memset(str,0,sizeof(str));   	
            DataU16 = PTR2U16(&databuf[103]);
            sprintf(str,"%d",DataU16);
            len = strlen(str);
            for(i=0;i<len;i++)
            {
                buf[count++] = str[i];
            }
            buf[count++] = ',';	 			
            //挡位	
            buf[count++] = ',';		
            //选定模式
            strncpy((char*)&buf[count],(char*)AngelModebuf,sizeof(AngelModebuf));          
            count = strlen(buf);
            buf[count++] = ',';	           
            break;
/*--------------------------------------------------------------------------------------------------------------------*/            
        case 5:                             //传感器模式
            buf[count++] = 0x20;
            //螺栓直径  
            buf[count++] = ',';	
            //螺栓等级                
            buf[count++] = ',';
            //目标角度   
            buf[count++] = ',';	   		
            //实际角度	
            buf[count++] = ',';	 			
            //挡位	
            buf[count++] = ',';		
            //选定模式
            strncpy((char*)&buf[count],(char*)SenorModebuf,sizeof(SenorModebuf));          
            count = strlen(buf);
            buf[count++] = ',';	
            break;
    }
    //工作方式
    switch(databuf[107])   
    {
        case 0:
            strncpy((char*)&buf[count],(char*)HandMode,sizeof(HandMode));     
            count = strlen(buf);
            break;
        case 1:
            strncpy((char*)&buf[count],(char*)AutoMode,sizeof(AutoMode));         
            count = strlen(buf);
            break;
    }	
    buf[count++] = ',';	
        
    //拧紧结果		
    switch(databuf[108])   
    {
        case 0:
            strncpy((char*)&buf[count],(char*)strRight,sizeof(strRight));      
            count = strlen(buf);
            break;
        case 1:
            strncpy((char*)&buf[count],(char*)strWrong,sizeof(strWrong));          
            count = strlen(buf);
            break;
    }
  		
	
}
 


//固定中文显示
char arrChina[170] = {0xD0,0xF2,0xBA,0xC5,0x2C,
    0xB9,0xA4,0xBE,0xDF,0xD0,0xCD,0xBA,0xC5,0x2C,
    0xB9,0xA4,0xBE,0xDF,0xB3,0xF6,0xB3,0xA7,0xB1,0xE0,0xBA,0xC5,0x2C,
    0xC8,0xD5,0xC6,0xDA,0x2C,
    0xD3,0xC3,0xBB,0xA7,0xC9,0xE8,0xB1,0xB8,0xC3,0xFB,0xB3,0xC6,0x2C,
    0xD3,0xC3,0xBB,0xA7,0xC9,0xE8,0xB1,0xB8,0xB1,0xE0,0xBA,0xC5,0x2C,
    0xB2,0xD9,0xD7,0xF7,0xC8,0xCB,0xD4,0xB1,0x2C,
    0xB6,0xD4,0xD3,0xA6,0xD1,0xB9,0xC1,0xA6,0x2C,
    0xC4,0xBF,0xB1,0xEA,0xC5,0xA4,0xBE,0xD8,0x2C,
    0xCA,0xB5,0xBC,0xCA,0xC5,0xA4,0xBE,0xD8,0x2C,
    0xC2,0xDD,0xCB,0xA8,0xB9,0xE6,0xB8,0xF1,0x2C,
    0xC2,0xDD,0xCB,0xA8,0xC7,0xBF,0xB6,0xC8,0xB5,0xC8,0xBC,0xB6,0x2C,
    0xC4,0xBF,0xB1,0xEA,0xBD,0xC7,0xB6,0xC8,0x2C,
    0xCA,0xB5,0xBC,0xCA,0xBD,0xC7,0xB6,0xC8,0x2C,
    0xB5,0xB2,0xCE,0xBB,0x2C,
    0xD1,0xA1,0xB6,0xA8,0xC4,0xA3,0xCA,0xBD,0x2C,
    0xB9,0xA4,0xD7,0xF7,0xB7,0xBD,0xCA,0xBD,0x2C,
    0xC5,0xA1,0xBD,0xF4,0xBD,0xE1,0xB9,0xFB,0x2C};
//固定英文显示
char arrEnglish[230] ="Number,Product type,Serial Number,Date,User EquipmentItem,User Equipment Number,Operator,Work Pressure,Target Torque,Actual Torque,Bolt Size,Bolt Strength,Target Angle,Actual Angle,Gear,Select Mode,Working Mode,Fastening Result,";

void WriteTail(void)
 {

    uint8_t buf[200] = {0};
    uint8_t Databuff[110] = {0};
    u32 i = 0;
    uint8_t j = 0;
    uint8_t count = 0;
    uint8_t len = 0;	   
	// 写入中文
    len = strlen(arrChina);
    formationStr((u8*)arrChina,16,len); 
    //写入英文
    len = strlen(arrEnglish);
    formationStr((u8*)arrEnglish,16,len); 
    
    for(i = 0;i < RecorddataCount;i++)
    {
        memset(buf,' ',sizeof(buf));    //填充空白至字符串
//        printf("i:%d\r\n",i);
        MX25L6406_Read(Databuff,20*4096+i*109,109); //读回Flash内储存的数据
        StructToStr((char*)buf,(char*)Databuff);    //处理数据
        len = strlen((char*)buf);
        formationStr(buf,16,len);                   //写入U盘
        FreeDogStructVar.UdiskTaskDog = 0;
        OSTimeDly(10); 
    }      
      
		 
 }













