#ifndef __SYS_H
#define __SYS_H	
#include "stm32f10x.h"
#include "stm32f10x_exti.h"

//0,不支持ucos
//1,支持ucos
#define SYSTEM_SUPPORT_UCOS		1		//定义系统文件夹是否支持UCOS
																	    
	 
//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

#define uchar    unsigned char
#define uint8    unsigned char
#define uint16   unsigned short int
#define uint32   unsigned long
#define int16    short int
#define int32    long
typedef enum {FALSE = 0, TRUE = !FALSE} bool;

void NVIC_Configuration(void);



#endif
