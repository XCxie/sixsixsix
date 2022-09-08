#include "Driver_usart.h"	
#include "gpio.h"
#include "usart.h"	
#include "Data_process.h"
#include "TASK.h"



u8 USART3_RX_BUF[64];                   //接收缓冲，最大64字节
u8 USART3_RX_CNT=0;                       //接收字节计数器
u8 Driver_flagFrame=0;                         //帧接收完成标志，即接收到一帧新数据

u8 RecordCount = 0;       //启动状态记录
float Collect_Pressure = 0;  //扳手每次打螺栓时的最大压力
u16 Collect_Angle = 0;  //
/*!
*   \brief   串口4初始化
*   \param   BaudRate-波特率设置
*/
void Driver_UartInit(uint32 BaudRate)
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
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级2
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
void  Driver_SendChar(uchar t)
{
    USART_SendData(UART4,t);
    while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
    while((USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET));//等待串口发送完毕
}

 void UART4_IRQHandler(void)
{
    u8 res;	                                    //定义数据缓存变量 
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//接收到数据
	{	 
		res =USART_ReceiveData(UART4);//;读取接收到的数据USART2->DR
		if(USART3_RX_CNT<sizeof(USART3_RX_BUF))    //一次只能接收64个字节，人为设定，可以更大，但浪费时间
		{
			USART3_RX_BUF[USART3_RX_CNT]=res;  //记录接收到的值           
			USART3_RX_CNT++;        //使收数据增加1 
		}
	}

}
//计算发送的数据长度，并且将数据放到*buf数组中                     
u8 Driver_UartRead(u8 *buf, u8 len)  
{
	 u8 i;
	if(len>USART3_RX_CNT)  //指定读取长度大于实际接收到的数据长度时
	{
		len=USART3_RX_CNT; //读取长度设置为实际接收到的数据长度
	}
	for(i=0;i<len;i++)  //拷贝接收到的数据到接收指针中
	{
		*buf=USART3_RX_BUF[i];  //将数据复制到buf中
		buf++;
	}
	USART3_RX_CNT=0;              //接收计数器清零
	return len;                   //返回实际读取长度
}

u8 Driver_UartWrite(u8 *buf ,u8 len) 										//发送
{
	u8 i=0;    
    for(i=0;i<len;i++)
    {
        USART_SendData(UART4,buf[i]);	                                      //通过USARTx外设发送单个数据
        while(USART_GetFlagStatus(UART4,USART_FLAG_TXE)==RESET);             //检查指定的USART标志位设置与否，发送数据空位标志
    }

}

//串口驱动函数，检测数据帧的接收，调度功能函数，需在主循环中调用
void UartDriver()
{
    unsigned char i=0;
    u16 len;
    float Pressure_temp = 0;
    unsigned int crc;
	unsigned char crch,crcl;
    u8 buf[64]; 
    u32 data;  
    if(Driver_flagFrame)            //帧接收完成标志，即接收到一帧新数据
    {   
        Driver_flagFrame = 0;
        len = Driver_UartRead(buf,sizeof(buf));   //将接收到的命令读到缓冲区中
        if(buf[0] == 0x5A)                     //判断帧头是否正确
        {
            crc=GetCRC16(buf,len-2);         // 除去CRC校验值   计算CRC校验值
            crch=crc>>8;    				//crc高位
            crcl=crc&0xFF;
//            printf("crc:%d\r\n",crc);
            if((buf[len-2]==crch)&&(buf[len-1]==crcl))  //判断CRC校验是否正确
            {
                switch (buf[1])  //按功能码执行操作
                {            
                    case 0x01:                  //压力值
                        Pressure_Value = (PTR2U32(&buf[2])/100.0);
                        if(DriveControl_status == 0 && RecordCount ==0)    //手动且启动
                        {
                            if(Pressure_Value >=25 )
                            {
                                Pressure_temp = Pressure_Value + 2.5;
                                Pressure_Value = Pressure_temp;  
                            }
                            else if(Pressure_Value <=25 &&Pressure_Value >15)
                            {
                                Pressure_temp = Pressure_Value + 2;
                                Pressure_Value = Pressure_temp;                
                            }
                            else if(Pressure_Value <=15 &&Pressure_Value >8)
                            {
                                Pressure_temp = Pressure_Value + 3.5;
                                Pressure_Value = Pressure_temp; 
                            }
                            else if(Pressure_Value<=8 &&Pressure_Value >2)
                            {
                                Pressure_temp = Pressure_Value + 2.5;
                                Pressure_Value = Pressure_temp; 
                            }
                            else if(Pressure_Value<=2 &&Pressure_Value >0)
                            {
                                Pressure_temp = Pressure_Value + 1.5;
                                Pressure_Value = Pressure_temp; 
                            }
                        }
//                        printf("  Pressure_Value:%.2f\r\n",  Pressure_Value);
                        break;
                    case 0x02:                  //使用次数
                        UsageCounter = PTR2U32(&buf[2]);
                        break;
                    case 0x03:                  //驱动板启动状态  
                        RecordCount = buf[2];
                        //printf("RecordCount:%d\r\n",RecordCount);
                        break;
                    case 0x04:                  //控制状态 手动/自动       
                        DriveControl_status = buf[2];
                        //printf("DriveControl_status:%d\r\n",DriveControl_status);
                        break;
                    case 0x05:
                        Collect_Pressure = (PTR2U32(&buf[2])/100.0);
                        break;
                    case 0x06:
                        Collect_Angle = PTR2U32(&buf[2]);
                        break;
                    default:
                        break;
                }  
            }
        }   
    }
    
}

//判断是否接收一帧数据
void UartRxMonitor(u8 ms) //串口接收监控
{
	static u8 USART3_RX_BKP=0;  //定义USART2_RC_BKP暂时存储诗句长度与实际长度比较
	static u8 idletmr=0;        //定义监控时间
	if(USART3_RX_CNT>0)//接收计数器大于零时，监控总线空闲时间
	{
		if(USART3_RX_BKP!=USART3_RX_CNT) //接收计数器改变，即刚接收到数据时，清零空闲计时
		{
			USART3_RX_BKP=USART3_RX_CNT;  //赋值操作，将实际长度给USART2_RX_BKP
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
					Driver_flagFrame=1;//设置命令到达标志，帧接收完毕标志                    
				}
			}
		}
	}
	else
	{
		USART3_RX_BKP=0;
	}
}