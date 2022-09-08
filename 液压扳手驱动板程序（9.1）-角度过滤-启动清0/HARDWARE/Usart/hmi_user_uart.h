#ifndef _USER_UART__
#define _USER_UART__
#include "stm32f10x_it.h"     //根据用户MCU进行修改
#include "includes.h"
extern u8 flagFrame;
extern char Password[8];//设定的密码
extern char Unlock_Password[8];  //解锁密码

#define cmd_head   0xA5
#define PTR2U16(PTR) ((((uint8 *)(PTR))[0]<<8)|((uint8 *)(PTR))[1])  //从缓冲区取16位数据
#define PTR2U32(PTR) ((((uint8 *)(PTR))[0]<<24)|(((uint8 *)(PTR))[1]<<16)|(((uint8 *)(PTR))[2]<<8)|((uint8 *)(PTR))[3])  //从缓冲区取32位数据

/****************************************************************************
* 名    称： UartInit()
* 功    能： 串口初始化
* 入口参数： 无
* 出口参数： 无
****************************************************************************/
void Uart4Init(uint32 Baudrate);

/*****************************************************************
* 名    称： SendChar()
* 功    能： 发送1个字节
* 入口参数： t  发送的字节
* 出口参数： 无
*****************************************************************/
void  SendChar(uchar t);
void UartRxMonitor(u8 ms); //串口接收监控
void UartDriver(); //串口驱动函数
u8 UartWrite(u8 *buf2 ,u8 len2);  //串口发送数据
u8 UartRead(u8 *buf, u8 len) ;
extern u8 Controlflag;
extern u8 Passwordflag;
#endif
