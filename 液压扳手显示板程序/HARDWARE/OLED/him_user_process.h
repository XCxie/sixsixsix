#ifndef _USER_PROCESS_
#define _USER_PROCESS_
#include "sys.h"
enum  InterfaceNumber
{
    Menu_Interface = 0,
    Menu_Language = 5,
    Menu_Chart_Standard = 6,
    Menu_Date_Set =7,
    Date_Set_Year =8,
    Date_Set_Month =9,
    Date_Set_Day =10,
    Date_Set_Hour =11,
    Date_Set_Minute =12,
    Date_Set_Second =13,
    
    Torque_Interface = 1,
    Product_Model_Set = 14,
    Product_Standard = 15,
    Product_Model = 16,
    Torque_Max = 17,
    Target_Torque_Set = 18,
    Target_Torque_parameter = 19,
    Target_Torque_unit = 20,
    Pressure_Unit = 21,
    
    Bolt_Interface = 2,
    Bolt_Diameter = 22,
    Bolt_Grade = 23,
//    Target_Torque_Set = 18,
//    Target_Torque_parameter = 19,
//    Target_Torque_unit = 20,    
    Dropping_Torque = 24,
    
    Angle_Interface = 3,
//    Product_Model_Set = 14,
//    Product_Standard = 15,
//    Product_Model = 16,
    Angle_Target = 25,
//    Pressure_Unit = 21,
    
    Gear_Interface = 4,
//    Product_Model_Set = 14,
//    Product_Standard = 15,
//    Product_Model = 16,
    Target_Gear  =26,
//    Pressure_Unit = 21,
  
};//画面ID

enum  MenuInterfaceID
{
    Menu_Main_Interface = 0,
    S_Main_language = 6,
    S_Main_ChartStandard = 7,
    S_Main_year =8,
    S_Main_month =9,
    S_Main_day =10,
    S_Main_hour =11,
    S_Main_minute =12,
    S_Main_second =13,
    
    Menu_Torque_Interface = 1,
    S_Torque_ProductType = 14,
    S_Torque_MaxTorqueuint = 15,
    S_Torque_TargetTorque = 16,
    S_Torque_TargetTorqueuint = 17,
    S_Torque_ActualPressureuint = 18,
    
    Menu_Bolt_Interface = 2,
    S_Bolt_Diameter = 19,
    S_Bolt_Grade = 20,
    S_Bolt_TargetTorque = 21,
    S_Bolt_TargetTorqueuint = 22,
    S_Bolt_ActualPressureuint = 23,    
    
    Menu_Angle_Interface = 3,
    S_Angle_ProductType = 24,
    S_Angle_PresetTorque = 25,
    S_Angle_PresetTorqueuint = 26,
    S_Angle_TargetAngle = 27,
    S_Angle_ActualPressureuint = 28,
    
    Menu_Gear_Interface = 4,
    S_Gear_ProductType = 29,
    S_Gear_TargetGear = 30,
    S_Gear_TargetTorque = 31,
    S_Gear_TargetTorqueuint  =32,
    S_Gear_ActualPressureuint = 33,
    
    Menu_Sensor_Interface = 5, //传感器模式界面 
    S_Sensor_ProductType = 34,
    S_Sensor_PresetTorque = 35,
    S_Sensor_PresetTorqueuint = 36,
    S_Sensor_TargetTorque = 37,
    S_Sensor_TargetTorqueuint = 38,
    S_Sensor_ActualPressureuint = 39,
  
};//画面ID


enum   LanguageType
{
    English = 0,
    Chinese = 1,  
};//语言类型

enum   ChartStandard
{
    Imperial = 0,
    Metric = 1,  
};//图表标准类型

enum   Type_id
{
    Type_XLCT = 1,
    Type_MXT = 2,
    Type_WTS = 3,     
};//产品型号
extern uint16 current_screen_id;

/*! 
*  \brief     画面切换
*  \param  screen_id   当前画面iD
*/
void HIM_Home_Switcher(uint16 screen_id);
/*! 
*  \brief     设置指向光标
*  \param  screen_id   当前画面iD
*  \param  Cursor_id   光标编号
*/
void HIM_Cursor(uint16 screen_id,uint16 Cursor_id);
/*! 
*  \brief     文本变量闪烁
*  \param  screen_id   画面ID
*  \param  Cursor_id   选项ID
*/
void Variable_Blink(uint16 screen_id,uint16 Cursor_id);
/*! 
*  \brief     刷新显示时间
*  \param  无
*/
void DisplayTime(void);
/*! 
*  \brief     产品规格显示
*  \param  screen_id    画面id
*  \param  control_id   控件id
*  \param  type  产品型号
*/
void HIM_Standard_display(uint16 screen_id,uint16 control_id,uint8 type);
/*! 
*  \brief     语言显示
*  \param  type   输入显示的语言类型
*/
void HIM_Language_display(uint8 type);
/*! 
*  \brief     语言显示
*  \param  type   输入显示的图表标准类型
*  \param  Languege_type   输入语言类型
*/
void HIM_ChartStandard_display(uint8 type,uint8 Languege_type);
/*! 
*  \brief     最大扭矩单位显示
*  \param  type   输入显示的单位类型
*/
void HIM_MaxTorqueUnit_display(uint8 type);
/*! 
*  \brief     显示扳手最大扭矩
*  \param  Unit    单位
*  \param  value  扳手系数
*  \param  Max_value     返回值 扳手最大扭矩
*/  
float HIM_MaxTorque_display(uint8 Unit,float value);
/*! 
*  \brief     显示扳手目标扭矩
*  \param  screen_id    画面id
*  \param  control_id   控件id
*  \param  Unit    单位
*  \param  value  扳手系数
*  \param  Max_value     返回值 扳手目标扭矩
*/  
void HIM_TargetTorque_display(uint16 screen_id,uint16 control_id,u32 value);
/*! 
*  \brief     目标扭矩单位显示
*  \param  screen_id    画面id
*  \param  control_id   控件id
*  \param  type   输入显示的单位类型
*/
void HIM_ActualPressureuint_display(uint16 screen_id,uint16 control_id,uint8 type);
/*! 
*  \brief    螺栓型号标准显示
*  \param  Standard_Type   图表标准 Imperial:英制  Metric：公制
*  \param  Bolt_Diameter   螺栓直径
*/
void Bolt_Standard_display(uint8 Standard_Type,uint8 Bolt_Diameter);
/*! 
*  \brief     目标扭矩单位显示
*  \param  screen_id    画面id
*  \param  control_id   控件id
*  \param  type   输入显示的单位类型
*/
void HIM_TorqueUint_display(uint16 screen_id,uint16 control_id,uint8 type); 
/*! 
*  \brief     压力值显示
*  \param  screen_id   当前画面iD
*  \param  control_id   控件id
*  \param  value   压力值
*  \param  Unit   压力单位
*/
void HIM_Pressure_display(uint16 screen_id,uint16 control_id,float value,u8 Unit);
u32 Get_GearTorqueValue(float value,u32 Gearvalue);
void Bolt_Grade_display(uint8 Standard_Type,uint8 Bolt_Garde);
void HIM_GearTorqueValue(uint16 screen_id,uint16 control_id,u32 value);
#endif  