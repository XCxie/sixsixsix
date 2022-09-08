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
*  \brief     获得扳手型号系数
*  \param  Spanner_type  扳手序号 
*  \param  value    返回值  扳手系数
*/
u32 Spanner_type_parameter(u8 Spanner_type)
{
    u32 value;
    value = SpannerType[Spanner_type].parameter;  //从结构体获得扳手系数
    return value;
}

/*! 
*  \brief     获得扳手型号回油时间
*  \param  Spanner_type  扳手序号
*  \param  value    返回值  扳手系数
*/
u8 Spanner_type_OilReturnTime(u8 Spanner_type)
{
    u8 value;
    value = SpannerType[Spanner_type].Delay_time;  //从结构体获得扳手回油时间
    return value;
}


/*! 
*  \brief     螺栓等级数
*  \param  Chart_Standard_Type  图表标准
*  \param  Spanner_type    扳手型号
*  \param  value    螺栓等级序号
*/
u8 Bolt_GradeNumber(u8 Chart_Standard_Type,u8 Spanner_type)
{
    u8 value;
    switch(Chart_Standard_Type)
    {
        case Imperial :              
            value = SpannerType[Spanner_type].Grade_Imperial;  //从结构体获得螺栓等级数量
        break;
        case Metric :        
            value = SpannerType[Spanner_type].Grade_Metric;    //从结构体获得螺栓等级数量
        break;
    }
    return value;
}
/*! 
*  \brief     螺栓直径数
*  \param  Chart_Standard_Type  图表标准
*  \param  Spanner_type    扳手型号
*  \param  value    螺栓直径序号
*/
u8 Bolt_DiameterNumber(u8 Chart_Standard_Type,u8 Spanner_type)
{
    u8 value;
    switch(Chart_Standard_Type)
    {
        case Imperial :              
            value = SpannerType[Spanner_type].Diameter_Imperial;  //从结构体获得螺栓直径数量
        break;
        case Metric :        
            value = SpannerType[Spanner_type].Diameter_Metric;    //从结构体获得螺栓直径数量
        break;
    }
    return value;
}


/*! 
*  \brief     获得扳手预设扭矩值
*  \param  Spanner_type  扳手序号    0-36
*  \param  value    返回值  扳手预设扭矩值
*/
u32 Spanner_Sensor_parameter(u8 Spanner_type)
{
    u32 value;
    value = SpannerType[Spanner_type].Sensor_parameter;  //从结构体获得扳手预设扭矩值
    return value;
}

/*! 
*  \brief     获得螺栓扭矩
*  \param  Chart_Standard_Type    图表标准类型
*  \param  Grade    螺栓等级（坐标）
*  \param  Diameter    螺栓直径（坐标）
*  \param  Bolttorque_value    返回值  螺栓扭矩
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

//获取目标扭矩值
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
//获取实际扭矩值
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

//获取档位扭矩值
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
//获取压力值
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

//获取目标压力值
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
*  \brief     获得扳手对应螺栓数据
*  \param  type    扳手序号
*/
void Get_Bolttorque_parameter(u8 Spanner_type)
{
    if(Spanner_type != Last_Spanner_type)
    {
        MX25L6406_Read((u8*)&BoltType,11*4096+Spanner_type*sizeof(Bolt),sizeof(Bolt));  //读取对应扳手的螺栓数据
        Last_Spanner_type = Spanner_type;
    }
}
