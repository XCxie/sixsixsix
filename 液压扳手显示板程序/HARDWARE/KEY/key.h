#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

//STM32F103核心板例程
//库函数版本例程
/************** 嵌入式开发网  **************/
/********** mcudev.taobao.com 出品  ********/

//////////////////////////////////////////////////////////////////////////////////	 

//STM32开发板
//按键驱动代码	   
							  
//////////////////////////////////////////////////////////////////////////////////   	 


#define KEY1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//读取按键K1


void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8);  	//按键扫描函数					    
#endif
