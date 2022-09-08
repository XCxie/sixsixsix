#include "Data_process.h"

u8 Spanner_type_Flag = 0;
u8 Targettorque_parameter_Flag = 0;


u16 Computing_Angle(u16 Initial_data,u16 Actual_value)
{
    u16 value = 0;
    if(Actual_value >= Initial_data)value = Actual_value - Initial_data;
    else value = (Actual_value + 360) - Initial_data;
}


/*! 
*  \brief     ��ð����ͺ�ϵ��
*  \param  Spanner_type  ������� 
*  \param  value    ����ֵ  ����ϵ��
*/
u32 Spanner_type_parameter(u8 Spanner_type)
{
    u32 value;
    value = SpannerType[Spanner_type].parameter;  //�ӽṹ���ð���ϵ��
    return value;
}

/*! 
*  \brief     ��ð����ͺŻ���ʱ��
*  \param  Spanner_type  �������
*  \param  value    ����ֵ  ����ϵ��
*/
u8 Spanner_type_OilReturnTime(u8 Spanner_type)
{
    u8 value;
    value = SpannerType[Spanner_type].Delay_time;  //�ӽṹ���ð��ֻ���ʱ��
    return value;
}


/*! 
*  \brief     ��˨�ȼ���
*  \param  Chart_Standard_Type  ͼ���׼
*  \param  Spanner_type    �����ͺ�
*  \param  value    ��˨�ȼ����
*/
u8 Bolt_GradeNumber(u8 Chart_Standard_Type,u8 Spanner_type)
{
    u8 value;
    switch(Chart_Standard_Type)
    {
        case Imperial :              
            value = SpannerType[Spanner_type].Grade_Imperial;  //�ӽṹ������˨�ȼ�����
        break;
        case Metric :        
            value = SpannerType[Spanner_type].Grade_Metric;    //�ӽṹ������˨�ȼ�����
        break;
    }
    return value;
}
/*! 
*  \brief     ��˨ֱ����
*  \param  Chart_Standard_Type  ͼ���׼
*  \param  Spanner_type    �����ͺ�
*  \param  value    ��˨ֱ�����
*/
u8 Bolt_DiameterNumber(u8 Chart_Standard_Type,u8 Spanner_type)
{
    u8 value;
    switch(Chart_Standard_Type)
    {
        case Imperial :              
            value = SpannerType[Spanner_type].Diameter_Imperial;  //�ӽṹ������˨ֱ������
        break;
        case Metric :        
            value = SpannerType[Spanner_type].Diameter_Metric;    //�ӽṹ������˨ֱ������
        break;
    }
    return value;
}


/*! 
*  \brief     ��ð���Ԥ��Ť��ֵ
*  \param  Spanner_type  �������    0-36
*  \param  value    ����ֵ  ����Ԥ��Ť��ֵ
*/
u32 Spanner_Sensor_parameter(u8 Spanner_type)
{
    u32 value;
    value = SpannerType[Spanner_type].Sensor_parameter;  //�ӽṹ���ð���Ԥ��Ť��ֵ
    return value;
}

/*! 
*  \brief     �����˨Ť��
*  \param  Chart_Standard_Type    ͼ���׼����
*  \param  Grade    ��˨�ȼ������꣩
*  \param  Diameter    ��˨ֱ�������꣩
*  \param  Bolttorque_value    ����ֵ  ��˨Ť��
*/
u32 Select_Bolttorque_parameter(u8 Chart_Standard_Type,u8 Grade,u8 Diameter)
{
    u32 Bolttorque_value;
    u8 i,j;
    switch(Chart_Standard_Type)
    {
        case Imperial :              
                Bolttorque_value = BoltType.Imperial_parameter[Grade][Diameter];
        break;
        case Metric :        
                Bolttorque_value = BoltType.Metric_parameter[Grade][Diameter];
        break;
    }
    return Bolttorque_value;
}

//��ȡĿ��Ť��ֵ
u32 Get_TargetTorque(uint8 Unit,float value)
{   
    u32 Target_value;
    switch(Unit)
    {
        case  0:   //N.m
            Target_value = ROUND_TO_UINT32(value);
        break;
        case  1:   //ft.lbs
            Target_value = ROUND_TO_UINT32(value/1.3549);
        break;     
    }
    return Target_value;
}
//��ȡʵ��Ť��ֵ
u32 Get_ActualTorque(uint8 Unit,float Pesssure_Val,float Coefficien)
{   
    u32 Actual_value;
    switch(Unit)
    {
        case  0:   //N.m
            Actual_value = ROUND_TO_UINT32(Pesssure_Val * Coefficien/10000.0);
        break;
        case  1:   //ft.lbs
            Actual_value = ROUND_TO_UINT32(Pesssure_Val * Coefficien/10000.0/1.3549);
        break;     
    }
    return Actual_value;
}

//��ȡ��λŤ��ֵ
u32 Get_GearTorque(uint8 Unit,float value)
{
    u32 Gear_value;
    switch(Unit)
    {
        case  0:   //N.m
            Gear_value = ROUND_TO_UINT32(value);
        break;
        case  1:   //ft.lbs
            Gear_value = ROUND_TO_UINT32(value/1.3549);
        break;     
    }
    return Gear_value;
}
//��ȡѹ��ֵ
float Get_Pressure(float value,u8 Unit)
{
    float Data = 0;
    switch(Unit)
    {
        case 0:   //bar
            Data = value*10.0;
        break;
        case 1:   //Mpa
            Data = value;
        break;
        case 2:   //psi
            Data = value*145.0;
        break;
    }
    return Data;
    
}

//��ȡĿ��ѹ��ֵ
u32 Get_TargetPressure(u32 Torque,float Coefficient,u8 Unit)
{
    float Data = 0;
    switch(Unit)
    {
        case 0:   //bar
            Data = ROUND_TO_UINT32(Torque/(Coefficient/10000.0)*10.0);
        break;
        case 1:   //Mpa
            Data = ROUND_TO_UINT32(Torque/(Coefficient/10000.0));
        break;
        case 2:   //psi
            Data = ROUND_TO_UINT32(Torque/(Coefficient/10000.0)*145.0);
        break;
    }
    return Data;
    
}


u8  Last_Spanner_type = 38;
/*! 
*  \brief     ��ð��ֶ�Ӧ��˨����
*  \param  type    �������
*/
void Get_Bolttorque_parameter(u8 Spanner_type)
{
    if(Spanner_type != Last_Spanner_type)
    {
        MX25L6406_Read((u8*)&BoltType,11*4096+Spanner_type*sizeof(Bolt),sizeof(Bolt));  //��ȡ��Ӧ���ֵ���˨����
        Last_Spanner_type = Spanner_type;
    }
}
