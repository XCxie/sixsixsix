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
#define RET_PRESSURE_MIN   6    //����ѹ����Сֵ
#define RET_PRESSURE_MAX   10   //����ѹ�����ֵ
#define DEVICE_DOWNTIME   1500  //�豸ͣ��ʱ��
#define SETZERO_TIME     150
u8 SolenoidValve_status = 0;//��ŷ�״̬
u8 Pressure_status = 0;     //ѹ��״̬
u8 Motor_status = 0;        //���״̬
u8 StartUp_State = 0;       //����״̬��¼


float ADC_PressureValue = 0;  //ʵʱ�ɼ�ѹ��
float ADC_AngleValue = 0;     //ʵʱ�ɼ��Ƕ�ֵ
int Usage_Counter = 0; //ʹ�ô��� 
u8 DriveControl_status  = 0;
u16 Motor_Timcount = 0;
u8 Operation_Status = 0;
u8 AngleControl_State = 0;

float AngleMark = 0;
float Record_Angle = 0;  //��¼�Ƕ�
float TargetAngle = 0;//�Ƕȹ���ֵ k_TargetAngle;//�Ƕȹ���ֵ
u16 g_data = 0;//����Ƕȱ�־λ��ֻ��ȡһ��

float Send_PressureValue = 0;  //��¼����ѹ��
float Send_AngleValue = 0;  //��¼���ͽǶ�
void SolenoidValveOFF(void)
{
    //����220vһ��24v��ŷ�
    KMCTRL3_OFF; 
    KMCTRL4_OFF;
    KMCTRL5_OFF; 
}
void SolenoidValveON(void)
{
    //����220vһ��24v��ŷ�
    KMCTRL3_ON;
    KMCTRL4_ON;
    KMCTRL5_ON; 
}

//�������ƹر�
void DriverContrlOff (void)
{
    KMCTRL1_OFF;
    KMCTRL3_OFF;
    KMCTRL4_OFF;
    KMCTRL5_OFF;
    KMCTRL6_OFF;    //����
    WANING_LED_OFF;
    STAR_LED_OFF; 
}

/*!
*  \brief     Ť��ģʽ����
*  \param  TorqueValue   Ť��ֵ
*  \param  SpannerValue  ����ϵ��
*/
void TorqueMode_Control(u8 KeyState,u32 SpannerValue,u32 TorqueValue,u8 OilReturnTime)
{   
    float Send_PressureTemp = 0;
    float Target_pressure = 0; //Ŀ��ѹ��
    Target_pressure = (float)TorqueValue/SpannerValue*10000.0;
    if(Target_pressure >= 70)
    {
        Target_pressure = 70;
    }
//    printf("Target_pressure:%f\r\n",Target_pressure); 
    //ֹͣ��   
    if(KeyState == 3)    
    {           
        Motor_Timcount = 0;   //�а�����������ռ���
        //�жϵ��״̬
        if(Motor_status == 1) //�����ˣ�����ر�
        {
            KMCTRL1_OFF;
            KMCTRL6_OFF;            
            Motor_status = 0; //�����״̬λ��Ϊ�ر�״̬
        }
        SolenoidValveOFF();   //�رյ�ŷ�            
    }
     //�Զ���
    else if(KeyState == 2 )   
    {
        Motor_Timcount = 0;        //�а�����������ռ���
        
        SolenoidValve_status = 1;  //��ŷ��Զ�״̬����
        DriveControl_status = 1;   //�Զ�
        //������Ƿ�����
        if(Motor_status == 0) //û����������������    
        {
            KMCTRL1_ON;    //���
            
            KMCTRL6_ON;    //����        
            Motor_status = 1;              
        }         
        if(ADC_PressureValue >= Target_pressure && Pressure_status ==0)    //�����趨��ѹ��ֵ
        {
            Send_PressureTemp = ADC_PressureValue -(ADC_PressureValue*0.04);  //��¼�����趨ѹ��ֵ
            printf("Send_PressureTemp:%f\r\n",Send_PressureTemp);
            if(Send_PressureTemp > Target_pressure*1.03 || Send_PressureTemp < Target_pressure*0.97)
            {
                
            }
            else
            {
                Send_PressureValue = Send_PressureTemp;
                printf("Send_PressureValue:%f\r\n",Send_PressureValue);
            }
            StartUp_State = 1;            //��¼             
            Pressure_status = 1;       
            SolenoidValveOFF(); //�رյ�ŷ�
            WANING_LED_OFF;   
        }else if(ADC_PressureValue< RET_PRESSURE_MAX && ADC_PressureValue >= RET_PRESSURE_MIN &&Pressure_status ==1)//�������ֵ
        {   
            OSTimeDly(OilReturnTime*200);//���͵ȴ�ʱ�� 
            StartUp_State = 0;           //ʹ��״̬����             
            Pressure_status = 0;
            SolenoidValveON(); //�򿪵�ŷ�
            WANING_LED_ON;            
        }
        else if(ADC_PressureValue< RET_PRESSURE_MAX &&Pressure_status ==0)//δ����
        {
            StartUp_State = 0;           //ʹ��״̬���� 
            SolenoidValveON(); //�򿪵�ŷ� 
            WANING_LED_ON;                
        }
    }
    //�ֶ���
    else if(KeyState == 1 )  
    {   
        Motor_Timcount = 0;        
        DriveControl_status = 0;   //�ֶ�
        //�����״̬�Ƿ�����
        if(Motor_status == 0) //û����������������    
        {
            KMCTRL1_ON;    //���
            KMCTRL6_ON;    //����                   
            Motor_status = 1; //�����״̬λ��Ϊ����״̬
        }  
        StartUp_State = 0;        //ʹ��״̬���� 
        SolenoidValve_status = 2;  //��ŷ���
        if(Send_PressureValue < 20)
        {
           Send_PressureValue = ADC_PressureValue + 4.5;  //��¼�����趨ѹ��ֵ 
        }else
        {            
            Send_PressureValue = ADC_PressureValue + 3;  //��¼�����趨ѹ��ֵ
        }
//        printf("Send_PressureValue:%f\r\n",Send_PressureValue);        
        SolenoidValveON(); //�򿪵�ŷ�
        STAR_LED_ON;       //���Եƿ���
                         
    } 
    else if(KeyState == 0)  //�ް��������Զ�ֹͣ���
    {
        if(SolenoidValve_status == 2)
        {
            StartUp_State = 1;     //ʹ��״̬�ر� 
            SolenoidValve_status = 0;
            SolenoidValveOFF(); //�رյ�ŷ�
            STAR_LED_OFF;             //���Եƹر�   
        }
        if(SolenoidValve_status == 1)
        {
            StartUp_State = 2;           //ʹ��״̬���� 
            Pressure_status = 0;
            SolenoidValve_status = 0;
            SolenoidValveOFF(); //�رյ�ŷ�
            WANING_LED_OFF;                              
        }      
        Motor_Timcount++;
        if(Motor_Timcount >=DEVICE_DOWNTIME)    //30�밴���޲���
        {
         //�жϵ��״̬
           Motor_Timcount = 4000;
           if(Motor_status == 1)//�����ˣ�����ر�
           {
                DriverContrlOff();              
                Motor_status = 0;  //�����״̬λ��Ϊ�ر�״̬
           }
       }
    }
}



/*!
*  \brief     ��˨ģʽ����
*  \param  TorqueValue   Ť��ֵ
*  \param  SpannerValue  ����ϵ��
*/
void BoltMode_Control(u8 KeyState,u32 SpannerValue,u32 TorqueValue,u8 OilReturnTime)
{
    float Send_PressureTemp = 0;
    float Target_pressure = 0; //Ŀ��ѹ��
    Target_pressure = (float)TorqueValue/SpannerValue*10000.0;//ϵ��/Ť�� = Ŀ��ѹ��
//    printf("ADC_PressureValue:%f\r\n",ADC_PressureValue); 
    if(Target_pressure >= 70)
    {
        Target_pressure = 70;
    }
    //ֹͣ��   
    if(KeyState == 3)    
    {           
        Motor_Timcount = 0;   //�а�����������ռ���
        //�жϵ��״̬
        if(Motor_status == 1) //�����ˣ�����ر�
        {
            KMCTRL1_OFF;
            KMCTRL6_OFF;            
            Motor_status = 0; //�����״̬λ��Ϊ�ر�״̬
        }
        SolenoidValveOFF();   //�رյ�ŷ�            
    }
     //�Զ���
    else if(KeyState == 2 )   
    {
        Motor_Timcount = 0;        //�а�����������ռ���
        
        SolenoidValve_status = 1;  //��ŷ��Զ�״̬����
        DriveControl_status = 1;   //�Զ�
        //������Ƿ�����
        if(Motor_status == 0) //û����������������    
        {
            KMCTRL1_ON;    //���
            KMCTRL6_ON;    //����        
            Motor_status = 1;              
        }         
        if(Target_pressure < 20)
        {
            Target_pressure = Target_pressure - 2;
        }
        printf("ADC_P == %f\r\n",ADC_PressureValue);
        if(ADC_PressureValue >= Target_pressure && Pressure_status ==0  )    //�����趨��ѹ��ֵ
        {
            Send_PressureTemp = ADC_PressureValue -(ADC_PressureValue*0.04);  //��¼�����趨ѹ��ֵ
            printf("Send_PressureTemp:%f\r\n",Send_PressureTemp);
            if(Send_PressureTemp > Target_pressure*1.03 || Send_PressureTemp < Target_pressure*0.97)
            {
                
            }
            else
            {
                Send_PressureValue = Send_PressureTemp;
                printf("Send_PressureValue:%f\r\n",Send_PressureValue);
            }
            StartUp_State = 1;            //��¼             
            Pressure_status = 1;       
            SolenoidValveOFF(); //�رյ�ŷ�
            WANING_LED_OFF;   
        }else if(ADC_PressureValue< RET_PRESSURE_MAX && ADC_PressureValue >= RET_PRESSURE_MIN &&Pressure_status ==1)//�������ֵ
        {           
            OSTimeDly(OilReturnTime*200);//���͵ȴ�ʱ�� 
            StartUp_State = 0;           //ʹ��״̬����             
            Pressure_status = 0;
            SolenoidValveON(); //�򿪵�ŷ�
            WANING_LED_ON;   
        }
        else if(ADC_PressureValue< RET_PRESSURE_MAX &&Pressure_status ==0)//δ����
        {
            StartUp_State = 0;           //ʹ��״̬���� 
            SolenoidValveON(); //�򿪵�ŷ� 
            WANING_LED_ON;                  
        }
    }
    //�ֶ���
    else if(KeyState == 1 )  
    {   
        Motor_Timcount = 0;        
        DriveControl_status = 0;   //�ֶ�
        //�����״̬�Ƿ�����
        if(Motor_status == 0) //û����������������    
        {
            KMCTRL1_ON;    //���
            KMCTRL6_ON;    //����                   
            Motor_status = 1; //�����״̬λ��Ϊ����״̬
        }  
        StartUp_State = 0;        //ʹ��״̬���� 
        SolenoidValve_status = 2;  //��ŷ���
        if(Send_PressureValue < 20)
        {
           Send_PressureValue = ADC_PressureValue + 4.5;  //��¼�����趨ѹ��ֵ 
        }else
        {            
            Send_PressureValue = ADC_PressureValue + 3;  //��¼�����趨ѹ��ֵ
        }
//        printf("Send_PressureValue:%f\r\n",Send_PressureValue);        
        SolenoidValveON(); //�򿪵�ŷ�
        STAR_LED_ON;       //���Եƿ���
                         
    } 
    else if(KeyState == 0)  //�ް��������Զ�ֹͣ���
    {
        if(SolenoidValve_status == 2)
        {
            StartUp_State = 1;     //ʹ��״̬�ر� 
            SolenoidValve_status = 0;
            SolenoidValveOFF(); //�رյ�ŷ�
            STAR_LED_OFF;             //���Եƹر�   
        }
        if(SolenoidValve_status == 1)
        {
            StartUp_State = 2;           //ʹ��״̬���� 
            Pressure_status = 0;
            SolenoidValve_status = 0;
            SolenoidValveOFF(); //�رյ�ŷ�
            WANING_LED_OFF;                              
        }      
        Motor_Timcount++;
        if(Motor_Timcount >=DEVICE_DOWNTIME)    //30�밴���޲���
        {
         //�жϵ��״̬
           Motor_Timcount = 4000;
           if(Motor_status == 1)//�����ˣ�����ر�
           {
                DriverContrlOff();              
                Motor_status = 0;  //�����״̬λ��Ϊ�ر�״̬
           }
       }
    }      
}

/*!
*  \brief     ��λģʽ����
*  \param  TorqueValue   Ť��ֵ
*  \param  SpannerValue  ����ϵ��
*/
void GearMode_Control(u8 KeyState,u32 SpannerValue,u32 TorqueValue,u8 OilReturnTime)
{
    float Send_PressureTemp = 0;
    float Target_pressure = 0; //Ŀ��ѹ��
    Target_pressure = (float)TorqueValue/SpannerValue*10000.0;
    if(Target_pressure >= 70)
    {
        Target_pressure = 70;
    }
   // printf("KeyState:%d\r\n",KeyState); 
    //ֹͣ��   
    if(KeyState == 3)    
    {           
        Motor_Timcount = 0;   //�а�����������ռ���
        //�жϵ��״̬
        if(Motor_status == 1) //�����ˣ�����ر�
        {
            KMCTRL1_OFF;
            KMCTRL6_OFF;            
            Motor_status = 0; //�����״̬λ��Ϊ�ر�״̬
        }
        SolenoidValveOFF();   //�رյ�ŷ�            
    }
     //�Զ���
    else if(KeyState == 2 )   
    {
        Motor_Timcount = 0;        //�а�����������ռ���
        
        SolenoidValve_status = 1;  //��ŷ��Զ�״̬����
        DriveControl_status = 1;   //�Զ�
        //������Ƿ�����
        if(Motor_status == 0) //û����������������    
        {
            KMCTRL1_ON;    //���
            KMCTRL6_ON;    //����        
            Motor_status = 1;              
        }         

        if(ADC_PressureValue >= Target_pressure && Pressure_status ==0 )    //�����趨��ѹ��ֵ
        {
            Send_PressureTemp = ADC_PressureValue -(ADC_PressureValue*0.04);  //��¼�����趨ѹ��ֵ
            printf("Send_PressureTemp:%f\r\n",Send_PressureTemp);
            if(Send_PressureTemp > Target_pressure*1.03 || Send_PressureTemp < Target_pressure*0.97)
            {
                
            }
            else
            {
                Send_PressureValue = Send_PressureTemp;
                printf("Send_PressureValue:%f\r\n",Send_PressureValue);
            }
            StartUp_State = 1;            //��¼             
            Pressure_status = 1;       
            SolenoidValveOFF(); //�رյ�ŷ�
            WANING_LED_OFF;   
        }else if(ADC_PressureValue< RET_PRESSURE_MAX && ADC_PressureValue >= RET_PRESSURE_MIN &&Pressure_status ==1)//�������ֵ
        {           
            OSTimeDly(OilReturnTime*200);//���͵ȴ�ʱ�� 
            StartUp_State = 0;           //ʹ��״̬����             
            Pressure_status = 0;
            SolenoidValveON(); //�򿪵�ŷ�
            WANING_LED_ON;   
        }
        else if(ADC_PressureValue< RET_PRESSURE_MAX &&Pressure_status ==0)//δ����
        {
            StartUp_State = 0;           //ʹ��״̬���� 
            SolenoidValveON(); //�򿪵�ŷ� 
            WANING_LED_ON;                  
        }
    }
    //�ֶ���
    else if(KeyState == 1 )  
    {   
        Motor_Timcount = 0;        
        DriveControl_status = 0;   //�ֶ�
        //�����״̬�Ƿ�����
        if(Motor_status == 0) //û����������������    
        {
            KMCTRL1_ON;    //���
            KMCTRL6_ON;    //����                   
            Motor_status = 1; //�����״̬λ��Ϊ����״̬
        }  
        StartUp_State = 0;        //ʹ��״̬���� 
        SolenoidValve_status = 2;  //��ŷ���
        if(Send_PressureValue < 20)
        {
           Send_PressureValue = ADC_PressureValue + 4.5;  //��¼�����趨ѹ��ֵ 
        }else
        {            
            Send_PressureValue = ADC_PressureValue + 3;  //��¼�����趨ѹ��ֵ
        }
       
        SolenoidValveON(); //�򿪵�ŷ�
     
        STAR_LED_ON;       //���Եƿ���
                         
    } 
    else if(KeyState == 0)  //�ް��������Զ�ֹͣ���
    {
        if(SolenoidValve_status == 2)
        {
            StartUp_State = 1;     //ʹ��״̬�ر� 
            SolenoidValve_status = 0;
            SolenoidValveOFF(); //�رյ�ŷ�
            STAR_LED_OFF;             //���Եƹر�   
        }
        if(SolenoidValve_status == 1)
        {
            StartUp_State = 2;           //ʹ��״̬���� 
            Pressure_status = 0;
            SolenoidValve_status = 0;
            SolenoidValveOFF(); //�رյ�ŷ�
            WANING_LED_OFF;                              
        }      
        Motor_Timcount++;
        if(Motor_Timcount >=DEVICE_DOWNTIME)    //30�밴���޲���
        {
         //�жϵ��״̬
           Motor_Timcount = 4000;
           if(Motor_status == 1)//�����ˣ�����ر�
           {
                DriverContrlOff();              
                Motor_status = 0;  //�����״̬λ��Ϊ�ر�״̬
           }
       }
    }       
}



/*!
*  \brief     �Ƕ�ģʽ����
*  \param  TorqueValue   Ť��ֵ
*  \param  SpannerValue  ����ϵ��
*  \param  Preset_Angle  Ԥ��Ƕ�
*/

//float Record_Angle = 0;  //��¼�Ƕ�
//u16 g_data = 0;//����Ƕȱ�־λ��ֻ��ȡһ��

void AngleMode_Control(u8 KeyState,u32 SpannerValue,u32 TorqueValue,float Current_Angle,u16 Preset_Angle,u8 OilReturnTime)
{
    float Send_PressureTemp = 0;
    static float Ago_Angle = 0; //����ʱ��ʼ�Ƕ�
    static u8 ControlStart_Flag = 0;
    static u8 autoAngle_flag = 0;
    float Target_pressure = 0; //Ŀ��ѹ��
    static u32 Set_Zero_Time = 0;
    u8 Angle_Num = 0;
    u8 Set_Zero_Flag = 0;
    Target_pressure = (float)TorqueValue/SpannerValue*10000.0;
    
    if(Target_pressure >= 70)
    {
        Target_pressure = 70;
    } 
    //ֹͣ��   
    if(KeyState == 3)    
    {           
        Motor_Timcount = 0;   //�а�����������ռ���
        //�жϵ��״̬
        if(Motor_status == 1)//�����ˣ�����ر�
        {
            KMCTRL1_OFF;
            KMCTRL6_OFF;            
            Motor_status = 0;  //�����״̬λ��Ϊ�ر�״̬
        }
        SolenoidValveOFF();
        g_data = 0;
        usCaptureRisingVal = 0;
        Record_Angle = 0;
        AngleMark = 0;//�Ƕȹ��ȹ���ֵ��0        
    }
     //�Զ���
    else if(KeyState == 2 )   
    {
        Record_Angle = Get_angle_calculattion();    //��ȡ�Ƕ�
        Motor_Timcount = 0;        //�а�����������ռ���
        StartUp_State = 0;
        SolenoidValve_status = 1;  //��ŷ��Զ�״̬����
        DriveControl_status = 1;   //�Զ�     

        //������Ƿ�����
        if(Motor_status == 0) //û����������������    
        {
            KMCTRL1_ON;    //���
            KMCTRL6_ON;    //����        
            Motor_status = 1;               
        } 
       if(g_data == 0){
            TargetAngle = Preset_Angle;
            printf("oneline\r\n");
            g_data = 1;
        }
        if(Record_Angle <= TargetAngle) //ÿ�λ�ȡ�ĽǶ�
        {
            if(ADC_PressureValue >= Target_pressure && Pressure_status ==0)    //�����趨��ѹ��ֵ
            {
                Send_PressureTemp = ADC_PressureValue - (ADC_PressureValue*0.04);  //��¼�����趨ѹ��ֵ
					 printf("1AngleMark:%f\r\n",AngleMark);	
					 printf("1Record_Angle:%f\r\n",Record_Angle);
					 printf("1Preset_Angle:%d\r\n",Preset_Angle);
						// ��ȡ�м�ֵ
					if(Record_Angle > 24) {
						Record_Angle = 24;
					}
					 AngleMark = AngleMark + Record_Angle;//ʵʱ�Ƕ�
					 TargetAngle = Preset_Angle - AngleMark;//ʣ��ĽǶ�     ��ֹͣʱ���Ƕȼ�¼   
					
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
					 
					 Record_Angle = 0;//ÿ������ǰ��0
					 usCaptureRisingVal = 0;
					 
                WANING_LED_OFF; 
            }                
				else if(ADC_PressureValue< RET_PRESSURE_MAX && ADC_PressureValue >= RET_PRESSURE_MIN &&Pressure_status ==1)//�������ֵ
            {                               
					  OSTimeDly(OilReturnTime*200);//���͵ȴ�ʱ�� 					 					
					  Pressure_status = 0;
					  Record_Angle = 0;//ÿ������ǰ��0
					  usCaptureRisingVal = 0;      
					  printf("2AngleMark:%f\r\n",AngleMark);	
					  printf("2Record_Angle:%f\r\n",Record_Angle);
				     printf("2Preset_Angle:%d\r\n",Preset_Angle);	
					  SolenoidValveON();
					  WANING_LED_ON;
         
            }
            else if(ADC_PressureValue< RET_PRESSURE_MAX &&Pressure_status == 0)//��������һֱ��0
            {
					
                SolenoidValveON();  
                WANING_LED_ON;                  
            }
			}
        else       
        {
            if(autoAngle_flag ==0)   //����Ƕ�ִ��һ��
            {
                Send_AngleValue = AngleMark + Record_Angle;  //��¼�Ƕ� 
					 printf("Send_AngleValue == %.2f\r\n",Send_AngleValue);               
                //����Ŀ��Ƕ�                 
                SolenoidValveOFF();  //�ر�
                
                WANING_LED_OFF;
                AngleControl_State = 1;
                StartUp_State = 0;
                autoAngle_flag = 1;
            }
        }
    }
    //�ֶ���
    else if(KeyState == 1 )  
    {  
        Record_Angle = Get_angle_calculattion();    //��ȡ�Ƕ�
        Motor_Timcount = 0;        
        DriveControl_status = 0;   //�ֶ�
        //�����״̬�Ƿ�����
        if(Motor_status == 0) //û����������������    
        {
            KMCTRL1_ON;    //���
            KMCTRL6_ON;    //����                   
            Motor_status = 1; //�����״̬λ��Ϊ����״̬
        }
        StartUp_State = 0;        //ʹ��״̬���� 
        SolenoidValve_status = 2;  //��ŷ��� 
        if(Send_PressureValue < 20)
        {
           Send_PressureValue = ADC_PressureValue + 4.5;  //��¼�����趨ѹ��ֵ 
        }else
        {            
            Send_PressureValue = ADC_PressureValue + 3;  //��¼�����趨ѹ��ֵ
        }
        printf("Send_PressureValue:%f\r\n",Send_PressureValue);          
        SolenoidValveON();  
        STAR_LED_ON;       //���Եƿ���        
                     
    } 
    //�ް��������Զ�ֹͣ���
    else if(KeyState == 0) 
    {
        if(SolenoidValve_status == 2)
        {
            SolenoidValveOFF(); 
            
   
            
            STAR_LED_OFF;                        //���Եƹر�  
            Send_AngleValue = Record_Angle/1.19;
				Record_Angle = 0;
				usCaptureRisingVal = 0;
            StartUp_State = 1;     //ʹ��״̬�ر� 

            SolenoidValve_status = 0;
            ControlStart_Flag = 0;  //���������¼�Ƕȱ�־λ     
        }
        
        
        if(SolenoidValve_status == 1)
        { 
            SolenoidValveOFF(); 
            
            usCaptureRisingVal = 0;//����ֵ��0
            Record_Angle = 0;//��¼�Ƕ���0
            AngleMark = 0;//�Ƕȹ��ȹ���ֵ��0
            
            WANING_LED_OFF; 
            g_data = 0;//����ֵ������0
			  
            StartUp_State = 2;           //ʹ��״̬����
            autoAngle_flag = 0;
            Pressure_status = 0;
            SolenoidValve_status = 0;              
            AngleControl_State = 0;            
            ControlStart_Flag = 0;  //���������¼�Ƕȱ�־λ                                  
        }

        
        
        //���˲���ͣ�����
        Motor_Timcount++;                     
        if(Motor_Timcount >=DEVICE_DOWNTIME)
        {
         //�жϵ��״̬
           Motor_Timcount = 4000;
           if(Motor_status == 1)//�����ˣ�����ر�
           {
                DriverContrlOff();              
                Motor_status = 0;  //�����״̬λ��Ϊ�ر�״̬
           }
       }
    }      
}

/*!
*  \brief     ������ģʽ����
*  \param  TorqueValue   Ť��ֵ
*  \param  SpannerValue  ����ϵ��
*  \param  Preset_sensor  Ԥ��Ť��
*/
void SensorMode_Control(u8 KeyState,u32 SpannerValue,u32 TorqueValue,u8 OilReturnTime)
{
    
    float Send_PressureTemp = 0;
    float Target_pressure = 0; //Ŀ��ѹ��
    Target_pressure = (float)TorqueValue/SpannerValue*10000.0;
     //ֹͣ��   
    if(KeyState == 3)    
    {           
        Motor_Timcount = 0;   //�а�����������ռ���
        //�жϵ��״̬
        if(Motor_status == 1) //�����ˣ�����ر�
        {
            KMCTRL1_OFF;
            KMCTRL6_OFF;            
            Motor_status = 0; //�����״̬λ��Ϊ�ر�״̬
        }
        SolenoidValveOFF();   //�رյ�ŷ�            
    }
     //�Զ���
    else if(KeyState == 2 )   
    {
        Motor_Timcount = 0;        //�а�����������ռ���
        
        SolenoidValve_status = 1;  //��ŷ��Զ�״̬����
        DriveControl_status = 1;   //�Զ�
        //������Ƿ�����
        if(Motor_status == 0) //û����������������    
        {
            KMCTRL1_ON;    //���
            KMCTRL6_ON;    //����        
            Motor_status = 1;              
        }         

        if(ADC_PressureValue >= Target_pressure && Pressure_status ==0 )    //�����趨��ѹ��ֵ
        {
            Send_PressureTemp = ADC_PressureValue -(ADC_PressureValue*0.04);  //��¼�����趨ѹ��ֵ
            printf("Send_PressureTemp:%f\r\n",Send_PressureTemp);
            if(Send_PressureTemp > Target_pressure*1.03 || Send_PressureTemp < Target_pressure*0.97)
            {
                
            }
            else
            {
                Send_PressureValue = Send_PressureTemp;
                printf("Send_PressureValue:%f\r\n",Send_PressureValue);
            }
            StartUp_State = 1;            //��¼             
            Pressure_status = 1;       
            SolenoidValveOFF(); //�رյ�ŷ�
            WANING_LED_OFF;   
        }else if(ADC_PressureValue< RET_PRESSURE_MAX && ADC_PressureValue >= RET_PRESSURE_MIN &&Pressure_status ==1)//�������ֵ
        {           
            OSTimeDly(OilReturnTime*200);//���͵ȴ�ʱ�� 
            StartUp_State = 0;           //ʹ��״̬����             
            Pressure_status = 0;
            SolenoidValveON(); //�򿪵�ŷ�
            WANING_LED_ON;   
        }
        else if(ADC_PressureValue< RET_PRESSURE_MAX &&Pressure_status ==0)//δ����
        {
            StartUp_State = 0;           //ʹ��״̬���� 
            SolenoidValveON(); //�򿪵�ŷ� 
            WANING_LED_ON;                  
        }
    }
    //�ֶ���
    else if(KeyState == 1 )  
    {   
        Motor_Timcount = 0;        
        DriveControl_status = 0;   //�ֶ�
        //�����״̬�Ƿ�����
        if(Motor_status == 0) //û����������������    
        {
            KMCTRL1_ON;    //���
            KMCTRL6_ON;    //����                   
            Motor_status = 1; //�����״̬λ��Ϊ����״̬
        }  
        StartUp_State = 0;        //ʹ��״̬���� 
        SolenoidValve_status = 2;  //��ŷ���
        if(Send_PressureValue < 20)
        {
           Send_PressureValue = ADC_PressureValue + 4.5;  //��¼�����趨ѹ��ֵ 
        }else
        {            
            Send_PressureValue = ADC_PressureValue + 3;  //��¼�����趨ѹ��ֵ
        }
//        printf("Send_PressureValue:%f\r\n",Send_PressureValue);        
        SolenoidValveON(); //�򿪵�ŷ�
        STAR_LED_ON;       //���Եƿ���
                         
    } 
    else if(KeyState == 0)  //�ް��������Զ�ֹͣ���
    {
        if(SolenoidValve_status == 2)
        {
            StartUp_State = 1;     //ʹ��״̬�ر� 
            SolenoidValve_status = 0;
            SolenoidValveOFF(); //�رյ�ŷ�
            STAR_LED_OFF;             //���Եƹر�   
        }
        if(SolenoidValve_status == 1)
        {
            StartUp_State = 2;           //ʹ��״̬���� 
            Pressure_status = 0;
            SolenoidValve_status = 0;
            SolenoidValveOFF(); //�رյ�ŷ�
            WANING_LED_OFF;                              
        }      
        Motor_Timcount++;
        if(Motor_Timcount >=DEVICE_DOWNTIME)    //30�밴���޲���
        {
         //�жϵ��״̬
           Motor_Timcount = 4000;
           if(Motor_status == 1)//�����ˣ�����ر�
           {
                DriverContrlOff();              
                Motor_status = 0;  //�����״̬λ��Ϊ�ر�״̬
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
    int k_UserTargetTorque = 0;    //�û��趨��Ť��
    int k_BoltTorque = 0;          //��˨Ť��
    int k_GearTorque = 0;          //��λŤ��
    u16 k_TargetAngle = 0;         //Ԥ��Ƕ�
    int k_TargetSensor = 0;        //Ԥ��Ť�أ�������ģʽ��
    int k_TargetUsageNumber = 0;   //Ŀ��ʹ�ô���
    u8  k_Mode = 0;                //��ǰģʽ
    u8  k_OilReturnTime = 0;        //����ʱ��  
    
    k_SpannerCoefficient = pata->p_SpannerCoefficient;
    k_UserTargetTorque   = pata->p_UserTargetTorque;
    k_BoltTorque         = pata->p_BoltTorque;
    k_GearTorque         = pata->p_GearTorque;
    k_TargetAngle        = pata->p_TargetAngle; //����
    k_TargetSensor       = pata->p_TargetSensor;
    k_Mode               = pata->p_Mode;
    k_OilReturnTime      = pata->p_OilReturnTime;
    

    //��ȡ����״̬
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
    switch(k_Mode)         //��ǰģʽ
    {        
        case MENU_MODE:
                DriverContrlOff(); //�����豸ȫ��        
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
