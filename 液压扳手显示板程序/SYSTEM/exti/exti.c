#include "exti.h"
#include "TASK.h"
#include "gpio.h"
#include "usart.h"
#include "delay.h"
u16 tempvalue = 0; 
//外部中断初始化函数
void EXTIX_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//外部中断，需要使能AFIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//使能PORTC时钟

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭jtag，使能SWD，可以用SWD模式调试
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9; //KEY1A   KEY1B  KEY1C
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置上拉输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    //KEY1A
    //GPIOC.7 中断线以及中断初始化配置
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource7);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line7;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//上升沿触发
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
    //keyb
    //GPIOC.8	  中断线以及中断初始化配置
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource8);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line8;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
    //keyC
    //GPIOC.9	  中断线以及中断初始化配置
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource9);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line9;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
 
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能按键所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//子优先级1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
		
 

 
}
u8 EC11_status = 0;
 void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line7) != RESET)
    {
        KEY_Type = 1;
        if(KEY_C == 1)
        {
            if(KEY_B == 1)
            {
                EC11_status = 11;
            }
            else
            {
                EC11_status = 12;
            }
        }
        else
        {   
            if(KEY_B == 1)
            {
                EC11_status = 3;
            }
            else
            {
                EC11_status = 4;  
            }
        }

    }
  
    EXTI_ClearITPendingBit(EXTI_Line7);    //清除LINE7上的中断标志位  
    EXTI_ClearITPendingBit(EXTI_Line8);    //清除LINE7上的中断标志位  
    EXTI_ClearITPendingBit(EXTI_Line9);    //清除LINE7上的中断标志位      
}

