#include "hmi_user_uart.h"	
#include "gpio.h"
#include "TASK.h"
#include "usart.h"	
#include "crc16.h"
#include "Data_process.h"
#include "MX25L6406.h"
u8 USART2_RX_BUF[255];                   //接收缓冲，最大64字节
u8 USART2_RX_CNT=0;                       //接收字节计数器
u8 flagFrame=0;                         //帧接收完成标志，即接收到一帧新数据
/*!
*   \brief   串口4初始化
*   \param   BaudRate-波特率设置
*/
void Uart4Init(uint32 BaudRate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

    /* Configure USART Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    //  /* Configure USART Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
    
    USART_DeInit(UART4);                                           //复位USART4
    USART_InitStructure.USART_BaudRate = BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode =   USART_Mode_Tx|USART_Mode_Rx;

    /* USART configuration */
    USART_Init(UART4, &USART_InitStructure);
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);                  //接收中断使能

    /* Enable USART */
    USART_Cmd(UART4, ENABLE);
}


/*!
*   \brief  发送1个字节
*   \param  t 发送的字节
*/
void  SendChar(uchar t)
{
    USART_SendData(UART4,t);
    while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
    while((USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET));//等待串口发送完毕
}

 void UART4_IRQHandler(void)
{
    u8 res;	
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//接收到数据
	{	 	
		res =USART_ReceiveData(UART4);//;读取接收到的数据USART2->DR
		if(USART2_RX_CNT<sizeof(USART2_RX_BUF))    //一次只能接收64个字节，人为设定，可以更大，但浪费时间
		{
			USART2_RX_BUF[USART2_RX_CNT]=res;  //记录接收到的值           
			USART2_RX_CNT++;        //使收数据增加1 
		}
	}

}
//计算发送的数据长度，并且将数据放到*buf数组中                     
u8 UartRead(u8 *buf, u8 len)  
{
	 u8 i;
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

u8 UartWrite(u8 *buf ,u8 len) 										//发送
{
	u8 i=0;                                                              //3MS延时   
    for(i=0;i<len;i++)
    {
        USART_SendData(UART4,buf[i]);	                                      //通过USARTx外设发送单个数据
        while(USART_GetFlagStatus(UART4,USART_FLAG_TXE)==RESET);             //检查指定的USART标志位设置与否，发送数据空位标志
    }
//	return 0;
}
u8 Passwordflag = 0;
u8 Controlflag = 0;
char Password[8] = "123456";//设定的密码
char Unlock_Password[8] = "123456";  //解锁密码
//串口驱动函数，检测数据帧的接收，调度功能函数，需在主循环中调用
void UartDriver()
{
    u16 len;
    u8 Cmd_Datalength = 0;
    unsigned int crc;
	unsigned char crch,crcl;
    u8 buf[255]; 
    if(flagFrame)            //帧接收完成标志，即接收到一帧新数据
	{   
        flagFrame=0;           //帧接收完成标志清零
		len = UartRead(buf,sizeof(buf));   //将接收到的命令读到缓冲区中
        if(buf[0] == cmd_head)                     //判断帧头是否正确
        {
            crc=GetCRC16(buf,len-2);         // 除去CRC校验值   计算CRC校验值
            crch=crc>>8;    				//crc高位
            crcl=crc&0xFF;
            printf("crc:%d\r\n",crc);
            if((buf[len-2]==crch)&&(buf[len-1]==crcl))  //判断CRC校验是否正确
            {
                switch (buf[1])  //按功能码执行操作
                {
                    case 0x01:                  //扳手系数
                        SpannerDate.p_SpannerCoefficient = PTR2U32(&buf[2]);
                        Data_Storage = 1;
                        
                        break;
                    case 0x02:                  //用户设定扭矩
                        SpannerDate.p_UserTargetTorque = PTR2U32(&buf[2]);
                        Data_Storage = 2;
                        
                        break;
                    case 0x03:                  //螺栓扭矩                       
                        SpannerDate.p_BoltTorque = PTR2U32(&buf[2]);
                        Data_Storage = 3;
                        
                        break;
                    case 0x04:                  //档位扭矩
                        
                        SpannerDate.p_GearTorque = PTR2U32(&buf[2]);
                        Data_Storage = 4;
                        
                        break;
                    case 0x05:                  //预设角度
                        
                        SpannerDate.p_TargetAngle = PTR2U32(&buf[2]);
                        Data_Storage = 5;
                        
                        break;
                    case 0x06:                  //预设传感器扭矩
                        SpannerDate.p_TargetSensor = PTR2U32(&buf[2]);
                        Data_Storage = 6;                   
                        break;
                    case 0x07:                  //设定使用次数
                        SpannerDate.p_TargetUsageNumber = PTR2U32(&buf[2]);
                        Data_Storage = 7;
                        break;
                    case 0x08:                  //模式
                        SpannerDate.p_Mode = buf[2];
                        Data_Storage = 8;
                        break;
                    case 0x09:                  //设置密码
                        Cmd_Datalength = buf[2];
                        strncpy((char*)&Password,(char*)&buf[3],Cmd_Datalength);  
                        Passwordflag = 1;
//                        printf("Passwordflag:%d\r\n",Passwordflag);
                        Data_Storage = 11;
                        break;
                    case 0x0A:                  //解锁密码
                        Cmd_Datalength = buf[2];
                        strncpy((char*)&Unlock_Password,(char*)&buf[3],Cmd_Datalength);
                        if(strncmp(Unlock_Password,Password,Cmd_Datalength) == 0)  //进行比对，判断密码是否正确
                        {
                            Passwordflag = 0;
                            printf("ok");                         
                        }
                        Data_Storage = 10;
                        break;
                    case 0x0B:                  //回油时间
                        SpannerDate.p_OilReturnTime = buf[2];
                        Data_Storage = 12;
                        break;  
                    
                }
//                crc=GetCRC16(buf,len); //计算CRC校验值
//				buf[len++]=crc>>8;     //CRC高字节
//				buf[len++]=crc&0xff;   //CRC低字节 
//                UartWrite(buf,len);    //发送响应帧

            }
        }  
    }
    
}

//判断是否接收一帧数据
void UartRxMonitor(u8 ms) //串口接收监控
{
	static u8 USART1_RX_BKP=0;  //定义USART2_RC_BKP暂时存储诗句长度与实际长度比较
	static u8 idletmr=0;        //定义监控时间
	if(USART2_RX_CNT>0)//接收计数器大于零时，监控总线空闲时间
	{
		if(USART1_RX_BKP!=USART2_RX_CNT) //接收计数器改变，即刚接收到数据时，清零空闲计时
		{
			USART1_RX_BKP=USART2_RX_CNT;  //赋值操作，将实际长度给USART2_RX_BKP
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
					flagFrame=1;//设置命令到达标志，帧接收完毕标志                    
				}
			}
		}
	}
	else
	{
		USART1_RX_BKP=0;
	}
}
