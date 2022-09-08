#ifndef __USBTypeC_H
#define __USBTypeC_H
#include "sys.h" 
#include "usart.h"
#include "crc16.h"
#include "gpio.h"
#include "rtc.h" 
#include "Data_process.h"
#include "Driver_user_driver.h"
extern u8 TypeC_flagFrame;
extern u16 Spanner_Number;
extern char Password[6];//设定的默认密码
extern char Unlock_Password[6];  //解锁密码

extern u8 Blot_addressNumber;//储存螺栓对应扳手位置
extern u8 Set_Factory_Number[20];//设备工具出厂编号
extern u8 Device_name[20];    //用户设备名称
extern u8 Set_Equipment_Number[20];//用户设备编号	
extern u8 Operator_name[20];   //操作人员

extern  u8 Data_Storage; //数据存储标志位
extern  u8 Factory_Number_length; 
extern  u8 Device_name_length;
extern  u8 Equipment_Number_length; 
extern  u8 Operator_name_length; 
extern u8 Gear_length ;

void TypeC_UartInit(uint32 BaudRate);
void  TypeC_SendChar(uchar t);    
u8 TypeC_UartWrite(u8 *buf ,u16 len);
u16 TypeC_UartRead(u8 *buf, u16 len);  
void Uart2RxMonitor(u8 ms); //串口接收监控
void TypeC_UartDriver();  //串口解析函数
#endif