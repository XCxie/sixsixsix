#include "Hydraulic_control.h"
#include "gpio.h"
#include "usart.h"	
#include "Data_process.h"
#include "timer.h"
#include "hmi_user_uart_driver.h"

enum
{
    MENU_MODE = 0,
    TORQUE_MODE,
    BOLT_MODE,
    GEAR_MODE,
    ANGEL_MODE,
    SENSOR_MODE  
}ModeState_e;
#define RET_PRESSURE_MIN   6    //回油压力最小值
#define RET_PRESSURE_MAX   10   //回油压力最大值
#define DEVICE_DOWNTIME   1500  //设备停机时间
#define SETZERO_TIME     150
u8 SolenoidValve_status = 0;//电磁阀状态
u8 Pressure_status = 0;     //压力状态
u8 Motor_status = 0;        //电机状态
u8 StartUp_State = 0;       //启动状态记录


float ADC_PressureValue = 0;  //实时采集压力
float ADC_AngleValue = 0;     //实时采集角度值
int Usage_Counter = 0; //使用次数 
u8 DriveControl_status  = 0;
u16 Motor_Timcount = 0;
u8 Operation_Status = 0;
u8 AngleControl_State = 0;

float AngleMark = 0;
float Record_Angle = 0;  //记录角度
float TargetAngle = 0;//角度过度值 k_TargetAngle;//角度过渡值
u16 g_data = 0;//传入角度标志位，只获取一次

float Send_PressureValue = 0;  //记录发送压力
float Send_AngleValue = 0;  //记录发送角度
void SolenoidValveOFF(void)
{
    //两个220v一个24v电磁阀
    KMCTRL3_OFF; 
    KMCTRL4_OFF;
    KMCTRL5_OFF; 
}
void SolenoidValveON(void)
{
    //两个220v一个24v电磁阀
    KMCTRL3_ON;
    KMCTRL4_ON;
    KMCTRL5_ON; 
}

//驱动控制关闭
void DriverContrlOff (void)
{
    KMCTRL1_OFF;
    KMCTRL3_OFF;
    KMCTRL4_OFF;
    KMCTRL5_OFF;
    KMCTRL6_OFF;    //风扇
    WANING_LED_OFF;
    STAR_LED_OFF; 
}

/*!
*  \brief     扭矩模式控制
*  \param  TorqueValue   扭矩值
*  \param  SpannerValue  扳手系数
*/
void TorqueMode_Control(u8 KeyState,u32 SpannerValue,u32 TorqueValue,u8 OilReturnTime)
{   
    float Send_PressureTemp = 0;
    float Target_pressure = 0; //目标压力
    Target_pressure = (float)TorqueValue/SpannerValue*10000.0;
    if(Target_pressure >= 70)
    {
        Target_pressure = 70;
    }
//    printf("Target_pressure:%f\r\n",Target_pressure); 
    //停止键   
    if(KeyState == 3)    
    {           
        Motor_Timcount = 0;   //有按键操作，清空计数
        //判断电机状态
        if(Motor_status == 1) //启动了，将其关闭
        {
            KMCTRL1_OFF;
            KMCTRL6_OFF;            
            Motor_status = 0; //将电机状态位置为关闭状态
        }
        SolenoidValveOFF();   //关闭电磁阀            
    }
     //自动键
    else if(KeyState == 2 )   
    {
        Motor_Timcount = 0;        //有按键操作，清空计数
        
        SolenoidValve_status = 1;  //电磁阀自动状态启动
        DriveControl_status = 1;   //自动
        //检测电机是否启动
        if(Motor_status == 0) //没有启动，将其启动    
        {
            KMCTRL1_ON;    //电机
            
            KMCTRL6_ON;    //风扇        
            Motor_status = 1;              
        }         
        if(ADC_PressureValue >= Target_pressure && Pressure_status ==0)    //到达设定的压力值
        {
            Send_PressureTemp = ADC_PressureValue -(ADC_PressureValue*0.04);  //记录到达设定压力值
            printf("Send_PressureTemp:%f\r\n",Send_PressureTemp);
            if(Send_PressureTemp > Target_pressure*1.03 || Send_PressureTemp < Target_pressure*0.97)
            {
                
            }
            else
            {
                Send_PressureValue = Send_PressureTemp;
                printf("Send_PressureValue:%f\r\n",Send_PressureValue);
            }
            StartUp_State = 1;            //记录             
            Pressure_status = 1;       
            SolenoidValveOFF(); //关闭电磁阀
            WANING_LED_OFF;   
        }else if(ADC_PressureValue< RET_PRESSURE_MAX && ADC_PressureValue >= RET_PRESSURE_MIN &&Pressure_status ==1)//到达回油值
        {   
            OSTimeDly(OilReturnTime*200);//回油等待时间 
            StartUp_State = 0;           //使用状态启动             
            Pressure_status = 0;
            SolenoidValveON(); //打开电磁阀
            WANING_LED_ON;            
        }
        else if(ADC_PressureValue< RET_PRESSURE_MAX &&Pressure_status ==0)//未到达
        {
            StartUp_State = 0;           //使用状态启动 
            SolenoidValveON(); //打开电磁阀 
            WANING_LED_ON;                
        }
    }
    //手动键
    else if(KeyState == 1 )  
    {   
        Motor_Timcount = 0;        
        DriveControl_status = 0;   //手动
        //检测电机状态是否启动
        if(Motor_status == 0) //没有启动，将其启动    
        {
            KMCTRL1_ON;    //电机
            KMCTRL6_ON;    //风扇                   
            Motor_status = 1; //将电机状态位置为启动状态
        }  
        StartUp_State = 0;        //使用状态启动 
        SolenoidValve_status = 2;  //电磁阀打开
        if(Send_PressureValue < 20)
        {
           Send_PressureValue = ADC_PressureValue + 4.5;  //记录到达设定压力值 
        }else
        {            
            Send_PressureValue = ADC_PressureValue + 3;  //记录到达设定压力值
        }
//        printf("Send_PressureValue:%f\r\n",Send_PressureValue);        
        SolenoidValveON(); //打开电磁阀
        STAR_LED_ON;       //测试灯开启
                         
    } 
    else if(KeyState == 0)  //无按键操作自动停止电机
    {
        if(SolenoidValve_status == 2)
        {
            StartUp_State = 1;     //使用状态关闭 
            SolenoidValve_status = 0;
            SolenoidValveOFF(); //关闭电磁阀
            STAR_LED_OFF;             //测试灯关闭   
        }
        if(SolenoidValve_status == 1)
        {
            StartUp_State = 2;           //使用状态启动 
            Pressure_status = 0;
            SolenoidValve_status = 0;
            SolenoidValveOFF(); //关闭电磁阀
            WANING_LED_OFF;                              
        }      
        Motor_Timcount++;
        if(Motor_Timcount >=DEVICE_DOWNTIME)    //30秒按键无操作
        {
         //判断电机状态
           Motor_Timcount = 4000;
           if(Motor_status == 1)//启动了，将其关闭
           {
                DriverContrlOff();              
                Motor_status = 0;  //将电机状态位置为关闭状态
           }
       }
    }
}



/*!
*  \brief     螺栓模式控制
*  \param  TorqueValue   扭矩值
*  \param  SpannerValue  扳手系数
*/
void BoltMode_Control(u8 KeyState,u32 SpannerValue,u32 TorqueValue,u8 OilReturnTime)
{
    float Send_PressureTemp = 0;
    float Target_pressure = 0; //目标压力
    Target_pressure = (float)TorqueValue/SpannerValue*10000.0;//系数/扭矩 = 目标压力
//    printf("ADC_PressureValue:%f\r\n",ADC_PressureValue); 
    if(Target_pressure >= 70)
    {
        Target_pressure = 70;
    }
    //停止键   
    if(KeyState == 3)    
    {           
        Motor_Timcount = 0;   //有按键操作，清空计数
        //判断电机状态
        if(Motor_status == 1) //启动了，将其关闭
        {
            KMCTRL1_OFF;
            KMCTRL6_OFF;            
            Motor_status = 0; //将电机状态位置为关闭状态
        }
        SolenoidValveOFF();   //关闭电磁阀            
    }
     //自动键
    else if(KeyState == 2 )   
    {
        Motor_Timcount = 0;        //有按键操作，清空计数
        
        SolenoidValve_status = 1;  //电磁阀自动状态启动
        DriveControl_status = 1;   //自动
        //检测电机是否启动
        if(Motor_status == 0) //没有启动，将其启动    
        {
            KMCTRL1_ON;    //电机
            KMCTRL6_ON;    //风扇        
            Motor_status = 1;              
        }         
        if(Target_pressure < 20)
        {
            Target_pressure = Target_pressure - 2;
        }
        printf("ADC_P == %f\r\n",ADC_PressureValue);
        if(ADC_PressureValue >= Target_pressure && Pressure_status ==0  )    //到达设定的压力值
        {
            Send_PressureTemp = ADC_PressureValue -(ADC_PressureValue*0.04);  //记录到达设定压力值
            printf("Send_PressureTemp:%f\r\n",Send_PressureTemp);
            if(Send_PressureTemp > Target_pressure*1.03 || Send_PressureTemp < Target_pressure*0.97)
            {
                
            }
            else
            {
                Send_PressureValue = Send_PressureTemp;
                printf("Send_PressureValue:%f\r\n",Send_PressureValue);
            }
            StartUp_State = 1;            //记录             
            Pressure_status = 1;       
            SolenoidValveOFF(); //关闭电磁阀
            WANING_LED_OFF;   
        }else if(ADC_PressureValue< RET_PRESSURE_MAX && ADC_PressureValue >= RET_PRESSURE_MIN &&Pressure_status ==1)//到达回油值
        {           
            OSTimeDly(OilReturnTime*200);//回油等待时间 
            StartUp_State = 0;           //使用状态启动             
            Pressure_status = 0;
            SolenoidValveON(); //打开电磁阀
            WANING_LED_ON;   
        }
        else if(ADC_PressureValue< RET_PRESSURE_MAX &&Pressure_status ==0)//未到达
        {
            StartUp_State = 0;           //使用状态启动 
            SolenoidValveON(); //打开电磁阀 
            WANING_LED_ON;                  
        }
    }
    //手动键
    else if(KeyState == 1 )  
    {   
        Motor_Timcount = 0;        
        DriveControl_status = 0;   //手动
        //检测电机状态是否启动
        if(Motor_status == 0) //没有启动，将其启动    
        {
            KMCTRL1_ON;    //电机
            KMCTRL6_ON;    //风扇                   
            Motor_status = 1; //将电机状态位置为启动状态
        }  
        StartUp_State = 0;        //使用状态启动 
        SolenoidValve_status = 2;  //电磁阀打开
        if(Send_PressureValue < 20)
        {
           Send_PressureValue = ADC_PressureValue + 4.5;  //记录到达设定压力值 
        }else
        {            
            Send_PressureValue = ADC_PressureValue + 3;  //记录到达设定压力值
        }
//        printf("Send_PressureValue:%f\r\n",Send_PressureValue);        
        SolenoidValveON(); //打开电磁阀
        STAR_LED_ON;       //测试灯开启
                         
    } 
    else if(KeyState == 0)  //无按键操作自动停止电机
    {
        if(SolenoidValve_status == 2)
        {
            StartUp_State = 1;     //使用状态关闭 
            SolenoidValve_status = 0;
            SolenoidValveOFF(); //关闭电磁阀
            STAR_LED_OFF;             //测试灯关闭   
        }
        if(SolenoidValve_status == 1)
        {
            StartUp_State = 2;           //使用状态启动 
            Pressure_status = 0;
            SolenoidValve_status = 0;
            SolenoidValveOFF(); //关闭电磁阀
            WANING_LED_OFF;                              
        }      
        Motor_Timcount++;
        if(Motor_Timcount >=DEVICE_DOWNTIME)    //30秒按键无操作
        {
         //判断电机状态
           Motor_Timcount = 4000;
           if(Motor_status == 1)//启动了，将其关闭
           {
                DriverContrlOff();              
                Motor_status = 0;  //将电机状态位置为关闭状态
           }
       }
    }      
}

/*!
*  \brief     档位模式控制
*  \param  TorqueValue   扭矩值
*  \param  SpannerValue  扳手系数
*/
void GearMode_Control(u8 KeyState,u32 SpannerValue,u32 TorqueValue,u8 OilReturnTime)
{
    float Send_PressureTemp = 0;
    float Target_pressure = 0; //目标压力
    Target_pressure = (float)TorqueValue/SpannerValue*10000.0;
    if(Target_pressure >= 70)
    {
        Target_pressure = 70;
    }
   // printf("KeyState:%d\r\n",KeyState); 
    //停止键   
    if(KeyState == 3)    
    {           
        Motor_Timcount = 0;   //有按键操作，清空计数
        //判断电机状态
        if(Motor_status == 1) //启动了，将其关闭
        {
            KMCTRL1_OFF;
            KMCTRL6_OFF;            
            Motor_status = 0; //将电机状态位置为关闭状态
        }
        SolenoidValveOFF();   //关闭电磁阀            
    }
     //自动键
    else if(KeyState == 2 )   
    {
        Motor_Timcount = 0;        //有按键操作，清空计数
        
        SolenoidValve_status = 1;  //电磁阀自动状态启动
        DriveControl_status = 1;   //自动
        //检测电机是否启动
        if(Motor_status == 0) //没有启动，将其启动    
        {
            KMCTRL1_ON;    //电机
            KMCTRL6_ON;    //风扇        
            Motor_status = 1;              
        }         

        if(ADC_PressureValue >= Target_pressure && Pressure_status ==0 )    //到达设定的压力值
        {
            Send_PressureTemp = ADC_PressureValue -(ADC_PressureValue*0.04);  //记录到达设定压力值
            printf("Send_PressureTemp:%f\r\n",Send_PressureTemp);
            if(Send_PressureTemp > Target_pressure*1.03 || Send_PressureTemp < Target_pressure*0.97)
            {
                
            }
            else
            {
                Send_PressureValue = Send_PressureTemp;
                printf("Send_PressureValue:%f\r\n",Send_PressureValue);
            }
            StartUp_State = 1;            //记录             
            Pressure_status = 1;       
            SolenoidValveOFF(); //关闭电磁阀
            WANING_LED_OFF;   
        }else if(ADC_PressureValue< RET_PRESSURE_MAX && ADC_PressureValue >= RET_PRESSURE_MIN &&Pressure_status ==1)//到达回油值
        {           
            OSTimeDly(OilReturnTime*200);//回油等待时间 
            StartUp_State = 0;           //使用状态启动             
            Pressure_status = 0;
            SolenoidValveON(); //打开电磁阀
            WANING_LED_ON;   
        }
        else if(ADC_PressureValue< RET_PRESSURE_MAX &&Pressure_status ==0)//未到达
        {
            StartUp_State = 0;           //使用状态启动 
            SolenoidValveON(); //打开电磁阀 
            WANING_LED_ON;                  
        }
    }
    //手动键
    else if(KeyState == 1 )  
    {   
        Motor_Timcount = 0;        
        DriveControl_status = 0;   //手动
        //检测电机状态是否启动
        if(Motor_status == 0) //没有启动，将其启动    
        {
            KMCTRL1_ON;    //电机
            KMCTRL6_ON;    //风扇                   
            Motor_status = 1; //将电机状态位置为启动状态
        }  
        StartUp_State = 0;        //使用状态启动 
        SolenoidValve_status = 2;  //电磁阀打开
        if(Send_PressureValue < 20)
        {
           Send_PressureValue = ADC_PressureValue + 4.5;  //记录到达设定压力值 
        }else
        {            
            Send_PressureValue = ADC_PressureValue + 3;  //记录到达设定压力值
        }
       
        SolenoidValveON(); //打开电磁阀
     
        STAR_LED_ON;       //测试灯开启
                         
    } 
    else if(KeyState == 0)  //无按键操作自动停止电机
    {
        if(SolenoidValve_status == 2)
        {
            StartUp_State = 1;     //使用状态关闭 
            SolenoidValve_status = 0;
            SolenoidValveOFF(); //关闭电磁阀
            STAR_LED_OFF;             //测试灯关闭   
        }
        if(SolenoidValve_status == 1)
        {
            StartUp_State = 2;           //使用状态启动 
            Pressure_status = 0;
            SolenoidValve_status = 0;
            SolenoidValveOFF(); //关闭电磁阀
            WANING_LED_OFF;                              
        }      
        Motor_Timcount++;
        if(Motor_Timcount >=DEVICE_DOWNTIME)    //30秒按键无操作
        {
         //判断电机状态
           Motor_Timcount = 4000;
           if(Motor_status == 1)//启动了，将其关闭
           {
                DriverContrlOff();              
                Motor_status = 0;  //将电机状态位置为关闭状态
           }
       }
    }       
}



/*!
*  \brief     角度模式控制
*  \param  TorqueValue   扭矩值
*  \param  SpannerValue  扳手系数
*  \param  Preset_Angle  预设角度
*/

//float Record_Angle = 0;  //记录角度
//u16 g_data = 0;//传入角度标志位，只获取一次

void AngleMode_Control(u8 KeyState,u32 SpannerValue,u32 TorqueValue,float Current_Angle,u16 Preset_Angle,u8 OilReturnTime)
{
    float Send_PressureTemp = 0;
    static float Ago_Angle = 0; //启动时初始角度
    static u8 ControlStart_Flag = 0;
    static u8 autoAngle_flag = 0;
    float Target_pressure = 0; //目标压力
    static u32 Set_Zero_Time = 0;
    u8 Angle_Num = 0;
    u8 Set_Zero_Flag = 0;
    Target_pressure = (float)TorqueValue/SpannerValue*10000.0;
    
    if(Target_pressure >= 70)
    {
        Target_pressure = 70;
    } 
    //停止键   
    if(KeyState == 3)    
    {           
        Motor_Timcount = 0;   //有按键操作，清空计数
        //判断电机状态
        if(Motor_status == 1)//启动了，将其关闭
        {
            KMCTRL1_OFF;
            KMCTRL6_OFF;            
            Motor_status = 0;  //将电机状态位置为关闭状态
        }
        SolenoidValveOFF();
        g_data = 0;
        usCaptureRisingVal = 0;
        Record_Angle = 0;
        AngleMark = 0;//角度过度过度值清0        
    }
     //自动键
    else if(KeyState == 2 )   
    {
        Record_Angle = Get_angle_calculattion();    //获取角度
        Motor_Timcount = 0;        //有按键操作，清空计数
        StartUp_State = 0;
        SolenoidValve_status = 1;  //电磁阀自动状态启动
        DriveControl_status = 1;   //自动     

        //检测电机是否启动
        if(Motor_status == 0) //没有启动，将其启动    
        {
            KMCTRL1_ON;    //电机
            KMCTRL6_ON;    //风扇        
            Motor_status = 1;               
        } 
       if(g_data == 0){
            TargetAngle = Preset_Angle;
            printf("oneline\r\n");
            g_data = 1;
        }
        if(Record_Angle <= TargetAngle) //每次获取的角度
        {
            if(ADC_PressureValue >= Target_pressure && Pressure_status ==0)    //到达设定的压力值
            {
                Send_PressureTemp = ADC_PressureValue - (ADC_PressureValue*0.04);  //记录到达设定压力值
					 printf("1AngleMark:%f\r\n",AngleMark);	
					 printf("1Record_Angle:%f\r\n",Record_Angle);
					 printf("1Preset_Angle:%d\r\n",Preset_Angle);
						// 抽取中间值
					if(Record_Angle > 24) {
						Record_Angle = 24;
					}
					 AngleMark = AngleMark + Record_Angle;//实时角度
					 TargetAngle = Preset_Angle - AngleMark;//剩余的角度     当停止时将角度记录   
					
                printf("Send_PressureTemp:%f\r\n",Send_PressureTemp);
                if(Send_PressureTemp > Target_pressure*1.03 || Send_PressureTemp < Target_pressure*0.97)
                {
                       
                }
                else
                {
                    Send_PressureValue = Send_PressureTemp;
                   // printf("Send_PressureValue:%f\r\n",Send_PressureValue);
                } 
                
                Pressure_status = 1;       
                SolenoidValveOFF();
					 
					 Record_Angle = 0;//每次启动前清0
					 usCaptureRisingVal = 0;
					 
                WANING_LED_OFF; 
            }                
				else if(ADC_PressureValue< RET_PRESSURE_MAX && ADC_PressureValue >= RET_PRESSURE_MIN &&Pressure_status ==1)//到达回油值
            {                               
					  OSTimeDly(OilReturnTime*200);//回油等待时间 					 					
					  Pressure_status = 0;
					  Record_Angle = 0;//每次启动前清0
					  usCaptureRisingVal = 0;      
					  printf("2AngleMark:%f\r\n",AngleMark);	
					  printf("2Record_Angle:%f\r\n",Record_Angle);
				     printf("2Preset_Angle:%d\r\n",Preset_Angle);	
					  SolenoidValveON();
					  WANING_LED_ON;
         
            }
            else if(ADC_PressureValue< RET_PRESSURE_MAX &&Pressure_status == 0)//程序在这一直清0
            {
					
                SolenoidValveON();  
                WANING_LED_ON;                  
            }
			}
        else       
        {
            if(autoAngle_flag ==0)   //到达角度执行一次
            {
                Send_AngleValue = AngleMark + Record_Angle;  //记录角度 
					 printf("Send_AngleValue == %.2f\r\n",Send_AngleValue);               
                //到达目标角度                 
                SolenoidValveOFF();  //关闭
                
                WANING_LED_OFF;
                AngleControl_State = 1;
                StartUp_State = 0;
                autoAngle_flag = 1;
            }
        }
    }
    //手动键
    else if(KeyState == 1 )  
    {  
        Record_Angle = Get_angle_calculattion();    //获取角度
        Motor_Timcount = 0;        
        DriveControl_status = 0;   //手动
        //检测电机状态是否启动
        if(Motor_status == 0) //没有启动，将其启动    
        {
            KMCTRL1_ON;    //电机
            KMCTRL6_ON;    //风扇                   
            Motor_status = 1; //将电机状态位置为启动状态
        }
        StartUp_State = 0;        //使用状态启动 
        SolenoidValve_status = 2;  //电磁阀打开 
        if(Send_PressureValue < 20)
        {
           Send_PressureValue = ADC_PressureValue + 4.5;  //记录到达设定压力值 
        }else
        {            
            Send_PressureValue = ADC_PressureValue + 3;  //记录到达设定压力值
        }
        printf("Send_PressureValue:%f\r\n",Send_PressureValue);          
        SolenoidValveON();  
        STAR_LED_ON;       //测试灯开启        
                     
    } 
    //无按键操作自动停止电机
    else if(KeyState == 0) 
    {
        if(SolenoidValve_status == 2)
        {
            SolenoidValveOFF(); 
            
   
            
            STAR_LED_OFF;                        //测试灯关闭  
            Send_AngleValue = Record_Angle/1.19;
				Record_Angle = 0;
				usCaptureRisingVal = 0;
            StartUp_State = 1;     //使用状态关闭 

            SolenoidValve_status = 0;
            ControlStart_Flag = 0;  //清除启动记录角度标志位     
        }
        
        
        if(SolenoidValve_status == 1)
        { 
            SolenoidValveOFF(); 
            
            usCaptureRisingVal = 0;//捕获值清0
            Record_Angle = 0;//记录角度清0
            AngleMark = 0;//角度过度过度值清0
            
            WANING_LED_OFF; 
            g_data = 0;//计数值重新清0
			  
            StartUp_State = 2;           //使用状态启动
            autoAngle_flag = 0;
            Pressure_status = 0;
            SolenoidValve_status = 0;              
            AngleControl_State = 0;            
            ControlStart_Flag = 0;  //清除启动记录角度标志位                                  
        }

        
        
        //无人操作停机检测
        Motor_Timcount++;                     
        if(Motor_Timcount >=DEVICE_DOWNTIME)
        {
         //判断电机状态
           Motor_Timcount = 4000;
           if(Motor_status == 1)//启动了，将其关闭
           {
                DriverContrlOff();              
                Motor_status = 0;  //将电机状态位置为关闭状态
           }
       }
    }      
}

/*!
*  \brief     传感器模式控制
*  \param  TorqueValue   扭矩值
*  \param  SpannerValue  扳手系数
*  \param  Preset_sensor  预设扭矩
*/
void SensorMode_Control(u8 KeyState,u32 SpannerValue,u32 TorqueValue,u8 OilReturnTime)
{
    
    float Send_PressureTemp = 0;
    float Target_pressure = 0; //目标压力
    Target_pressure = (float)TorqueValue/SpannerValue*10000.0;
     //停止键   
    if(KeyState == 3)    
    {           
        Motor_Timcount = 0;   //有按键操作，清空计数
        //判断电机状态
        if(Motor_status == 1) //启动了，将其关闭
        {
            KMCTRL1_OFF;
            KMCTRL6_OFF;            
            Motor_status = 0; //将电机状态位置为关闭状态
        }
        SolenoidValveOFF();   //关闭电磁阀            
    }
     //自动键
    else if(KeyState == 2 )   
    {
        Motor_Timcount = 0;        //有按键操作，清空计数
        
        SolenoidValve_status = 1;  //电磁阀自动状态启动
        DriveControl_status = 1;   //自动
        //检测电机是否启动
        if(Motor_status == 0) //没有启动，将其启动    
        {
            KMCTRL1_ON;    //电机
            KMCTRL6_ON;    //风扇        
            Motor_status = 1;              
        }         

        if(ADC_PressureValue >= Target_pressure && Pressure_status ==0 )    //到达设定的压力值
        {
            Send_PressureTemp = ADC_PressureValue -(ADC_PressureValue*0.04);  //记录到达设定压力值
            printf("Send_PressureTemp:%f\r\n",Send_PressureTemp);
            if(Send_PressureTemp > Target_pressure*1.03 || Send_PressureTemp < Target_pressure*0.97)
            {
                
            }
            else
            {
                Send_PressureValue = Send_PressureTemp;
                printf("Send_PressureValue:%f\r\n",Send_PressureValue);
            }
            StartUp_State = 1;            //记录             
            Pressure_status = 1;       
            SolenoidValveOFF(); //关闭电磁阀
            WANING_LED_OFF;   
        }else if(ADC_PressureValue< RET_PRESSURE_MAX && ADC_PressureValue >= RET_PRESSURE_MIN &&Pressure_status ==1)//到达回油值
        {           
            OSTimeDly(OilReturnTime*200);//回油等待时间 
            StartUp_State = 0;           //使用状态启动             
            Pressure_status = 0;
            SolenoidValveON(); //打开电磁阀
            WANING_LED_ON;   
        }
        else if(ADC_PressureValue< RET_PRESSURE_MAX &&Pressure_status ==0)//未到达
        {
            StartUp_State = 0;           //使用状态启动 
            SolenoidValveON(); //打开电磁阀 
            WANING_LED_ON;                  
        }
    }
    //手动键
    else if(KeyState == 1 )  
    {   
        Motor_Timcount = 0;        
        DriveControl_status = 0;   //手动
        //检测电机状态是否启动
        if(Motor_status == 0) //没有启动，将其启动    
        {
            KMCTRL1_ON;    //电机
            KMCTRL6_ON;    //风扇                   
            Motor_status = 1; //将电机状态位置为启动状态
        }  
        StartUp_State = 0;        //使用状态启动 
        SolenoidValve_status = 2;  //电磁阀打开
        if(Send_PressureValue < 20)
        {
           Send_PressureValue = ADC_PressureValue + 4.5;  //记录到达设定压力值 
        }else
        {            
            Send_PressureValue = ADC_PressureValue + 3;  //记录到达设定压力值
        }
//        printf("Send_PressureValue:%f\r\n",Send_PressureValue);        
        SolenoidValveON(); //打开电磁阀
        STAR_LED_ON;       //测试灯开启
                         
    } 
    else if(KeyState == 0)  //无按键操作自动停止电机
    {
        if(SolenoidValve_status == 2)
        {
            StartUp_State = 1;     //使用状态关闭 
            SolenoidValve_status = 0;
            SolenoidValveOFF(); //关闭电磁阀
            STAR_LED_OFF;             //测试灯关闭   
        }
        if(SolenoidValve_status == 1)
        {
            StartUp_State = 2;           //使用状态启动 
            Pressure_status = 0;
            SolenoidValve_status = 0;
            SolenoidValveOFF(); //关闭电磁阀
            WANING_LED_OFF;                              
        }      
        Motor_Timcount++;
        if(Motor_Timcount >=DEVICE_DOWNTIME)    //30秒按键无操作
        {
         //判断电机状态
           Motor_Timcount = 4000;
           if(Motor_status == 1)//启动了，将其关闭
           {
                DriverContrlOff();              
                Motor_status = 0;  //将电机状态位置为关闭状态
           }
       }
    }   
}



//-------------------------------------------------------------------------------------------------------------------------

void Key_Control(SpannerStruct *pata)
{
    static u16 Info_Times = 0;
    
    u8 Key_Status = 0; 
    int k_SpannerCoefficient = 0;
    int k_UserTargetTorque = 0;    //用户设定的扭矩
    int k_BoltTorque = 0;          //螺栓扭矩
    int k_GearTorque = 0;          //档位扭矩
    u16 k_TargetAngle = 0;         //预设角度
    int k_TargetSensor = 0;        //预设扭矩（传感器模式）
    int k_TargetUsageNumber = 0;   //目标使用次数
    u8  k_Mode = 0;                //当前模式
    u8  k_OilReturnTime = 0;        //回油时间  
    
    k_SpannerCoefficient = pata->p_SpannerCoefficient;
    k_UserTargetTorque   = pata->p_UserTargetTorque;
    k_BoltTorque         = pata->p_BoltTorque;
    k_GearTorque         = pata->p_GearTorque;
    k_TargetAngle        = pata->p_TargetAngle; //测试
    k_TargetSensor       = pata->p_TargetSensor;
    k_Mode               = pata->p_Mode;
    k_OilReturnTime      = pata->p_OilReturnTime;
    

    //获取按键状态
    Key_Status = KEY_Scan(1);
#if 0    
Info_Times++;
if(Info_Times >=  10)
{    
    printf("k_SpannerCoefficient:%d\r\n",k_SpannerCoefficient); 
    printf("k_UserTargetTorque:%d\r\n",k_UserTargetTorque); 
    printf("k_BoltTorque:%d\r\n",k_BoltTorque); 
    printf("k_GearTorque:%d\r\n",k_GearTorque); 
    printf("k_TargetAngle:%d\r\n",k_TargetAngle); 
    printf("key_state:%d\r\n",Key_Status);
    printf("k_Mode:%d\r\n",k_Mode); 
    printf("k_OilReturnTime:%d\r\n",k_OilReturnTime); 
    Info_Times = 0;
}
#endif
    switch(k_Mode)         //当前模式
    {        
        case MENU_MODE:
                DriverContrlOff(); //驱动设备全关        
        break;
        case TORQUE_MODE:
                TorqueMode_Control(Key_Status,k_SpannerCoefficient,k_UserTargetTorque,k_OilReturnTime);    
        break;
        case BOLT_MODE:
                BoltMode_Control(Key_Status,k_SpannerCoefficient,k_BoltTorque,k_OilReturnTime);       
        break;
        case GEAR_MODE:
                GearMode_Control(Key_Status,k_SpannerCoefficient,k_GearTorque,k_OilReturnTime);
        break;
        case ANGEL_MODE:
                AngleMode_Control(Key_Status,k_SpannerCoefficient,k_UserTargetTorque,ADC_AngleValue,k_TargetAngle,k_OilReturnTime);   
        break;
        case SENSOR_MODE:
                SensorMode_Control(Key_Status,k_SpannerCoefficient,k_UserTargetTorque,k_OilReturnTime);  
        break; 
    }   
      
}
