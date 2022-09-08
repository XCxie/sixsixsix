#ifndef _DATAPROCESS_H
#define _DATAPROCESS_H
#include "includes.h"

float Get_PressureValue();
float Get_AngleValue();
u8 Angle_Overloop_Detection(float CurrentAngel,u8 Control_State);

#endif
