#include "hmi_user_uart_driver.h"
#include "crc16.h"
#include "hmi_user_uart.h"
#include "MX25L6406.h"
#include "Hydraulic_control.h"
#define   Driver_SEND_DATA(P)   SendChar(P)            //发送一个字节
#define   Driver_TX_8(P)        Driver_SEND_DATA((P)&0xFF)    //发送单个字节
#define   Driver_TX_16(P)       Driver_TX_8((P)>>8);Driver_TX_8(P)   //发送16位整数
#define   Driver_TX_32(P)       Driver_TX_16((P)>>16);Driver_TX_16(P)//发送32位整数


/*! 
*  \brief     设置32位的数据变量
*  \param  function    功能位
*  \param  address  数据地址
*  \param  value    数值
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
    crc=GetCRC16(buff,len);           //计算CRC校验值
    buff[len++] = crc>>8 & 0xff;
    buff[len++] = crc    & 0xff;
    UartWrite(buff,len);  
    OSTimeDly(5);  
}

/*! 
*  \brief     设置8位的数据变量
*  \param  function    功能位
*  \param  address  数据地址
*  \param  value    数值
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
    crc=GetCRC16(buff,len);           //计算CRC校验值
    buff[len++] = crc>>8 & 0xff;
    buff[len++] = crc    & 0xff;
    UartWrite(buff,len);  
    OSTimeDly(5);  
}

//发送启动状态并储存使用次数
void Send_StartUp_State(u8 State)
{
    static u8 Last_StartUp_State = 0;
    if(State != Last_StartUp_State )       //判断状态是否发生改变
    {
        if(State == 1)
        {
                Usage_Counter += 1;             //使用次数加1                            
                Data_Storage = 9;               //存储使用次数
        }
        Driver_Send8DataValue(0x03,State);//发送启动状态给上位机 
        Last_StartUp_State = State;       //记录上一次的状态
        
    }
 
}
//发送每次打压压力
void Send_Pressure_Value(float value)
{
    static u32 Last_PressureValue = 0;
    u32 PressData = 0;
    PressData = value * 100.0;
    if(PressData != Last_PressureValue )       //判断状态是否发生改变
    {
        //printf("PressData:%d\r\n",PressData); 
        Driver_Send32DataValue(0x05,PressData);//发送启动状态给上位机 
        Last_PressureValue = PressData;       //记录上一次的状态      
    }
}
//发送转动角度
void Send_Angle_Value(float value)
{
    static u32 Last_AngleValue = 0;
    u32 AngleData = 0;
    AngleData = ROUND_TO_UINT32(value);
//    printf("Last_AngleValue:%d\r\n",Last_AngleValue);
    if(AngleData != Last_AngleValue )       //判断状态是否发生改变
    {
       // printf("AngleData:%d\r\n",AngleData); 
        Driver_Send32DataValue(0x06,AngleData);//发送启动状态给上位机 
        Last_AngleValue = AngleData;       //记录上一次的状态      
    }
}



//发送控制状态：自动或者手动
void Send_Contrl_Status(u8 Status)
{
    static u8 Last_DriveControl_status = 0;
    if(Status != Last_DriveControl_status )      //判断状态是否发生改变
    {
        Driver_Send8DataValue(0x04,Status);      //发送启动状态给上位机 
        Last_DriveControl_status = Status;       //记录上一次的状态 
    }
 
}

//发送显示数据：使用次数和压力
void Send_DisplayData(u8 Status,u32 Counter,float Pressure)
{
    u32 PressureValue = 0;
    static u8 Last_Counter = 0;
    static u32 Last_Value = 0;
    PressureValue = Pressure*100.0; //获得整数压力值 便于发送给屏板
    if(Status == 0)
    {
        if(Counter != Last_Counter )       //判断状态是否发生改变
        {
            Driver_Send32DataValue(0x02,Counter);//发送使用次数给上位机 
            Last_Counter = Counter;       //记录上一次的状态 
        }
    }
    else
    {
        if(PressureValue != Last_Value )       //判断状态是否发生改变
        {
            Driver_Send32DataValue(0x01,PressureValue);  
//            printf("PressureValue:%d\r\n",PressureValue);
            Last_Value = PressureValue;       //记录上一次的状态 
        }
    }
}

