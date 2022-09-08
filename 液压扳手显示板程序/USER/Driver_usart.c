#include "Driver_usart.h"	
#include "gpio.h"
#include "usart.h"	
#include "Data_process.h"
#include "TASK.h"



u8 USART3_RX_BUF[64];                   //���ջ��壬���64�ֽ�
u8 USART3_RX_CNT=0;                       //�����ֽڼ�����
u8 Driver_flagFrame=0;                         //֡������ɱ�־�������յ�һ֡������

u8 RecordCount = 0;       //����״̬��¼
float Collect_Pressure = 0;  //����ÿ�δ���˨ʱ�����ѹ��
u16 Collect_Angle = 0;  //
/*!
*   \brief   ����4��ʼ��
*   \param   BaudRate-����������
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
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�2
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
void  Driver_SendChar(uchar t)
{
    USART_SendData(UART4,t);
    while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
    while((USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET));//�ȴ����ڷ������
}

 void UART4_IRQHandler(void)
{
    u8 res;	                                    //�������ݻ������ 
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//���յ�����
	{	 
		res =USART_ReceiveData(UART4);//;��ȡ���յ�������USART2->DR
		if(USART3_RX_CNT<sizeof(USART3_RX_BUF))    //һ��ֻ�ܽ���64���ֽڣ���Ϊ�趨�����Ը��󣬵��˷�ʱ��
		{
			USART3_RX_BUF[USART3_RX_CNT]=res;  //��¼���յ���ֵ           
			USART3_RX_CNT++;        //ʹ����������1 
		}
	}

}
//���㷢�͵����ݳ��ȣ����ҽ����ݷŵ�*buf������                     
u8 Driver_UartRead(u8 *buf, u8 len)  
{
	 u8 i;
	if(len>USART3_RX_CNT)  //ָ����ȡ���ȴ���ʵ�ʽ��յ������ݳ���ʱ
	{
		len=USART3_RX_CNT; //��ȡ��������Ϊʵ�ʽ��յ������ݳ���
	}
	for(i=0;i<len;i++)  //�������յ������ݵ�����ָ����
	{
		*buf=USART3_RX_BUF[i];  //�����ݸ��Ƶ�buf��
		buf++;
	}
	USART3_RX_CNT=0;              //���ռ���������
	return len;                   //����ʵ�ʶ�ȡ����
}

u8 Driver_UartWrite(u8 *buf ,u8 len) 										//����
{
	u8 i=0;    
    for(i=0;i<len;i++)
    {
        USART_SendData(UART4,buf[i]);	                                      //ͨ��USARTx���跢�͵�������
        while(USART_GetFlagStatus(UART4,USART_FLAG_TXE)==RESET);             //���ָ����USART��־λ������񣬷������ݿ�λ��־
    }

}

//���������������������֡�Ľ��գ����ȹ��ܺ�����������ѭ���е���
void UartDriver()
{
    unsigned char i=0;
    u16 len;
    float Pressure_temp = 0;
    unsigned int crc;
	unsigned char crch,crcl;
    u8 buf[64]; 
    u32 data;  
    if(Driver_flagFrame)            //֡������ɱ�־�������յ�һ֡������
    {   
        Driver_flagFrame = 0;
        len = Driver_UartRead(buf,sizeof(buf));   //�����յ������������������
        if(buf[0] == 0x5A)                     //�ж�֡ͷ�Ƿ���ȷ
        {
            crc=GetCRC16(buf,len-2);         // ��ȥCRCУ��ֵ   ����CRCУ��ֵ
            crch=crc>>8;    				//crc��λ
            crcl=crc&0xFF;
//            printf("crc:%d\r\n",crc);
            if((buf[len-2]==crch)&&(buf[len-1]==crcl))  //�ж�CRCУ���Ƿ���ȷ
            {
                switch (buf[1])  //��������ִ�в���
                {            
                    case 0x01:                  //ѹ��ֵ
                        Pressure_Value = (PTR2U32(&buf[2])/100.0);
                        if(DriveControl_status == 0 && RecordCount ==0)    //�ֶ�������
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
                    case 0x02:                  //ʹ�ô���
                        UsageCounter = PTR2U32(&buf[2]);
                        break;
                    case 0x03:                  //����������״̬  
                        RecordCount = buf[2];
                        //printf("RecordCount:%d\r\n",RecordCount);
                        break;
                    case 0x04:                  //����״̬ �ֶ�/�Զ�       
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

//�ж��Ƿ����һ֡����
void UartRxMonitor(u8 ms) //���ڽ��ռ��
{
	static u8 USART3_RX_BKP=0;  //����USART2_RC_BKP��ʱ�洢ʫ�䳤����ʵ�ʳ��ȱȽ�
	static u8 idletmr=0;        //������ʱ��
	if(USART3_RX_CNT>0)//���ռ�����������ʱ��������߿���ʱ��
	{
		if(USART3_RX_BKP!=USART3_RX_CNT) //���ռ������ı䣬���ս��յ�����ʱ��������м�ʱ
		{
			USART3_RX_BKP=USART3_RX_CNT;  //��ֵ��������ʵ�ʳ��ȸ�USART2_RX_BKP
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
					Driver_flagFrame=1;//����������־��֡������ϱ�־                    
				}
			}
		}
	}
	else
	{
		USART3_RX_BKP=0;
	}
}