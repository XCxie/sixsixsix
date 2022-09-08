#ifndef _USER_DRIVE_
#define _USER_DRIVE_
#include "includes.h"

void Driver_Send32DataValue(uint8 function,uint32 value);
void Driver_Send8DataValue(uint8 function,u8 value);
void Send_DisplayData(u8 Status,u32 Counter,float Pressure);
void Send_StartUp_State(u8 State);
void Send_Contrl_Status(u8 DriveControl_status);
void Send_Pressure_Value(float value);
void Send_Angle_Value(float value);
#endif  
