#ifndef	_TASK_H_
#define	_TASK_H_
#include "includes.h"

typedef struct
{
	
	uint8_t  AdcTaskDog;
	uint8_t  Usart4TaskDog;
	
	uint8_t  FlashTaskDog;
	uint8_t  ControlTaskDog; 
	uint8_t  UsartsendTaskDog; 
}FreeDogStruct;

typedef struct
{
    int p_SpannerCoefficient;  //扳手系数
    int p_UserTargetTorque;    //用户设定的扭矩
    int p_BoltTorque;          //螺栓扭矩
    int p_GearTorque;          //档位扭矩
    u16 p_TargetAngle;         //预设角度
    int p_TargetSensor;        //预设扭矩（传感器模式）
    int p_TargetUsageNumber;   //目标使用次数
     u8 p_Mode;                //当前模式
     u8 p_OilReturnTime;        //回油时间        
}SpannerStruct;

extern SpannerStruct SpannerDate;
extern FreeDogStruct FreeDogStructVar;
extern float ADC_PressureValue;  //实时压力
extern float ADC_AngleValue;     //实时角度值
void SYS_task(void *pdata);
void Time_task(void *pdata);
void HIM_task(void *pdata);
void HMIRev_task(void *pdata);














#endif


