#ifndef	_TASK_H_
#define	_TASK_H_
#include "includes.h"
extern char Set_Torque_Unit;//设置扭矩单位

extern char Set_Language_Type;       //设置语言类型   
extern char Set_Chart_Standard_Type ; //设置图表标准类型
extern char Set_ProductStandard_Type;//设置产品规格类型
extern char Set_Pressure_Unit_Type ;  //设置压力单位类型 
extern char Set_Bolt_Diameter ;       //设置螺栓直径
extern char Set_Bolt_Garder ;         //设置螺栓等级


extern int Spanner_Coefficien;  //扳手保存系数
extern int TargetTorque_value;     //目标扭矩值
extern int BoltCoefficient_value;//螺栓扭矩
extern int GearTorque_value;//档位扭矩
extern short int Set_Target_Angle;       //设置预设角度值
extern char Set_Target_Gear;
extern int Set_SensorTorque;     //设置传感器预设扭矩值
extern u32 Set_Target_Usage;     //设置限制使用的次数
extern char  Set_Control_Mode;       //设置控制模式
extern u32 UsageCounter;  //使用次数
extern float Pressure_Value ; //压力值
extern u16 Angle_Value;  //实时角度值
extern u16 Max_Angle; //最大限制角度

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


