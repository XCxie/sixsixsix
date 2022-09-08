#ifndef _DATAPROCESS_H
#define _DATAPROCESS_H
#include "sys.h"
#include "MX25L6406.h"
#define   Spanner_MUN   37            //���37�����Ͱ���
//#define   Bolt_MUN   37            //���255��������˨
#define   Gear_MUN   255            //���255�����͵�λ
#define PTR2U16(PTR) ((((uint8 *)(PTR))[0]<<8)|((uint8 *)(PTR))[1])  //�ӻ�����ȡ16λ����
#define PTR2U32(PTR) ((((uint8 *)(PTR))[0]<<24)|(((uint8 *)(PTR))[1]<<16)|(((uint8 *)(PTR))[2]<<8)|((uint8 *)(PTR))[3])  //�ӻ�����ȡ32λ����
typedef struct
{
    u8 Metric_Grade[10][10];//���Ƶȼ�
    u8 Metric_Diameter[20][10];//����ֱ��
    u8 Imperial_Grade[10][10];//Ӣ�Ƶȼ�
    u8 Imperial_Diameter[20][10];//Ӣ��ֱ��
    u32 Metric_parameter[10][20];//����Ť�ز���
    u32 Imperial_parameter[10][20];//Ӣ��Ť�ز���
}Bolt;

typedef struct 
{
    char type[10]; //�����ͺ�
    u32  parameter;//����ϵ��
    u32  Sensor_parameter;//Ԥ�赵λֵ
    u8  Delay_time;  //��ʱʱ��    
    u8  Grade_Metric;//���Ƶȼ�����
    u8  Diameter_Metric;//����ֱ������
    u8  Grade_Imperial;//Ӣ�Ƶȼ�����
    u8  Diameter_Imperial;//Ӣ��ֱ������
}Spanner;

typedef struct
{
   u8 Levels;//�ȼ�
   u32 parameter;//����
}Simple1;


extern Spanner SpannerType[Spanner_MUN];//��������
extern  Simple1 Gears_data[Gear_MUN];//��λ����
extern  Bolt BoltType;//��˨����


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