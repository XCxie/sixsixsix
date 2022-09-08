#ifndef __GPIO_H
#define __GPIO_H	 
#include "includes.h"
//LED
#define SYS_LED_PIN  GPIO_Pin_4
#define SYS_LED_GPIO_Port GPIOC
#define SYS_LED_OFF    GPIO_SetBits(SYS_LED_GPIO_Port,SYS_LED_PIN)
#define SYS_LED_ON     GPIO_ResetBits(SYS_LED_GPIO_Port,SYS_LED_PIN) 
#define SYS_LED_FZ		GPIOC->ODR ^= (1<<4)

#define WANING_LED_PIN  GPIO_Pin_5
#define WANING_LED_GPIO_Port GPIOC
#define WANING_LED_OFF    GPIO_SetBits(WANING_LED_GPIO_Port,WANING_LED_PIN)	
#define WANING_LED_ON     GPIO_ResetBits(WANING_LED_GPIO_Port,WANING_LED_PIN)
#define WANING_LED_FZ		GPIOC->ODR ^= (1<<5)


//按键
#define KEY1_PIN  GPIO_Pin_0
#define KEY1_GPIO_Port GPIOC
#define KEY1     GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)
#define KEY2_PIN  GPIO_Pin_1
#define KEY2_GPIO_Port GPIOC
#define KEY2     GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)
#define KEY3_PIN  GPIO_Pin_2
#define KEY3_GPIO_Port GPIOC
#define KEY3     GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2) 
#define KEY4_PIN  GPIO_Pin_3
#define KEY4_GPIO_Port GPIOC
#define KEY4     GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3)		 

#define KEY1_PRES	1		//KEY1   下翻
#define KEY2_PRES	2		//KEY2   上翻
#define EKEY1_PRES	4		//EC11   旋钮快速下翻
#define EKEY2_PRES	3		//EC11   旋钮快速上翻
#define KEY3_PRES	5		//KEY3   确认键
#define KEY4_PRES	6		//KEY4   菜单键短按
#define KEY4_Long_PRES	7	//KEY4   菜单键长按
#define KEY1_Long_PRES	8	//KEY1   菜单键长按
#define KEY2_Long_PRES	9	//KEY2   菜单键长按
#define EKEY3_PRES	11		//EC11   旋钮上翻
#define EKEY4_PRES	12		//EC11   旋钮上翻


extern u8 KEY_Type; 
void gpio_Init(void);//初始化
u8 KEY_Scan(u8 mode);

#endif
