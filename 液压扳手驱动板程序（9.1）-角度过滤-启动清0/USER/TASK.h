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
    int p_SpannerCoefficient;  //����ϵ��
    int p_UserTargetTorque;    //�û��趨��Ť��
    int p_BoltTorque;          //��˨Ť��
    int p_GearTorque;          //��λŤ��
    u16 p_TargetAngle;         //Ԥ��Ƕ�
    int p_TargetSensor;        //Ԥ��Ť�أ�������ģʽ��
    int p_TargetUsageNumber;   //Ŀ��ʹ�ô���
     u8 p_Mode;                //��ǰģʽ
     u8 p_OilReturnTime;        //����ʱ��        
}SpannerStruct;

extern SpannerStruct SpannerDate;
extern FreeDogStruct FreeDogStructVar;
extern float ADC_PressureValue;  //ʵʱѹ��
extern float ADC_AngleValue;     //ʵʱ�Ƕ�ֵ
void SYS_task(void *pdata);
void Time_task(void *pdata);
void HIM_task(void *pdata);
void HMIRev_task(void *pdata);














#endif


