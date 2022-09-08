#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "stm32f10x_exti.h"

void TIM3_Int_Init(u16 arr,u16 psc);
void	Tim2GpioInit(void);
void	Tim2ModeInit(void);
void	Tim2NvicInit(void);
void Tim2Init(void);
float Get_angle_calculattion(void);
extern uint16 usCaptureRisingVal;//输入捕获脉冲个数
extern u16 Timer_count ;

void ExtiInit(void);
#endif

