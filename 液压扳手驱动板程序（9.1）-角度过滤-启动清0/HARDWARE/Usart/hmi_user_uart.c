#include "hmi_user_uart.h"	
#include "gpio.h"
#include "TASK.h"
#include "usart.h"	
#include "crc16.h"
#include "Data_process.h"
#include "MX25L6406.h"
u8 USART2_RX_BUF[255];                   //���ջ��壬���64�ֽ�
u8 USART2_RX_CNT=0;                       //�����ֽڼ�����
u8 flagFrame=0;                         //֡������ɱ�־�������յ�һ֡������
/*!
*   \brief   ����4��ʼ��
*   \param   BaudRate-����������
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
    
    USART_DeInit(UART4);                                           //��λUSART4
    USART_InitStructure.USART_BaudRate = BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode =   USART_Mode_Tx|USART_Mode_Rx;

    /* USART configuration */
    USART_Init(UART4, &USART_InitStructure);
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);                  //�����ж�ʹ��

    /* Enable USART */
    USART_Cmd(UART4, ENABLE);
}


/*!
*   \brief  ����1���ֽ�
*   \param  t ���͵��ֽ�
*/
void  SendChar(uchar t)
{
    USART_SendData(UART4,t);
    while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
    while((USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET));//�ȴ����ڷ������
}

 void UART4_IRQHandler(void)
{
    u8 res;	
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//���յ�����
	{	 	
		res =USART_ReceiveData(UART4);//;��ȡ���յ�������USART2->DR
		if(USART2_RX_CNT<sizeof(USART2_RX_BUF))    //һ��ֻ�ܽ���64���ֽڣ���Ϊ�趨�����Ը��󣬵��˷�ʱ��
		{
			USART2_RX_BUF[USART2_RX_CNT]=res;  //��¼���յ���ֵ           
			USART2_RX_CNT++;        //ʹ����������1 
		}
	}

}
//���㷢�͵����ݳ��ȣ����ҽ����ݷŵ�*buf������                     
u8 UartRead(u8 *buf, u8 len)  
{
	 u8 i;
	if(len>USART2_RX_CNT)  //ָ����ȡ���ȴ���ʵ�ʽ��յ������ݳ���ʱ
	{
		len=USART2_RX_CNT; //��ȡ��������Ϊʵ�ʽ��յ������ݳ���
	}
	for(i=0;i<len;i++)  //�������յ������ݵ�����ָ����
	{
		*buf=USART2_RX_BUF[i];  //�����ݸ��Ƶ�buf��
		buf++;
	}
	USART2_RX_CNT=0;              //���ռ���������
	return len;                   //����ʵ�ʶ�ȡ����
}

u8 UartWrite(u8 *buf ,u8 len) 										//����
{
	u8 i=0;                                                              //3MS��ʱ   
    for(i=0;i<len;i++)
    {
        USART_SendData(UART4,buf[i]);	                                      //ͨ��USARTx���跢�͵�������
        while(USART_GetFlagStatus(UART4,USART_FLAG_TXE)==RESET);             //���ָ����USART��־λ������񣬷������ݿ�λ��־
    }
//	return 0;
}
u8 Passwordflag = 0;
u8 Controlflag = 0;
char Password[8] = "123456";//�趨������
char Unlock_Password[8] = "123456";  //��������
//���������������������֡�Ľ��գ����ȹ��ܺ�����������ѭ���е���
void UartDriver()
{
    u16 len;
    u8 Cmd_Datalength = 0;
    unsigned int crc;
	unsigned char crch,crcl;
    u8 buf[255]; 
    if(flagFrame)            //֡������ɱ�־�������յ�һ֡������
	{   
        flagFrame=0;           //֡������ɱ�־����
		len = UartRead(buf,sizeof(buf));   //�����յ������������������
        if(buf[0] == cmd_head)                     //�ж�֡ͷ�Ƿ���ȷ
        {
            crc=GetCRC16(buf,len-2);         // ��ȥCRCУ��ֵ   ����CRCУ��ֵ
            crch=crc>>8;    				//crc��λ
            crcl=crc&0xFF;
            printf("crc:%d\r\n",crc);
            if((buf[len-2]==crch)&&(buf[len-1]==crcl))  //�ж�CRCУ���Ƿ���ȷ
            {
                switch (buf[1])  //��������ִ�в���
                {
                    case 0x01:                  //����ϵ��
                        SpannerDate.p_SpannerCoefficient = PTR2U32(&buf[2]);
                        Data_Storage = 1;
                        
                        break;
                    case 0x02:                  //�û��趨Ť��
                        SpannerDate.p_UserTargetTorque = PTR2U32(&buf[2]);
                        Data_Storage = 2;
                        
                        break;
                    case 0x03:                  //��˨Ť��                       
                        SpannerDate.p_BoltTorque = PTR2U32(&buf[2]);
                        Data_Storage = 3;
                        
                        break;
                    case 0x04:                  //��λŤ��
                        
                        SpannerDate.p_GearTorque = PTR2U32(&buf[2]);
                        Data_Storage = 4;
                        
                        break;
                    case 0x05:                  //Ԥ��Ƕ�
                        
                        SpannerDate.p_TargetAngle = PTR2U32(&buf[2]);
                        Data_Storage = 5;
                        
                        break;
                    case 0x06:                  //Ԥ�贫����Ť��
                        SpannerDate.p_TargetSensor = PTR2U32(&buf[2]);
                        Data_Storage = 6;                   
                        break;
                    case 0x07:                  //�趨ʹ�ô���
                        SpannerDate.p_TargetUsageNumber = PTR2U32(&buf[2]);
                        Data_Storage = 7;
                        break;
                    case 0x08:                  //ģʽ
                        SpannerDate.p_Mode = buf[2];
                        Data_Storage = 8;
                        break;
                    case 0x09:                  //��������
                        Cmd_Datalength = buf[2];
                        strncpy((char*)&Password,(char*)&buf[3],Cmd_Datalength);  
                        Passwordflag = 1;
//                        printf("Passwordflag:%d\r\n",Passwordflag);
                        Data_Storage = 11;
                        break;
                    case 0x0A:                  //��������
                        Cmd_Datalength = buf[2];
                        strncpy((char*)&Unlock_Password,(char*)&buf[3],Cmd_Datalength);
                        if(strncmp(Unlock_Password,Password,Cmd_Datalength) == 0)  //���бȶԣ��ж������Ƿ���ȷ
                        {
                            Passwordflag = 0;
                            printf("ok");                         
                        }
                        Data_Storage = 10;
                        break;
                    case 0x0B:                  //����ʱ��
                        SpannerDate.p_OilReturnTime = buf[2];
                        Data_Storage = 12;
                        break;  
                    
                }
//                crc=GetCRC16(buf,len); //����CRCУ��ֵ
//				buf[len++]=crc>>8;     //CRC���ֽ�
//				buf[len++]=crc&0xff;   //CRC���ֽ� 
//                UartWrite(buf,len);    //������Ӧ֡

            }
        }  
    }
    
}

//�ж��Ƿ����һ֡����
void UartRxMonitor(u8 ms) //���ڽ��ռ��
{
	static u8 USART1_RX_BKP=0;  //����USART2_RC_BKP��ʱ�洢ʫ�䳤����ʵ�ʳ��ȱȽ�
	static u8 idletmr=0;        //������ʱ��
	if(USART2_RX_CNT>0)//���ռ�����������ʱ��������߿���ʱ��
	{
		if(USART1_RX_BKP!=USART2_RX_CNT) //���ռ������ı䣬���ս��յ�����ʱ��������м�ʱ
		{
			USART1_RX_BKP=USART2_RX_CNT;  //��ֵ��������ʵ�ʳ��ȸ�USART2_RX_BKP
			idletmr=0;                    //�����ʱ������
		}
		else                              ////���ռ�����δ�ı䣬�����߿���ʱ���ۼƿ���ʱ��
		{
			//�����һ֡�������֮ǰ�г���3.5���ֽ�ʱ���ͣ�٣������豸��ˢ�µ�ǰ����Ϣ���ٶ���һ���ֽ���һ���µ�����֡�Ŀ�ʼ
			if(idletmr<5)                  //����ʱ��С��1msʱ�������ۼ�
			{
				idletmr +=ms;
				if(idletmr>=5)             //����ʱ��ﵽ1msʱ�����ж�Ϊ1֡�������
				{
					flagFrame=1;//����������־��֡������ϱ�־                    
				}
			}
		}
	}
	else
	{
		USART1_RX_BKP=0;
	}
}
