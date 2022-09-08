#include "USBType-C.h" 
Simple1 Gears_data[Gear_MUN];


u8 USART2_RX_BUF[2048];                   //接收缓冲，最大64字节
u16 USART2_RX_CNT=0;                       //接收字节计数器
u8 TypeC_flagFrame=0;                         //帧接收完成标志，即接收到一帧新数据
/*!
*   \brief   串口2初始化
*   \param   BaudRate-波特率设置
*/
void TypeC_UartInit(uint32 BaudRate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* Configure USART Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //  /* Configure USART Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
    
    USART_DeInit(USART2);                                           //复位USART2
    USART_InitStructure.USART_BaudRate = BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode =   USART_Mode_Tx|USART_Mode_Rx;

    /* USART configuration */
    USART_Init(USART2, &USART_InitStructure);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                  //接收中断使能

    /* Enable USART */
    USART_Cmd(USART2, ENABLE);
}

void USART2_IRQHandler(void)
{
    u8 res;	
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        res = USART_ReceiveData(USART2);
        if(USART2_RX_CNT<sizeof(USART2_RX_BUF))    //一次只能接收1024个字节，人为设定，可以更大，但浪费时间
        {
            USART2_RX_BUF[USART2_RX_CNT]=res;  //记录接收到的值           
            USART2_RX_CNT++;        //使收数据增加1 
        } 
    }
}

/*!*   \brief  发送1个字节
*   \param  t 发送的字节
*/
void  TypeC_SendChar(uchar t)
{
    USART_SendData(USART2,t);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    while((USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET));//等待串口发送完毕
}

//计算发送的数据长度，并且将数据放到*buf数组中                     
u16 TypeC_UartRead(u8 *buf, u16 len)  
{
	u16 i;
	if(len>USART2_RX_CNT)  //指定读取长度大于实际接收到的数据长度时
	{
		len=USART2_RX_CNT; //读取长度设置为实际接收到的数据长度
	}
	for(i=0;i<len;i++)  //拷贝接收到的数据到接收指针中
	{
		*buf=USART2_RX_BUF[i];  //将数据复制到buf中
		buf++;
	}
	USART2_RX_CNT=0;              //接收计数器清零
	return len;                   //返回实际读取长度
}


u8 TypeC_UartWrite(u8 *buf ,u16 len) 										//发送
{
	u16 i=0;    
    for(i=0;i<len;i++)
    {
        USART_SendData(USART2,buf[i]);	                                      //通过USARTx外设发送单个数据
        while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);             //检查指定的USART标志位设置与否，发送数据空位标志
    }

}
u8 Set_Factory_Number[20] ={0};//设备工具出厂编号
u8 Device_name[20] ={0};    //用户设备名称
u8 Set_Equipment_Number[20] ={0};//用户设备编号
u8 Operator_name[20] ={0};   //操作人员


u16 Spanner_Number = 0;  //扳手数量
char Password[6] = "123456";//设定的密码
char Unlock_Password[6];  //解锁密码

u8 Data_Storage = 0; //数据存储标志位

u8 Factory_Number_length = 0;
u8 Device_name_length = 0;
u8 Equipment_Number_length = 0;
u8 Operator_name_length = 0;
u8 Gear_length = 7;



u8 Blot_addressNumber = 0;
//串口驱动函数，检测数据帧的接收，调度功能函数，需在主循环中调用
void TypeC_UartDriver()
{
    
    unsigned char i = 0,j = 0;
    u16 cnt = 0;
    u8 Cmd_Datalength = 0;
    u16 DatalengthU16 = 0;
    u8 spa_name[10];
	unsigned int crc;
	unsigned char crch,crcl;
	static u16 len;
    u32 Cmd_adress = 0;
    u8 buf[2048];  
    if(TypeC_flagFrame)            //帧接收完成标志，即接收到一帧新数据
	{
		TypeC_flagFrame=0;           //帧接收完成标志清零
		len = TypeC_UartRead(buf,sizeof(buf));   //将接收到的命令读到缓冲区中
        printf("len:%d\r\n",len);
        if(buf[0] == 0x5A && buf[1] == 0xA5)     //帧头
        {
            crc=GetCRC16(buf,len-2);       //计算CRC校验值，出去CRC校验值
            printf("crc:%x\r\n",crc);
			crch=crc>>8;    				//crc高位
			crcl=crc&0xFF;					//crc低位
            if((buf[len-2]==crch)&&(buf[len-1]==crcl))  //判断CRC校验是否正确
            {				 
                switch(buf[4])    //命令码
                {
                    case 0x01:    //设置时间
                        RTC_EnterConfigMode();     //允许配置
                        calendar.w_year = buf[7]<<8|buf[8];
                        calendar.w_month = buf[9];
                        calendar.w_date = buf[10];
                        calendar.hour = buf[11];
                        calendar.min = buf[12];
                        calendar.sec = buf[13] + 1;
                        RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);                    
                        RTC_ExitConfigMode();      //退出配置模式 
                        len -= 11;
                        buf[len++] = 0x00;
                        buf[len++] = 0x00;
                        break;
                    case 0x03:    //设置出厂编号                       
                        Factory_Number_length = (buf[5]<<8|buf[6]);    //获取设备名称数据长度                       
                        for(i = 0;i<Factory_Number_length ;i++)            //获取出厂编号
                        {
                            Set_Factory_Number[i] = buf[i+7];
//                            printf("%c",Set_Factory_Number[i]);
                        } 
                        Data_Storage = 2;
                        len -= Factory_Number_length+4;
                        buf[len++] = 0x00;
                        buf[len++] = 0x00;
                        break;
                    case 0x04:
                        len -=2 ;
                        break;                        
                    case 0x05:    //设置用户设备名称                                                
                        Device_name_length = (buf[5]<<8|buf[6]);        //获取设备名称数据长度
                        for(i = 0;i<Device_name_length ;i++)            //获取设置设备名称
                        {
                            Device_name[i] = buf[i+7];
//                            printf("%c",Device_name[i]);
                        } 
                        Data_Storage = 3;                        
                        len -= Device_name_length+4;
                        buf[len++] = 0x00;
                        buf[len++] = 0x00;
                        break;
                    case 0x06:
                        len -= 2;
                        break; 
                    case 0x07:    //设置设备编号                      
                        Equipment_Number_length = (buf[5]<<8|buf[6]);        //获取设备编号数据长度                       
                        for(i = 0;i<Equipment_Number_length ;i++)            //获取设置设备编号
                        {
                            Set_Equipment_Number[i] = buf[i+7];     
                            printf("%c",Set_Equipment_Number[i]);
                        }
                        Data_Storage = 4;
                        len -= Equipment_Number_length+4;  //响应帧长度
                        buf[len++] = 0x00;    //数据位清空
                        buf[len++] = 0x00;
                        break;
                    case 0x08:
                        len -= 2;
                        break;
                    case 0x09:    //解锁
                        Cmd_Datalength = (buf[5]<<8|buf[6]) - 1;    //获取密码数据长度
                        if(buf[7] == 0)
                        {
                            Driver_SendChardata(0x0A,(char*)&buf[8],Cmd_Datalength);  //给驱动板发送解密密码 
                        }                            
                        len -= 2;
                        break;
                    case 0x10:
                        len -= 2;
                        break;
                    case 0x0A:    //设置解锁密码
                        Cmd_Datalength = (buf[5]<<8|buf[6]) ;    //获取解锁密码数据长度
                        Driver_SendChardata(0x09,(char*)&buf[7],Cmd_Datalength);  //给驱动板发送解密密码
                        buf[5] = 0x00;              //发送的数据长度
                        buf[6] = 0x00;                     
                        len = 7;
                        break;
                    case 0x0B:    //设置使用次数                  
                        Driver_Send32DataValue(0x07,PTR2U32(&buf[7]));//给驱动板发送限制次数
                        buf[5] = 0x00;              //发送的数据长度
                        buf[6] = 0x00;                     
                        len = 7;
                        break;
                    case 0x0F:    //设置操作人员                        
                        Operator_name_length = (buf[5]<<8|buf[6]);    //获取设备名称数据长度                       
                        for(i = 0;i<Operator_name_length ;i++)            //获取设置设备名称
                        {
                            Operator_name[i] = buf[i+7];                            
                            printf("%c",Operator_name[i]);
                        } 
                        Data_Storage = 5; 
                        len -= Operator_name_length+4;
                        buf[len++] = 0x00;
                        buf[len++] = 0x00;
                        break;
                    case 0x0E:    //导入数据
                                           
                    break;
                    case 0x11:         //扭矩模式  
                        Spanner_Number = buf[7]<<8|buf[8];    //获取导入扳手个数  
                        for(cnt = 0;cnt < Spanner_Number;cnt++)
                        {                                                       
                            strncpy(SpannerType[cnt].type,(char*)&buf[cnt*15+9],10);     
									 printf("type == %s\r\n",SpannerType[cnt].type);
                            SpannerType[cnt].parameter = PTR2U32(&buf[cnt*15+19]);
									 printf("parameter == %d\r\n",SpannerType[cnt].parameter);
                            SpannerType[cnt].Delay_time = buf[cnt*15+23];
                        }
                        Data_Storage = 1; 
                        buf[5] = 0x00;              //发送的数据长度
                        buf[6] = 0x00;  
                        len  = 7; 
                    break;
                    case 0x12:         //螺栓模式   
                         for(cnt = 0;cnt < Spanner_Number;cnt++)
                            {
                                if(strcmp(SpannerType[cnt].type,(char*)&buf[7]) == 0)
                                {
                                    Blot_addressNumber = cnt;
                                    printf("Blot_addressNumber:%d\r\n",Blot_addressNumber);  
                                    //公制
                                    if(buf[17] == 0x00)
                                    {
                                            SpannerType[cnt].Diameter_Metric = buf[18];//公制直径数量 
                                            //公制螺栓名称
                                            for(i = 0;i < SpannerType[cnt].Diameter_Metric;i++)
                                            {
                                                strncpy((char*)BoltType.Metric_Diameter[i],(char*)&buf[i*10+19],10); 
                                                printf("%s\r\n",BoltType.Metric_Diameter[i]);                                                
                                            }
                                            
                                            SpannerType[cnt].Grade_Metric = buf[19+SpannerType[cnt].Diameter_Metric*10];  //公制等级数
                                             //公制等级名称
                                            for(i = 0;i < SpannerType[cnt].Grade_Metric;i++)
                                            {
                                                strncpy((char*)BoltType.Metric_Grade[i],(char*)&buf[20+(SpannerType[cnt].Diameter_Metric+i)*10],10); 
                                                printf("%s\r\n",BoltType.Metric_Grade[i]);  
                                            }
                                            //螺栓扭矩
                                            for(i = 0;i < SpannerType[cnt].Grade_Metric;i++)
                                            {
                                                for(j = 0;j < SpannerType[cnt].Diameter_Metric;j++)
                                                {
                                                    BoltType.Metric_parameter[i][j] = 
                                                    PTR2U32(&buf[20+(SpannerType[cnt].Diameter_Metric+SpannerType[cnt].Grade_Metric)*10 + (i*SpannerType[cnt].Diameter_Metric+j)*4]); 
                                                    printf("%d\r\n",BoltType.Metric_parameter[i][j]);  
                                                }                                                
                                            }
                                     }
                                    Cmd_adress = 20 + (SpannerType[cnt].Diameter_Metric + SpannerType[cnt].Grade_Metric) * 10 + (SpannerType[cnt].Diameter_Metric * SpannerType[cnt].Grade_Metric)*4;  //英制起始位置
//                                    printf("Cmd_adress：%d\r\n",Cmd_adress);  
                                    //英制
                                     if(buf[Cmd_adress] ==0x01)   
                                    {
                                            SpannerType[cnt].Diameter_Imperial = buf[Cmd_adress+1];//英制直径数量
                                            //英制螺栓名称
                                            for(i = 0;i < SpannerType[cnt].Diameter_Imperial;i++)
                                            {
                                                strncpy((char*)BoltType.Imperial_Diameter[i],(char*)&buf[i*10+Cmd_adress+2],10); 
                                                printf("%s\r\n",BoltType.Imperial_Diameter[i]);                                                  
                                            }
                                            SpannerType[cnt].Grade_Imperial = buf[Cmd_adress+2+SpannerType[cnt].Diameter_Imperial*10];  //公制等级数
                                             //英制等级名称
                                            for(i = 0;i < SpannerType[cnt].Grade_Imperial;i++)
                                            {
                                                strncpy((char*)BoltType.Imperial_Grade[i],(char*)&buf[Cmd_adress+3+SpannerType[cnt].Diameter_Imperial*10+i*10],10);
                                                printf("%s\r\n",BoltType.Imperial_Grade[i]);  
                                                printf("ok\r\n"); 
                                            }
                                            //螺栓扭矩
                                            for(i = 0;i < SpannerType[cnt].Grade_Imperial;i++)
                                            {
                                                for(j = 0;j < SpannerType[cnt].Diameter_Imperial;j++)
                                                {
                                                    BoltType.Imperial_parameter[i][j] = 
                                                    PTR2U32(&buf[Cmd_adress+3+(SpannerType[cnt].Diameter_Imperial+SpannerType[cnt].Grade_Imperial)*10 + (i*SpannerType[cnt].Diameter_Imperial+j)*4]);
                                                    printf("%d\r\n",BoltType.Imperial_parameter[i][j]);  
                                                }                                                
                                            }
                                    }                                         
                                   
                                    break;
                                }
                            }
                        Data_Storage = 10; 
                        buf[5] = 0x00;              //发送的数据长度
                        buf[6] = 0x00;  
                        len  = 7;                             
                    break;
                    case 0x13:         //角度模式                       
                        Max_Angle = buf[7]<<8|buf[8];
                      //  printf("Max_Angle:%d\r\n",Max_Angle);  
                        Data_Storage = 6; 
                        buf[5] = 0x00;              //发送的数据长度
                        buf[6] = 0x00; 
                        len = 7;
                    break;
                    case 0x14:         //档位模式                 
                        Gear_length = buf[7];  //获取档位个数
                        for(i = 0;i<Gear_length ;i++)            
                        {
                             Gears_data[i].Levels = buf[i*5+8];      //档位等级1字节
                             Gears_data[i].parameter = PTR2U32(&buf[i*5+9]);//档位压力4字节
                        } 
                        Data_Storage = 7; 
                        buf[5] = 0x00;              //发送的数据长度
                        buf[6] = 0x00; 
                        len  = 7; 
                       
                    break;
                    case 0x15:         //传感器模式                 
                        DatalengthU16 = buf[7]<<8|buf[8];    //获取导入数据个数  
                        for(i = 0;i < DatalengthU16;i++)
                        {   
                            for(cnt = 0;cnt < Spanner_Number;cnt++)
                            {
                                if(strcmp(SpannerType[cnt].type,(char*)&buf[i*14+9]) == 0)
                                {
                                    SpannerType[cnt].Sensor_parameter = PTR2U32(&buf[i*14+19]);
                                    printf("%d\r\n",SpannerType[cnt].Sensor_parameter);
                                    break;
                                }
                            }                                                        
                        }
                        Data_Storage = 8; 
                        buf[5] = 0x00;              //发送的数据长度
                        buf[6] = 0x00;  
                        len  = 7; 
                    break;          
                }
                crc=GetCRC16(buf,len);           //计算CRC校验值
				buf[len++]=crc>>8;           //CRC高字节
				buf[len++]=crc&0xff;        //CRC低字节 
                TypeC_UartWrite(buf,len);  //发送响应帧
            }
        
        }
    }
}




//判断是否接收一帧数据
void Uart2RxMonitor(u8 ms) //串口接收监控
{
	static u16 USART2_RX_BKP=0;  //定义USART2_RC_BKP暂时存储诗句长度与实际长度比较
	static u8 idletmr=0;        //定义监控时间
	if(USART2_RX_CNT>0)//接收计数器大于零时，监控总线空闲时间
	{
		if(USART2_RX_BKP!=USART2_RX_CNT) //接收计数器改变，即刚接收到数据时，清零空闲计时
		{
			USART2_RX_BKP=USART2_RX_CNT;  //赋值操作，将实际长度给USART2_RX_BKP
			idletmr=0;                    //将监控时间清零
		}
		else                              ////接收计数器未改变，即总线空闲时，累计空闲时间
		{
			//如果在一帧数据完成之前有超过3.5个字节时间的停顿，接收设备将刷新当前的消息并假定下一个字节是一个新的数据帧的开始
			if(idletmr<5)                  //空闲时间小于1ms时，持续累加
			{
				idletmr +=ms;
				if(idletmr>=5)             //空闲时间达到1ms时，即判定为1帧接收完毕
				{
					TypeC_flagFrame=1;//设置命令到达标志，帧接收完毕标志                    
				}
			}
		}
	}
	else
	{
		USART2_RX_BKP=0;
	}
}