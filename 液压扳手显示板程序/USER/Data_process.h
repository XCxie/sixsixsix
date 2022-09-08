#ifndef _DATAPROCESS_H
#define _DATAPROCESS_H
#include "sys.h"
#include "MX25L6406.h"
#define   Spanner_MUN   37            //最多37种类型扳手
//#define   Bolt_MUN   37            //最多255种类型螺栓
#define   Gear_MUN   255            //最多255种类型档位
#define PTR2U16(PTR) ((((uint8 *)(PTR))[0]<<8)|((uint8 *)(PTR))[1])  //从缓冲区取16位数据
#define PTR2U32(PTR) ((((uint8 *)(PTR))[0]<<24)|(((uint8 *)(PTR))[1]<<16)|(((uint8 *)(PTR))[2]<<8)|((uint8 *)(PTR))[3])  //从缓冲区取32位数据
typedef struct
{
    u8 Metric_Grade[10][10];//公制等级
    u8 Metric_Diameter[20][10];//公制直径
    u8 Imperial_Grade[10][10];//英制等级
    u8 Imperial_Diameter[20][10];//英制直径
    u32 Metric_parameter[10][20];//公制扭矩参数
    u32 Imperial_parameter[10][20];//英制扭矩参数
}Bolt;

typedef struct 
{
    char type[10]; //扳手型号
    u32  parameter;//扳手系数
    u32  Sensor_parameter;//预设档位值
    u8  Delay_time;  //延时时间    
    u8  Grade_Metric;//公制等级数量
    u8  Diameter_Metric;//公制直径数量
    u8  Grade_Imperial;//英制等级数量
    u8  Diameter_Imperial;//英制直径数量
}Spanner;

typedef struct
{
   u8 Levels;//等级
   u32 parameter;//数据
}Simple1;


extern Spanner SpannerType[Spanner_MUN];//扳手类型
extern  Simple1 Gears_data[Gear_MUN];//档位类型
extern  Bolt BoltType;//螺栓参数


extern u8 Spanner_type_Flag;
extern u8 Targettorque_parameter_Flag;
u16 Computing_Angle(u16 Initial_data,u16 Actual_value);
u32 Spanner_type_parameter(u8 Spanner_type);
u32 Spanner_Sensor_parameter(u8 Spanner_type);
u8 Bolt_GradeNumber(u8 Chart_Standard_Type,u8 Spanner_type);
u8 Bolt_DiameterNumber(u8 Chart_Standard_Type,u8 Spanner_type);
void Get_Bolttorque_parameter(u8 Spanner_type);
u32 Select_Bolttorque_parameter(u8 Chart_Standard_Type,u8 Grade,u8 Diameter);
u32 Get_TargetTorque(uint8 Unit,float value);
u32 Get_GearTorque(uint8 Unit,float value);
u32 Get_ActualTorque(uint8 Unit,float Pesssure_Val,float Coefficien);
float Get_Pressure(float value,u8 Unit);
u32 Get_TargetPressure(u32 Torque,float Coefficient,u8 Unit);
u8 Spanner_type_OilReturnTime(u8 Spanner_type);
#endif