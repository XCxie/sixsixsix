#include "hmi_user_uart_driver.h"
#include "crc16.h"
#include "hmi_user_uart.h"
#include "MX25L6406.h"
#include "Hydraulic_control.h"
#define   Driver_SEND_DATA(P)   SendChar(P)            //����һ���ֽ�
#define   Driver_TX_8(P)        Driver_SEND_DATA((P)&0xFF)    //���͵����ֽ�
#define   Driver_TX_16(P)       Driver_TX_8((P)>>8);Driver_TX_8(P)   //����16λ����
#define   Driver_TX_32(P)       Driver_TX_16((P)>>16);Driver_TX_16(P)//����32λ����


/*! 
*  \brief     ����32λ�����ݱ���
*  \param  function    ����λ
*  \param  address  ���ݵ�ַ
*  \param  value    ��ֵ
*/
void Driver_Send32DataValue(uint8 function,uint32 value)
{
    u8 buff[20];
    u16 crc;
    u8 len;
    buff[0] = 0x5A;
    buff[1] = function;
    buff[2] = (value >> 24) & 0xff;
    buff[3] = (value >> 16) & 0xff;
    buff[4] = (value >> 8)  & 0xff;
    buff[5] =  value        & 0xff;
    len = 6;
    crc=GetCRC16(buff,len);           //����CRCУ��ֵ
    buff[len++] = crc>>8 & 0xff;
    buff[len++] = crc    & 0xff;
    UartWrite(buff,len);  
    OSTimeDly(5);  
}

/*! 
*  \brief     ����8λ�����ݱ���
*  \param  function    ����λ
*  \param  address  ���ݵ�ַ
*  \param  value    ��ֵ
*/
void Driver_Send8DataValue(uint8 function,u8 value)
{
    u8 buff[20];
    u16 crc;
    u8 len;
    buff[0] = 0x5A;
    buff[1] = function;
    buff[2] =  value        & 0xff;
    len = 3;
    crc=GetCRC16(buff,len);           //����CRCУ��ֵ
    buff[len++] = crc>>8 & 0xff;
    buff[len++] = crc    & 0xff;
    UartWrite(buff,len);  
    OSTimeDly(5);  
}

//��������״̬������ʹ�ô���
void Send_StartUp_State(u8 State)
{
    static u8 Last_StartUp_State = 0;
    if(State != Last_StartUp_State )       //�ж�״̬�Ƿ����ı�
    {
        if(State == 1)
        {
                Usage_Counter += 1;             //ʹ�ô�����1                            
                Data_Storage = 9;               //�洢ʹ�ô���
        }
        Driver_Send8DataValue(0x03,State);//��������״̬����λ�� 
        Last_StartUp_State = State;       //��¼��һ�ε�״̬
        
    }
 
}
//����ÿ�δ�ѹѹ��
void Send_Pressure_Value(float value)
{
    static u32 Last_PressureValue = 0;
    u32 PressData = 0;
    PressData = value * 100.0;
    if(PressData != Last_PressureValue )       //�ж�״̬�Ƿ����ı�
    {
        //printf("PressData:%d\r\n",PressData); 
        Driver_Send32DataValue(0x05,PressData);//��������״̬����λ�� 
        Last_PressureValue = PressData;       //��¼��һ�ε�״̬      
    }
}
//����ת���Ƕ�
void Send_Angle_Value(float value)
{
    static u32 Last_AngleValue = 0;
    u32 AngleData = 0;
    AngleData = ROUND_TO_UINT32(value);
//    printf("Last_AngleValue:%d\r\n",Last_AngleValue);
    if(AngleData != Last_AngleValue )       //�ж�״̬�Ƿ����ı�
    {
       // printf("AngleData:%d\r\n",AngleData); 
        Driver_Send32DataValue(0x06,AngleData);//��������״̬����λ�� 
        Last_AngleValue = AngleData;       //��¼��һ�ε�״̬      
    }
}



//���Ϳ���״̬���Զ������ֶ�
void Send_Contrl_Status(u8 Status)
{
    static u8 Last_DriveControl_status = 0;
    if(Status != Last_DriveControl_status )      //�ж�״̬�Ƿ����ı�
    {
        Driver_Send8DataValue(0x04,Status);      //��������״̬����λ�� 
        Last_DriveControl_status = Status;       //��¼��һ�ε�״̬ 
    }
 
}

//������ʾ���ݣ�ʹ�ô�����ѹ��
void Send_DisplayData(u8 Status,u32 Counter,float Pressure)
{
    u32 PressureValue = 0;
    static u8 Last_Counter = 0;
    static u32 Last_Value = 0;
    PressureValue = Pressure*100.0; //�������ѹ��ֵ ���ڷ��͸�����
    if(Status == 0)
    {
        if(Counter != Last_Counter )       //�ж�״̬�Ƿ����ı�
        {
            Driver_Send32DataValue(0x02,Counter);//����ʹ�ô�������λ�� 
            Last_Counter = Counter;       //��¼��һ�ε�״̬ 
        }
    }
    else
    {
        if(PressureValue != Last_Value )       //�ж�״̬�Ƿ����ı�
        {
            Driver_Send32DataValue(0x01,PressureValue);  
//            printf("PressureValue:%d\r\n",PressureValue);
            Last_Value = PressureValue;       //��¼��һ�ε�״̬ 
        }
    }
}

