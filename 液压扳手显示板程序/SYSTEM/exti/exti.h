#ifndef __EXTI_H
#define __EXIT_H	 
#include "sys.h"					    
#define KEY_A     GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)
#define KEY_B     GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)
#define KEY_C     GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)
void EXTIX_Init(void);//IO≥ı ºªØ
extern u8 EC11_status  ;  					    
#endif