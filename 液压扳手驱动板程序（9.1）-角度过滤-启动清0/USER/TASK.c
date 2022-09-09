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

//SYS����
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
        if(Timer_count >= 500)  //���Գ䵱ѹ���仯1s��1
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
            if(NeedToResT==0)//�����Ҫ��λ�������ٽ���ι�����ȴ����Ź����и�λ
            {	
                IWDG_Feed();
            } 
            else
            {	
                printf("��λ\r\n");
                
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
		if(FreeDogStructVar.FlashTaskDog>30)//�����ʱ��
			{
			  NeedToResT=1;
			}
		if(	FreeDogStructVar.AdcTaskDog>5)//�����ʱ��
			{
			  NeedToResT=1;
			}
			
		if(FreeDogStructVar.ControlTaskDog>30)//�����ʱ��
			{
				
			  NeedToResT=1;
			}
		if(FreeDogStructVar.Usart4TaskDog>5)//�����ʱ��
			{
				NeedToResT=1;  
			}
   		if(FreeDogStructVar.UsartsendTaskDog>5)//�����ʱ��
			{
				NeedToResT=1;  
			}	
	
    }
	
}
	
}

//���ֿ�������
void Control_task(void *pdata)
{
    pdata = pdata;
   // printf("controlflag == %d\r\n",Controlflag);
	while(1)
	{
        OSTimeDly(2);   //1 = 5ms
        SYS_LED_ON;
        FreeDogStructVar.ControlTaskDog = 0;
        if(Usage_Counter >= SpannerDate.p_TargetUsageNumber && Passwordflag == 1)//��ʹ�ô��������趨�������趨���룬������
        {
            Controlflag = 1;
        }
        else                                                                   //��ʹ�ô���δ�ﵽ�趨������������������߽���״̬���ͽ���
        {
            Controlflag = 0;
        }
        
       // printf("p_mode == %d\r\n",SpannerDate.p_Mode);
        if(Controlflag == 0)    //δ��
        {

            Key_Control(&SpannerDate);
        }
        else
        {
            //�޷�����
        }
        
      //  printf("������ŵ�Ƽ�\r\n");
	};
}
//ADC����
void ADC_task(void *pdata)
{	 
    pdata = pdata;
	while(1)
	{
        OSTimeDly(1); 
        FreeDogStructVar.AdcTaskDog = 0; 
       ADC_PressureValue = Get_PressureValue();    //��adѹ��
            
//        ADC_AngleValue = Get_AngleValue();          //��ad�Ƕ�
//        ADC_AngleValue = Get_angle_calculattion();    //���������Ƕ� �����л�ȡ������Ҫ�ٴδ���
//       printf("ADC_PressureValue:%f\r\n",ADC_AngleValue);   
	};
}
//����ͨ�ŷ�������
void Usart4RevSend_task(void *pdata)
{	  
    pdata = pdata;
	while(1)
	{
        OSTimeDly(2);
        FreeDogStructVar.Usart4TaskDog = 0;
        UartDriver();   //����ָ��
     //   printf("������ŵ�Ƽ�\r\n");
	};
}
//�洢����
void Flash_task(void *pdata)
{	  
    pdata = pdata;
	while(1)
	{
        OSTimeDly(2);  
        FreeDogStructVar.FlashTaskDog = 0;      
        Flash_Model_parameters(&Data_Storage);//�����ı�ʱ�������
     //   printf("�洢��ŵ�Ƽ�\r\n");
	};
}

//��������
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
    //    printf("������ŵ�Ƽ�\r\n");
	};
}
