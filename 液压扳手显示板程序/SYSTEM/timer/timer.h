#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
extern u16 KEY1_ucTimer;
extern u16 KEY2_ucTimer;
extern u16 KEY4_ucTimer;
void TIM3_Int_Init(u16 arr,u16 psc); 
#endif