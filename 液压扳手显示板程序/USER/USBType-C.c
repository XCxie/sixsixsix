#include "USBType-C.h" 
Simple1 Gears_data[Gear_MUN];


u8 USART2_RX_BUF[2048];                   //���ջ��壬���64�ֽ�
u16 USART2_RX_CNT=0;                       //�����ֽڼ�����
u8 TypeC_flagFrame=0;                         //֡������ɱ�־�������յ�һ֡������
/*!
*   \brief   ����2��ʼ��
*   \param   BaudRate-����������
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
    
    USART_DeInit(USART2);                                           //��λUSART2
    USART_InitStructure.USART_BaudRate = BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode =   USART_Mode_Tx|USART_Mode_Rx;

    /* USART configuration */
    USART_Init(USART2, &USART_InitStructure);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                  //�����ж�ʹ��

    /* Enable USART */
    USART_Cmd(USART2, ENABLE);
}

void USART2_IRQHandler(void)
{
    u8 res;	
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        res = USART_ReceiveData(USART2);
        if(USART2_RX_CNT<sizeof(USART2_RX_BUF))    //һ��ֻ�ܽ���1024���ֽڣ���Ϊ�趨�����Ը��󣬵��˷�ʱ��
        {
            USART2_RX_BUF[USART2_RX_CNT]=res;  //��¼���յ���ֵ           
            USART2_RX_CNT++;        //ʹ����������1 
        } 
    }
}

/*!*   \brief  ����1���ֽ�
*   \param  t ���͵��ֽ�
*/
void  TypeC_SendChar(uchar t)
{
    USART_SendData(USART2,t);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    while((USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET));//�ȴ����ڷ������
}

//���㷢�͵����ݳ��ȣ����ҽ����ݷŵ�*buf������                     
u16 TypeC_UartRead(u8 *buf, u16 len)  
{
	u16 i;
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


u8 TypeC_UartWrite(u8 *buf ,u16 len) 										//����
{
	u16 i=0;    
    for(i=0;i<len;i++)
    {
        USART_SendData(USART2,buf[i]);	                                      //ͨ��USARTx���跢�͵�������
        while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);             //���ָ����USART��־λ������񣬷������ݿ�λ��־
    }

}
u8 Set_Factory_Number[20] ={0};//�豸���߳������
u8 Device_name[20] ={0};    //�û��豸����
u8 Set_Equipment_Number[20] ={0};//�û��豸���
u8 Operator_name[20] ={0};   //������Ա


u16 Spanner_Number = 0;  //��������
char Password[6] = "123456";//�趨������
char Unlock_Password[6];  //��������

u8 Data_Storage = 0; //���ݴ洢��־λ

u8 Factory_Number_length = 0;
u8 Device_name_length = 0;
u8 Equipment_Number_length = 0;
u8 Operator_name_length = 0;
u8 Gear_length = 7;



u8 Blot_addressNumber = 0;
//���������������������֡�Ľ��գ����ȹ��ܺ�����������ѭ���е���
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
    if(TypeC_flagFrame)            //֡������ɱ�־�������յ�һ֡������
	{
		TypeC_flagFrame=0;           //֡������ɱ�־����
		len = TypeC_UartRead(buf,sizeof(buf));   //�����յ������������������
        printf("len:%d\r\n",len);
        if(buf[0] == 0x5A && buf[1] == 0xA5)     //֡ͷ
        {
            crc=GetCRC16(buf,len-2);       //����CRCУ��ֵ����ȥCRCУ��ֵ
            printf("crc:%x\r\n",crc);
			crch=crc>>8;    				//crc��λ
			crcl=crc&0xFF;					//crc��λ
            if((buf[len-2]==crch)&&(buf[len-1]==crcl))  //�ж�CRCУ���Ƿ���ȷ
            {				 
                switch(buf[4])    //������
                {
                    case 0x01:    //����ʱ��
                        RTC_EnterConfigMode();     //��������
                        calendar.w_year = buf[7]<<8|buf[8];
                        calendar.w_month = buf[9];
                        calendar.w_date = buf[10];
                        calendar.hour = buf[11];
                        calendar.min = buf[12];
                        calendar.sec = buf[13] + 1;
                        RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);                    
                        RTC_ExitConfigMode();      //�˳�����ģʽ 
                        len -= 11;
                        buf[len++] = 0x00;
                        buf[len++] = 0x00;
                        break;
                    case 0x03:    //���ó������                       
                        Factory_Number_length = (buf[5]<<8|buf[6]);    //��ȡ�豸�������ݳ���                       
                        for(i = 0;i<Factory_Number_length ;i++)            //��ȡ�������
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
                    case 0x05:    //�����û��豸����                                                
                        Device_name_length = (buf[5]<<8|buf[6]);        //��ȡ�豸�������ݳ���
                        for(i = 0;i<Device_name_length ;i++)            //��ȡ�����豸����
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
                    case 0x07:    //�����豸���                      
                        Equipment_Number_length = (buf[5]<<8|buf[6]);        //��ȡ�豸������ݳ���                       
                        for(i = 0;i<Equipment_Number_length ;i++)            //��ȡ�����豸���
                        {
                            Set_Equipment_Number[i] = buf[i+7];     
                            printf("%c",Set_Equipment_Number[i]);
                        }
                        Data_Storage = 4;
                        len -= Equipment_Number_length+4;  //��Ӧ֡����
                        buf[len++] = 0x00;    //����λ���
                        buf[len++] = 0x00;
                        break;
                    case 0x08:
                        len -= 2;
                        break;
                    case 0x09:    //����
                        Cmd_Datalength = (buf[5]<<8|buf[6]) - 1;    //��ȡ�������ݳ���
                        if(buf[7] == 0)
                        {
                            Driver_SendChardata(0x0A,(char*)&buf[8],Cmd_Datalength);  //�������巢�ͽ������� 
                        }                            
                        len -= 2;
                        break;
                    case 0x10:
                        len -= 2;
                        break;
                    case 0x0A:    //���ý�������
                        Cmd_Datalength = (buf[5]<<8|buf[6]) ;    //��ȡ�����������ݳ���
                        Driver_SendChardata(0x09,(char*)&buf[7],Cmd_Datalength);  //�������巢�ͽ�������
                        buf[5] = 0x00;              //���͵����ݳ���
                        buf[6] = 0x00;                     
                        len = 7;
                        break;
                    case 0x0B:    //����ʹ�ô���                  
                        Driver_Send32DataValue(0x07,PTR2U32(&buf[7]));//�������巢�����ƴ���
                        buf[5] = 0x00;              //���͵����ݳ���
                        buf[6] = 0x00;                     
                        len = 7;
                        break;
                    case 0x0F:    //���ò�����Ա                        
                        Operator_name_length = (buf[5]<<8|buf[6]);    //��ȡ�豸�������ݳ���                       
                        for(i = 0;i<Operator_name_length ;i++)            //��ȡ�����豸����
                        {
                            Operator_name[i] = buf[i+7];                            
                            printf("%c",Operator_name[i]);
                        } 
                        Data_Storage = 5; 
                        len -= Operator_name_length+4;
                        buf[len++] = 0x00;
                        buf[len++] = 0x00;
                        break;
                    case 0x0E:    //��������
                                           
                    break;
                    case 0x11:         //Ť��ģʽ  
                        Spanner_Number = buf[7]<<8|buf[8];    //��ȡ������ָ���  
                        for(cnt = 0;cnt < Spanner_Number;cnt++)
                        {                                                       
                            strncpy(SpannerType[cnt].type,(char*)&buf[cnt*15+9],10);     
									 printf("type == %s\r\n",SpannerType[cnt].type);
                            SpannerType[cnt].parameter = PTR2U32(&buf[cnt*15+19]);
									 printf("parameter == %d\r\n",SpannerType[cnt].parameter);
                            SpannerType[cnt].Delay_time = buf[cnt*15+23];
                        }
                        Data_Storage = 1; 
                        buf[5] = 0x00;              //���͵����ݳ���
                        buf[6] = 0x00;  
                        len  = 7; 
                    break;
                    case 0x12:         //��˨ģʽ   
                         for(cnt = 0;cnt < Spanner_Number;cnt++)
                            {
                                if(strcmp(SpannerType[cnt].type,(char*)&buf[7]) == 0)
                                {
                                    Blot_addressNumber = cnt;
                                    printf("Blot_addressNumber:%d\r\n",Blot_addressNumber);  
                                    //����
                                    if(buf[17] == 0x00)
                                    {
                                            SpannerType[cnt].Diameter_Metric = buf[18];//����ֱ������ 
                                            //������˨����
                                            for(i = 0;i < SpannerType[cnt].Diameter_Metric;i++)
                                            {
                                                strncpy((char*)BoltType.Metric_Diameter[i],(char*)&buf[i*10+19],10); 
                                                printf("%s\r\n",BoltType.Metric_Diameter[i]);                                                
                                            }
                                            
                                            SpannerType[cnt].Grade_Metric = buf[19+SpannerType[cnt].Diameter_Metric*10];  //���Ƶȼ���
                                             //���Ƶȼ�����
                                            for(i = 0;i < SpannerType[cnt].Grade_Metric;i++)
                                            {
                                                strncpy((char*)BoltType.Metric_Grade[i],(char*)&buf[20+(SpannerType[cnt].Diameter_Metric+i)*10],10); 
                                                printf("%s\r\n",BoltType.Metric_Grade[i]);  
                                            }
                                            //��˨Ť��
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
                                    Cmd_adress = 20 + (SpannerType[cnt].Diameter_Metric + SpannerType[cnt].Grade_Metric) * 10 + (SpannerType[cnt].Diameter_Metric * SpannerType[cnt].Grade_Metric)*4;  //Ӣ����ʼλ��
//                                    printf("Cmd_adress��%d\r\n",Cmd_adress);  
                                    //Ӣ��
                                     if(buf[Cmd_adress] ==0x01)   
                                    {
                                            SpannerType[cnt].Diameter_Imperial = buf[Cmd_adress+1];//Ӣ��ֱ������
                                            //Ӣ����˨����
                                            for(i = 0;i < SpannerType[cnt].Diameter_Imperial;i++)
                                            {
                                                strncpy((char*)BoltType.Imperial_Diameter[i],(char*)&buf[i*10+Cmd_adress+2],10); 
                                                printf("%s\r\n",BoltType.Imperial_Diameter[i]);                                                  
                                            }
                                            SpannerType[cnt].Grade_Imperial = buf[Cmd_adress+2+SpannerType[cnt].Diameter_Imperial*10];  //���Ƶȼ���
                                             //Ӣ�Ƶȼ�����
                                            for(i = 0;i < SpannerType[cnt].Grade_Imperial;i++)
                                            {
                                                strncpy((char*)BoltType.Imperial_Grade[i],(char*)&buf[Cmd_adress+3+SpannerType[cnt].Diameter_Imperial*10+i*10],10);
                                                printf("%s\r\n",BoltType.Imperial_Grade[i]);  
                                                printf("ok\r\n"); 
                                            }
                                            //��˨Ť��
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
                        buf[5] = 0x00;              //���͵����ݳ���
                        buf[6] = 0x00;  
                        len  = 7;                             
                    break;
                    case 0x13:         //�Ƕ�ģʽ                       
                        Max_Angle = buf[7]<<8|buf[8];
                      //  printf("Max_Angle:%d\r\n",Max_Angle);  
                        Data_Storage = 6; 
                        buf[5] = 0x00;              //���͵����ݳ���
                        buf[6] = 0x00; 
                        len = 7;
                    break;
                    case 0x14:         //��λģʽ                 
                        Gear_length = buf[7];  //��ȡ��λ����
                        for(i = 0;i<Gear_length ;i++)            
                        {
                             Gears_data[i].Levels = buf[i*5+8];      //��λ�ȼ�1�ֽ�
                             Gears_data[i].parameter = PTR2U32(&buf[i*5+9]);//��λѹ��4�ֽ�
                        } 
                        Data_Storage = 7; 
                        buf[5] = 0x00;              //���͵����ݳ���
                        buf[6] = 0x00; 
                        len  = 7; 
                       
                    break;
                    case 0x15:         //������ģʽ                 
                        DatalengthU16 = buf[7]<<8|buf[8];    //��ȡ�������ݸ���  
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
                        buf[5] = 0x00;              //���͵����ݳ���
                        buf[6] = 0x00;  
                        len  = 7; 
                    break;          
                }
                crc=GetCRC16(buf,len);           //����CRCУ��ֵ
				buf[len++]=crc>>8;           //CRC���ֽ�
				buf[len++]=crc&0xff;        //CRC���ֽ� 
                TypeC_UartWrite(buf,len);  //������Ӧ֡
            }
        
        }
    }
}




//�ж��Ƿ����һ֡����
void Uart2RxMonitor(u8 ms) //���ڽ��ռ��
{
	static u16 USART2_RX_BKP=0;  //����USART2_RC_BKP��ʱ�洢ʫ�䳤����ʵ�ʳ��ȱȽ�
	static u8 idletmr=0;        //������ʱ��
	if(USART2_RX_CNT>0)//���ռ�����������ʱ��������߿���ʱ��
	{
		if(USART2_RX_BKP!=USART2_RX_CNT) //���ռ������ı䣬���ս��յ�����ʱ��������м�ʱ
		{
			USART2_RX_BKP=USART2_RX_CNT;  //��ֵ��������ʵ�ʳ��ȸ�USART2_RX_BKP
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
					TypeC_flagFrame=1;//����������־��֡������ϱ�־                    
				}
			}
		}
	}
	else
	{
		USART2_RX_BKP=0;
	}
}