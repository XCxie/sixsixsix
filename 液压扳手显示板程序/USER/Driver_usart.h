#ifndef _DRIVER_UART__
#define _DRIVER_UART__

#include "stm32f10x_it.h"     //根据用户MCU进行修改
#include "sys.h"
#include "crc16.h"
extern u8 Driver_flagFrame;
extern u8 RecordCount ;
extern float Collect_Pressure;
extern u16 Collect_Angle;
#define PTR2U16(PTR) ((((uint8 *)(PTR))[0]<<8)|((uint8 *)(PTR))[1])  //从缓冲区取16位数据
#define PTR2U32(PTR) ((((uint8 *)(PTR))[0]<<24)|(((uint8 *)(PTR))[1]<<16)|(((uint8 *)(PTR))[2]<<8)|((uint8 *)(PTR))[3])  //从缓冲区取32位数据

/****************************************************************************
* 名    称： UartInit()
* 功    能： 串口初始化
* 入口参数： 无
* 出口参数： 无
****************************************************************************/
void Driver_UartInit(uint32 Baudrate);

/*****************************************************************
* 名    称： Driver_SendChar()
* 功    能： 发送1个字节
* 入口参数： t  发送的字节
* 出口参数： 无
*****************************************************************/
void  Driver_SendChar(uchar t);
void UartRxMonitor(u8 ms); //串口接收监控
void UartDriver(); 
u8 Driver_UartWrite(u8 *buf2 ,u8 len2);  //串口发送数据
u8 Driver_UartRead(u8 *buf, u8 len) ;//串口接收数据


#endif
