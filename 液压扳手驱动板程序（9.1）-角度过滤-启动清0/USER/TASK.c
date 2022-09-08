#include "includes.h"
#include "TASK.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "gpio.h"
#include "adc.h"
#include "Hydraulic_control.h"
#include "MX25L6406.h"
#include "Data_process.h"
#include "hmi_user_uart.h"
#include "hmi_user_uart_driver.h"
#include "iwdg.h"
#include "ads8688.h"

SpannerStruct SpannerDate = {0};
FreeDogStruct FreeDogStructVar;

//SYS任务
void SYS_task(void *pdata)
{
	
    static uint8 NeedToResT = 0; 
    static uint8 ucCount1s = 10;
    static uint8 ucCount500ms = 5;
    pdata = pdata;
    while(1)
    {	
     
		OSTimeDly(20); 
//Test
#if  0
        if(Timer_count >= 500)  //测试充当压力变化1s加1
       {
           ADC_PressureValue++;
           Timer_count = 0;
       }
       if(ADC_PressureValue >= 70)
       {
           ADC_PressureValue= 0;
       }
       printf("ADC_PressureValue:%f\r\n",ADC_PressureValue);
#endif 
    if(!(--ucCount500ms))
    {
             ucCount500ms = 5;
            if(NeedToResT==0)//如果需要复位，将不再进行喂狗，等待看门狗进行复位
            {	
                IWDG_Feed();
            } 
            else
            {	
                printf("复位\r\n");
                
            }
         					 
    }
			 
	if(!(--ucCount1s))
    {
			ucCount1s = 10;
			
			FreeDogStructVar.FlashTaskDog++;
			FreeDogStructVar.AdcTaskDog++;
			FreeDogStructVar.ControlTaskDog++;
			FreeDogStructVar.Usart4TaskDog++;
			FreeDogStructVar.UsartsendTaskDog++;
		if(FreeDogStructVar.FlashTaskDog>30)//这里计时，
			{
			  NeedToResT=1;
			}
		if(	FreeDogStructVar.AdcTaskDog>5)//这里计时，
			{
			  NeedToResT=1;
			}
			
		if(FreeDogStructVar.ControlTaskDog>30)//这里计时，
			{
				
			  NeedToResT=1;
			}
		if(FreeDogStructVar.Usart4TaskDog>5)//这里计时，
			{
				NeedToResT=1;  
			}
   		if(FreeDogStructVar.UsartsendTaskDog>5)//这里计时，
			{
				NeedToResT=1;  
			}	
	
    }
	
}
	
}

//扳手控制任务
void Control_task(void *pdata)
{
    pdata = pdata;
   // printf("controlflag == %d\r\n",Controlflag);
	while(1)
	{
        OSTimeDly(2);   //1 = 5ms
        SYS_LED_ON;
        FreeDogStructVar.ControlTaskDog = 0;
        if(Usage_Counter >= SpannerDate.p_TargetUsageNumber && Passwordflag == 1)//当使用次数到达设定次数且设定密码，就上锁
        {
            Controlflag = 1;
        }
        else                                                                   //当使用次数未达到设定次数且无设置密码或者解锁状态，就解锁
        {
            Controlflag = 0;
        }
        
       // printf("p_mode == %d\r\n",SpannerDate.p_Mode);
        if(Controlflag == 0)    //未锁
        {

            Key_Control(&SpannerDate);
        }
        else
        {
            //无法操作
        }
        
      //  printf("扳手轶诺科技\r\n");
	};
}
//ADC任务
void ADC_task(void *pdata)
{	 
    pdata = pdata;
	while(1)
	{
        OSTimeDly(1); 
        FreeDogStructVar.AdcTaskDog = 0; 
       ADC_PressureValue = Get_PressureValue();    //读ad压力
            
//        ADC_AngleValue = Get_AngleValue();          //读ad角度
//        ADC_AngleValue = Get_angle_calculattion();    //读传感器角度 程序中获取，不需要再次传递
//       printf("ADC_PressureValue:%f\r\n",ADC_AngleValue);   
	};
}
//串口通信发送任务
void Usart4RevSend_task(void *pdata)
{	  
    pdata = pdata;
	while(1)
	{
        OSTimeDly(2);
        FreeDogStructVar.Usart4TaskDog = 0;
        UartDriver();   //解析指令
     //   printf("串口轶诺科技\r\n");
	};
}
//存储任务
void Flash_task(void *pdata)
{	  
    pdata = pdata;
	while(1)
	{
        OSTimeDly(2);  
        FreeDogStructVar.FlashTaskDog = 0;      
        Flash_Model_parameters(&Data_Storage);//变量改变时储存变量
     //   printf("存储轶诺科技\r\n");
	};
}

//监听任务
void UsartSend_task(void *pdata)
{	  
    pdata = pdata;
	while(1)
	{
        OSTimeDly(2);  
        FreeDogStructVar.UsartsendTaskDog = 0;
        Send_DisplayData(SpannerDate.p_Mode,Usage_Counter,ADC_PressureValue);
        Send_Pressure_Value(Send_PressureValue);
        Send_Angle_Value(Send_AngleValue);
        Send_StartUp_State(StartUp_State); 
        Send_Contrl_Status(DriveControl_status);
    //    printf("监听轶诺科技\r\n");
	};
}
