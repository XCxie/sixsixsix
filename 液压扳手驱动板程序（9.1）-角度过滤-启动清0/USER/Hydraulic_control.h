#ifndef	_Hydraulic_control_H_
#define	_Hydraulic_control_H_
#include "includes.h"
#include "TASK.h"
extern u8 StartUp_State;
extern float ADC_PressureValue;
extern float ADC_AngleValue;
extern int Usage_Counter;
extern u8 Motor_status;    
extern u8 SolenoidValve_status;
extern u8 Pressure_status;
extern float Spanner_value;
extern u32 Targettorque_value;
extern u8 DriveControl_status;
extern u8 Operation_Status;
extern float Send_PressureValue;
extern float Send_AngleValue;  //¼ÇÂ¼·¢ËÍ½Ç¶È
void TorqueMode_Control(u8 KeyState,u32 SpannerValue,u32 TorqueValue,u8 OilReturnTime);
void BoltMode_Control(u8 KeyState,u32 SpannerValue,u32 TorqueValue,u8 OilReturnTime);
void GearMode_Control(u8 KeyState,u32 SpannerValue,u32 TorqueValue,u8 OilReturnTime);
void AngleMode_Control(u8 KeyState,u32 SpannerValue,u32 TorqueValue,float Current_Angle,u16 Preset_Angle,u8 OilReturnTime);
void SensorMode_Control(u8 KeyState,u32 SpannerValue,u32 TorqueValue,u8 OilReturnTime);
void DriverContrlOff(void);
void Key_Control(SpannerStruct *pata);
#endif
