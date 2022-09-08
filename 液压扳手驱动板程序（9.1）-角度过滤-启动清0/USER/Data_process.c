#include "Data_process.h"
#include "math.h"
#include "usart.h"	
#include "MX25L6406.h"
#include "adc.h"
#include "ads8688.h"
#define PRESS_ADC_VAL_STA    1996    //压力ADC采集起始值
#define PRESS_ADC_VAL_RANGE   7984  //范围
#define ANGLE_ADC_VAL_STA    1975    //角度ADC采集起始值
#define ANGLE__ADC_VAL_RANGE  7905
#define TORQUE_ADC_VAL_STA   1996    //扭矩ADC采集起始值 
#define TORQUE_ADC_VAL_RANGE  7984


//采集压力传感器值
float Get_PressureValue()
{
    static u32 Press_info_time = 0;
    u32 Pressure_temp = 0;
    float Pressure_data = 0;
    Pressure_temp = ADS8688_Filter(0,22);   
    Pressure_data = ((float)Pressure_temp - PRESS_ADC_VAL_STA)/7984.0 * 100.0 ;
    if(Pressure_data <= 0)
    {
        Pressure_data = 0;
    }else if(Pressure_data >= 70)
    {
        Pressure_data = 70;
    }
#if 0
    Press_info_time++;
    if(Press_info_time >= 10)
    {
        Press_info_time = 0;
        printf("Pressure_temp:%d\r\n",Pressure_temp);
        printf("Pressure_data:%f\r\n",Pressure_data);
        
    }
#endif
    return Pressure_data;
}


//采集角度传感器值
float Get_AngleValue()
{
    u32 Angel_temp = 0;
    float Angel_data = 0;
    static u32 Press_info_time = 0;
    Angel_temp = ADS8688_Filter(1,10);
    Angel_data = (Angel_temp - ANGLE_ADC_VAL_STA)/7820.0* 3600.0;
    if(Angel_data <= 0)
    {
        Angel_data = 0;
    }else if(Angel_data >= 3600)
    {
        Angel_data = 3600;
    }
#if 0
    Press_info_time++;
    if(Press_info_time >= 40)
    {
        Press_info_time = 0;
        printf("Angel_temp:%d\r\n",Angel_temp);
        printf("Angel_data:%f\r\n",Angel_data);
    }
#endif
    return Angel_data;
}




u8 Angle_Overloop_Detection(float CurrentAngel,u8 Control_State)
{
    static u8 Circle_Flag = 0;
    static u8 Turn_Number = 0;
    if(Control_State == 0)  //模式关
    {
        Turn_Number = 0;
        Circle_Flag = 0;
    }
    else
    {     
        if(CurrentAngel >= 3000.0)  //过圈角度
        {
            Circle_Flag = 1;
        }
        else if(CurrentAngel <= 100.0 && Circle_Flag == 1)
        {   
            Turn_Number += 1;
            Circle_Flag = 0;
        }
    }
    return Turn_Number;
}
