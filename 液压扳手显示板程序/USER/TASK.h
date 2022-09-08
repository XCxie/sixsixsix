#ifndef	_TASK_H_
#define	_TASK_H_
#include "includes.h"
extern char Set_Torque_Unit;//����Ť�ص�λ

extern char Set_Language_Type;       //������������   
extern char Set_Chart_Standard_Type ; //����ͼ���׼����
extern char Set_ProductStandard_Type;//���ò�Ʒ�������
extern char Set_Pressure_Unit_Type ;  //����ѹ����λ���� 
extern char Set_Bolt_Diameter ;       //������˨ֱ��
extern char Set_Bolt_Garder ;         //������˨�ȼ�


extern int Spanner_Coefficien;  //���ֱ���ϵ��
extern int TargetTorque_value;     //Ŀ��Ť��ֵ
extern int BoltCoefficient_value;//��˨Ť��
extern int GearTorque_value;//��λŤ��
extern short int Set_Target_Angle;       //����Ԥ��Ƕ�ֵ
extern char Set_Target_Gear;
extern int Set_SensorTorque;     //���ô�����Ԥ��Ť��ֵ
extern u32 Set_Target_Usage;     //��������ʹ�õĴ���
extern char  Set_Control_Mode;       //���ÿ���ģʽ
extern u32 UsageCounter;  //ʹ�ô���
extern float Pressure_Value ; //ѹ��ֵ
extern u16 Angle_Value;  //ʵʱ�Ƕ�ֵ
extern u16 Max_Angle; //������ƽǶ�

extern u8 AutoNumber;
extern u8 AutoDatabuff[4][120];
void SYS_task(void *pdata);
void Time_task(void *pdata);
void HIM_task(void *pdata);
void Udisk_task(void *pdata);




typedef struct{
	
	uint8_t  TimeTaskDog;
	uint8_t  HiMTaskDog;
	uint8_t  UsartTaskDog;
	uint8_t  FlashTaskDog;
	uint8_t  UdiskTaskDog; 
	
}FreeDogStruct;


extern FreeDogStruct FreeDogStructVar;







#endif


