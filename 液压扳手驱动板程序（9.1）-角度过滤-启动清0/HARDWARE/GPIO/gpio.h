#ifndef __GPIO_H
#define __GPIO_H	 
#include "includes.h"
//AD8688标定端口
#define AD8688_IO_1_PIN  GPIO_Pin_0
#define AD8688_IO_1_GPIO_Port GPIOC
#define AD8688_IO_1_H    GPIO_SetBits(AD8688_IO_1_GPIO_Port,AD8688_IO_1_PIN)
#define AD8688_IO_1_L     GPIO_ResetBits(AD8688_IO_1_GPIO_Port,AD8688_IO_1_PIN) 
#define AD8688_IO_1_FZ		GPIOC->ODR ^= (1<<0)

#define AD8688_IO_0_PIN  GPIO_Pin_1
#define AD8688_IO_0_GPIO_Port GPIOC
#define AD8688_IO_0_H    GPIO_SetBits(AD8688_IO_0_GPIO_Port,AD8688_IO_0_PIN)
#define AD8688_IO_0_L     GPIO_ResetBits(AD8688_IO_0_GPIO_Port,AD8688_IO_0_PIN) 
#define AD8688_IO_0_FZ		GPIOC->ODR ^= (1<<1)


//LED
#define SYS_LED_PIN  GPIO_Pin_7
#define SYS_LED_GPIO_Port GPIOB
#define SYS_LED_OFF    GPIO_SetBits(GPIOB,GPIO_Pin_7)
#define SYS_LED_ON     GPIO_ResetBits(GPIOB,GPIO_Pin_7) 
#define SYS_LED_FZ		GPIOB->ODR ^= (1<<7)

#define WANING_LED_PIN  GPIO_Pin_8
#define WANING_LED_GPIO_Port GPIOB
#define WANING_LED_OFF    GPIO_SetBits(GPIOB,GPIO_Pin_8)	
#define WANING_LED_ON     GPIO_ResetBits(GPIOB,GPIO_Pin_8)
#define WANING_LED_FZ		GPIOB->ODR ^= (1<<8)

#define STAR_LED_PIN  GPIO_Pin_9
#define STAR_LED_GPIO_Port GPIOB
#define STAR_LED_OFF    GPIO_SetBits(GPIOB,GPIO_Pin_9)	
#define STAR_LED_ON     GPIO_ResetBits(GPIOB,GPIO_Pin_9)
#define STAR_LED_FZ		GPIOB->ODR ^= (1<<9)
 //手柄按键
#define EKEY1_PIN  GPIO_Pin_8               //手动
#define EKEY1_GPIO_Port GPIOA
#define EKEY1     GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)
#define EKEY3_PIN  GPIO_Pin_8               //停止
#define EKEY3_GPIO_Port GPIOC
#define EKEY3     GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)
#define EKEY2_PIN  GPIO_Pin_9               //自动    
#define EKEY2_GPIO_Port GPIOC
#define EKEY2     GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9) 
	 
//Relay
#define KMCTRL1_PIN  GPIO_Pin_12
#define KMCTRL1_GPIO_Port GPIOB
#define KMCTRL1_OFF    GPIO_SetBits(GPIOB,GPIO_Pin_12)	
#define KMCTRL1_ON     GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define KMCTRL1_FZ		GPIOB->ODR ^= (1<<12)

#define KMCTRL2_PIN  GPIO_Pin_13
#define KMCTRL2_GPIO_Port GPIOB
#define KMCTRL2_OFF    GPIO_SetBits(GPIOB,GPIO_Pin_13)	
#define KMCTRL2_ON     GPIO_ResetBits(GPIOB,GPIO_Pin_13)
#define KMCTRL2_FZ		GPIOB->ODR ^= (1<<13)

#define KMCTRL3_PIN  GPIO_Pin_14
#define KMCTRL3_GPIO_Port GPIOB
#define KMCTRL3_OFF    GPIO_SetBits(GPIOB,GPIO_Pin_14)	
#define KMCTRL3_ON     GPIO_ResetBits(GPIOB,GPIO_Pin_14)
#define KMCTRL3_FZ		GPIOB->ODR ^= (1<<14)

#define KMCTRL4_PIN  GPIO_Pin_15
#define KMCTRL4_GPIO_Port GPIOB
#define KMCTRL4_OFF    GPIO_SetBits(GPIOB,GPIO_Pin_15)	
#define KMCTRL4_ON     GPIO_ResetBits(GPIOB,GPIO_Pin_15)
#define KMCTRL4_FZ		GPIOB->ODR ^= (1<<15)

#define KMCTRL5_PIN  GPIO_Pin_6
#define KMCTRL5_GPIO_Port GPIOC
#define KMCTRL5_OFF    GPIO_SetBits(GPIOC,GPIO_Pin_6)	
#define KMCTRL5_ON     GPIO_ResetBits(GPIOC,GPIO_Pin_6)
#define KMCTRL5_FZ		GPIOB->ODR ^= (1<<6)

#define KMCTRL6_PIN  GPIO_Pin_7
#define KMCTRL6_GPIO_Port GPIOC
#define KMCTRL6_OFF    GPIO_SetBits(GPIOC,GPIO_Pin_7)	
#define KMCTRL6_ON     GPIO_ResetBits(GPIOC,GPIO_Pin_7)
#define KMCTRL6_FZ		GPIOB->ODR ^= (1<<7)


#define KEY_NOPRES  0	
#define KEY1_PRES	1		//KEY1  
#define KEY2_PRES	2		//KEY2 
#define KEY3_PRES	3		//KEY3 
void gpio_Init(void);//初始化
u8 KEY_Scan(u8 mode);


#endif
