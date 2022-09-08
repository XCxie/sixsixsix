#include "Driver_user_driver.h"

#define   Driver_SEND_DATA(P)   Driver_SendChar(P)            //����һ���ֽ�
#define   Driver_TX_8(P)        Driver_SEND_DATA((P)&0xFF)    //���͵����ֽ�
#define   Driver_TX_16(P)       Driver_TX_8((P)>>8);Driver_TX_8(P)   //����16λ����
#define   Driver_TX_32(P)       Driver_TX_16((P)>>16);Driver_TX_16(P)//����32λ����


u8  Data_send_Status = 0;
char Spanner_OilReturnTime = 0;//���ֻ���ʱ��

//��¼����
u32 Last_TargetTorque_value = 0;     //Ŀ��Ť��ֵ
u32 Last_BoltCoefficient_value = 0;//��˨Ť��
u32 Last_GearTorque_value = 0;//��λŤ��
u16 Last_Set_Target_Angle = 0;       //����Ԥ��Ƕ�ֵ
u32 Last_Set_SensorTorque = 0;     //���ô�����Ԥ��Ť��ֵ
u32 Last_Set_Target_Usage = 0;     //��������ʹ�õĴ���
u8  Last_Set_Control_Mode = 0;       //���ÿ���ģʽ
u8 Last_Language_Type = 0;       //������������   
u8 Last_Chart_Standard_Type = 0; //����ͼ���׼����
u8 Last_ProductStandard_Type = 0;//���ò�Ʒ�������
u8 Last_TargetTorque_unit = 0;   //����Ŀ��Ť�ص�λ����
u8 Last_SensorTorque_unit = 0;   //���ô�����Ԥ��Ť�ص�λ����
u8 Last_MaxTorque_unit = 0;      //�������Ť�ص�λ����
u8 Last_Pressure_Unit_Type = 0;  //����ѹ����λ���� 
u8 Last_Bolt_Diameter = 0;       //������˨ֱ��
u8 Last_Bolt_Garder = 0;         //������˨�ȼ�
u8 Last_BoltTargetTorque_unit = 0; //������˨Ŀ��Ť�ص�λ
u8 Last_Set_Target_Gear = 0;

//������������һ�β����Լ����������ı䷢�Ͳ���
void Send_DriverParameter()
{
    
    Spanner_OilReturnTime =  Spanner_type_OilReturnTime(Set_ProductStandard_Type); //��ȡ���ֻ���ʱ��
    if(Data_send_Status ==0)
    {
        Driver_Send32DataValue(0x01,Spanner_Coefficien);
        Driver_Send32DataValue(0x02,TargetTorque_value);
        Driver_Send32DataValue(0x03,BoltCoefficient_value);
        Driver_Send32DataValue(0x04,GearTorque_value);
        Driver_Send32DataValue(0x05,Set_Target_Angle);
        Driver_Send32DataValue(0x06,Set_SensorTorque);
        Driver_Send32DataValue(0x07,Set_Target_Usage);
        Driver_Send8DataValue(0x08,Set_Control_Mode);
        Driver_Send8DataValue(0x0B,Spanner_OilReturnTime);
//        printf("Spanner_Coefficien:%d\r\n",Spanner_Coefficien);
//        printf("TargetTorque_value:%d\r\n",TargetTorque_value);
//        printf("BoltCoefficient_value:%d\r\n",BoltCoefficient_value);
//        printf("GearTorque_value:%d\r\n",GearTorque_value);
//        printf("Set_Target_Angle:%d\r\n",Set_Target_Angle);
//        printf("Set_SensorTorque:%d\r\n",Set_SensorTorque);
//        printf("Set_Target_Usage:%d\r\n",Set_Target_Usage);
//        printf("Set_Control_Mode:%d\r\n",Set_Control_Mode);  
        Data_send_Status  = 1;
    }
    if(TargetTorque_value !=Last_TargetTorque_value)
    {
        Data_Storage = 11;
        Driver_Send32DataValue(0x02,TargetTorque_value);
        Last_TargetTorque_value = TargetTorque_value;
    }
    else if(BoltCoefficient_value != Last_BoltCoefficient_value)
    {
        Driver_Send32DataValue(0x03,BoltCoefficient_value);
        Last_BoltCoefficient_value = BoltCoefficient_value;
    }
    else if(GearTorque_value != Last_GearTorque_value)
    {
        Driver_Send32DataValue(0x04,GearTorque_value);
        Last_GearTorque_value = GearTorque_value;
    }
    else if(Set_Target_Gear != Last_Set_Target_Gear)
    {
        Data_Storage = 14;
        Last_Set_Target_Gear = Set_Target_Gear;
    }
    else if(Set_Target_Angle != Last_Set_Target_Angle)
    {
        Data_Storage = 15;
        Driver_Send32DataValue(0x05,Set_Target_Angle);
        Last_Set_Target_Angle = Set_Target_Angle;
    }
    else if(Set_SensorTorque != Last_Set_SensorTorque)
    {
        Data_Storage = 16;
        Driver_Send32DataValue(0x06,Set_SensorTorque);
        Last_Set_SensorTorque = Set_SensorTorque;
    }
    else if(Set_Target_Usage != Last_Set_Target_Usage)
    {
        Data_Storage = 17;
        Driver_Send32DataValue(0x07,Set_Target_Usage);
        Last_Set_Target_Usage = Set_Target_Usage;
    }
    else if(Set_Control_Mode != Last_Set_Control_Mode)
    {
        Data_Storage = 18;
        Driver_Send8DataValue(0x08,Set_Control_Mode);
        Last_Set_Control_Mode = Set_Control_Mode;
    } 
    else if(Set_Language_Type != Last_Language_Type)
    {
        Data_Storage = 19;
        Last_Language_Type = Set_Language_Type;
    }
    else if(Set_Chart_Standard_Type != Last_Chart_Standard_Type)
    {
        Data_Storage = 20;
        Last_Chart_Standard_Type = Set_Chart_Standard_Type;
    }
    else if(Set_ProductStandard_Type != Last_ProductStandard_Type)
    {
        
        Driver_Send32DataValue(0x01,Spanner_Coefficien);
        Driver_Send8DataValue(0x0B,Spanner_OilReturnTime);
        Data_Storage = 21;
        Last_ProductStandard_Type = Set_ProductStandard_Type;
    }
    else if(Set_Torque_Unit != Last_TargetTorque_unit)
    {
        Data_Storage = 22;
        Last_TargetTorque_unit = Set_Torque_Unit;
    }
    else if(Set_Pressure_Unit_Type != Last_Pressure_Unit_Type)
    {
        Data_Storage = 25;
        Last_Pressure_Unit_Type = Set_Pressure_Unit_Type;
    }
    else if(Set_Bolt_Diameter != Last_Bolt_Diameter)
    {
        Data_Storage = 26;
        Last_Bolt_Diameter = Set_Bolt_Diameter;
    }
    else if(Set_Bolt_Garder != Last_Bolt_Garder)
    {
        Data_Storage = 27;
        Last_Bolt_Garder = Set_Bolt_Garder;
    }

 
     
}

/*! 
*  \brief     �����ַ�������
*  \param  function    ����λ
*  \param  pbuffer    �ַ���
*  \param  length    ����
*/
void Driver_SendChardata(uint8 function,char *pbuffer,u8 length)
{
    u8 buff[255] ={0};
    u8 i =0;
    u16 crc = 0;
    u8 len = 0;
    buff[0] = 0xA5;
    buff[1] = function;
    buff[2] = length ;
    for(i = 0;i < length;i++)
    {
        buff[3+i] = pbuffer[i];
    }
//    strncpy((char*)&buff[3],pbuffer,length);
    len = 3+length;
    crc=GetCRC16(buff,len);           //����CRCУ��ֵ
    buff[len++] = crc>>8 & 0xff;
    buff[len++] = crc    & 0xff;
    Driver_UartWrite(buff,len);  
    OSTimeDly(2);  
}

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
    buff[0] = 0xA5;
    buff[1] = function;
    buff[2] = (value >> 24) & 0xff;
    buff[3] = (value >> 16) & 0xff;
    buff[4] = (value >> 8)  & 0xff;
    buff[5] =  value        & 0xff;
    len = 6;
    crc=GetCRC16(buff,len);           //����CRCУ��ֵ
    buff[len++] = crc>>8 & 0xff;
    buff[len++] = crc    & 0xff;
    Driver_UartWrite(buff,len);  
    OSTimeDly(20);  
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
    buff[0] = 0xA5;
    buff[1] = function;
    buff[2] =  value        & 0xff;
    len = 3;
    crc=GetCRC16(buff,len);           //����CRCУ��ֵ
    buff[len++] = crc>>8 & 0xff;
    buff[len++] = crc    & 0xff;
    Driver_UartWrite(buff,len);  
    OSTimeDly(20);  
}