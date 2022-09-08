#include "TASK.h"
#include "delay.h"
#include "usart.h"
#include "rtc.h" 
#include "gpio.h"
 #include "adc.h"
#include "Hydraulic_control.h"
#include "Hydraulic_process.h"
#include "Driver_user_driver.h"
#include "exti.h"
#include "timer.h"
#include "Data_process.h"
#include "USBType-C.h" 
#include "iwdg.h"
#define CH374HF_NO_CODE 
#include "CH374HFM.H"
#include "CH374DRV.H"

#define Menu_interface_options   8
#define Torque_interface_options 5
#define Bolt_interface_options   4
#define Angle_interface_options  5
#define Gear_interface_options   4
#define Sensor_interface_options 6

Spanner SpannerType[Spanner_MUN] = {0};
Bolt BoltType ={0};
FreeDogStruct FreeDogStructVar = {0};

char  Cursor_id = 0;       //�����
char Set_Language_Type = 0;       //������������   
char Set_Chart_Standard_Type = 0; //����ͼ���׼����
char Set_ProductStandard_Type = 0;//���ò�Ʒ�������
char Set_Bolt_Diameter = 0;       //������˨ֱ��
char Set_Bolt_Garder = 0;         //������˨�ȼ�
char Set_Torque_Unit = 0;   //����Ť�ص�λ����
char Set_Pressure_Unit_Type = 0;  //����ѹ����λ���� 

int TargetTorque_value = 0;     //Ŀ��Ť��ֵ
short int Set_Target_Angle = 0;       //����Ԥ��Ƕ�ֵ
char Set_Target_Gear = 0;         //����Ŀ�굵λ
int GearTorque_value = 0;//��λŤ��

int Spanner_Coefficien = 0;  //���ֱ���ϵ��
int BoltCoefficient_value = 0;//��˨Ť��
int Set_SensorTorque = 0;     //���ô�����Ԥ��Ť��ֵ
u32 Set_Target_Usage = 0;     //��������ʹ�õĴ���
char  Set_Control_Mode = 0;       //���ÿ���ģʽ
u32 UsageCounter = 0;  //ʹ�ô���
float Pressure_Value = 0; //ѹ��ֵ
u16 Angle_Value  = 0; //ʵʱ�Ƕ�ֵ
u16 Max_Angle = 360; //������ƽǶ�

//SYS����
void SYS_task(void *pdata)
{	 
    u8  i;   
    static uint8 NeedToResT = 0; 
    static uint8 ucCount1s = 10;
    static uint8 ucCount200ms = 2;
    pdata = pdata;
		
	while(1)
	{	
        OSTimeDly(20);   
        //���Ź��������        
		 if(!(--ucCount200ms))
			 {
				 ucCount200ms = 2;
			   if(NeedToResT==0)//�����Ҫ��λ�������ٽ���ι�����ȴ����Ź����и�λ
			    {	
				   IWDG_Feed();
			    }else
                {
                    printf("����λ\r\n");	
                }                    
            }
			 
		if(!(--ucCount1s))
		{
			ucCount1s = 10;
		
			
			FreeDogStructVar.FlashTaskDog++;
			FreeDogStructVar.HiMTaskDog++;
			FreeDogStructVar.TimeTaskDog++;
			FreeDogStructVar.UdiskTaskDog++;
			FreeDogStructVar.UsartTaskDog++;
		if(FreeDogStructVar.FlashTaskDog>60)//�����ʱ��
			{
			  NeedToResT=1;
                	printf("FLASH \r\n");
			}
		if(FreeDogStructVar.HiMTaskDog>5)//�����ʱ��
			{
			  NeedToResT=1;
                	printf("HIM\r\n");
			}
			
		if(FreeDogStructVar.TimeTaskDog>5)//�����ʱ��
			{
			  NeedToResT=1;
                printf("TIME\r\n");
			}
		if(FreeDogStructVar.UdiskTaskDog>60)//�����ʱ��
			{
			  NeedToResT=1;
                	printf("UDISK\r\n");
			}
   			
		if(FreeDogStructVar.UsartTaskDog>5)//�����ʱ��
			{
			  NeedToResT=1;
                	printf("USART\r\n");
			}
						
		}
	
	}
}
//��������շ�������
void UsartDriverRev_task(void *pdata)
{	
    pdata = pdata;
	while(1)
	{	
        OSTimeDly(2);
        FreeDogStructVar.UsartTaskDog = 0;
        UartDriver();   //����ָ��   
	};
}

u8 AutoDatabuff[4][120] ={0};
u8 AutoNumber = 0;
//�洢����
void Flash_task(void *pdata)
{	   
    static u8 RecordFlag = 0;  //��¼���ݱ�־λ��1����ʼ��¼��0��δ��¼��
    static u8 RecordFlag1 = 0;  //��¼���ݱ�־λ��1����ʼ��¼��0��δ��¼��
    static u8 Start_StorageFlag = 0;
    pdata = pdata;
	while(1)
	{
        OSTimeDly(2);
        FreeDogStructVar.FlashTaskDog = 0;
        Flash_Model_parameters(&Data_Storage);    //�洢����	
        if(DriveControl_status == 1 )
        {
            
            if(Set_Control_Mode ==4)  //��Ϊ�Ƕ�ģʽʱ
            {
                if(RecordCount == 0)   
                {
                    Data_Record_Storage(Databuff,RecorddataCount,sizeof(Databuff));	  //��¼ʹ������
                    RecordFlag1 = 1;
                }
                else if(RecordCount ==2 &&RecordFlag1 ==1)
                {    
                     
                    RecorddataCount ++;       //���ݼ�¼������1 
    //                printf( "RecorddataCount:%d\r\n",RecorddataCount );                
                    Data_Storage = 9;                 
                    RecordFlag1 = 0;
                }               
            }
            else
            {
                if(RecordCount == 0)   
                { 
                    Start_StorageFlag = 0;
                    Data_Record_Storage(AutoDatabuff[AutoNumber],RecorddataCount,sizeof(Databuff));	  //��¼ʹ������
                    RecordFlag = 1;
                }
                else if(RecordCount ==1 &&RecordFlag ==1)
                {    
                    if(AutoNumber >2)
                    {    
                        memcpy(AutoDatabuff[0],AutoDatabuff[1],sizeof(Databuff)); 
                        memcpy(AutoDatabuff[1],AutoDatabuff[2],sizeof(Databuff));
                        memcpy(AutoDatabuff[2],AutoDatabuff[3],sizeof(Databuff));
                        AutoNumber = 3;                      
                    }
                    else
                    {
                        AutoNumber ++;  
                    }                
                   
                    RecordFlag = 0;
                }
                else if(RecordCount ==2 && Start_StorageFlag == 0)
                {
                        RecorddataCount  +=  AutoNumber ;    //���ݼ�¼���� 
    //                    printf( "RecorddataCount:%d\r\n",RecorddataCount ); 
                        Data_Storage = 9;    
                        Start_StorageFlag = 1;
                }
            }
        }
        else
        {
            if(RecordCount == 0)   
            {
                Data_Record_Storage(Databuff,RecorddataCount,sizeof(Databuff));	  //��¼ʹ������
                RecordFlag1 = 1;
            }
            else if(RecordCount ==1 &&RecordFlag1 ==1)
            {    
                 
                RecorddataCount ++;       //���ݼ�¼������1                
                Data_Storage = 9;                 
                RecordFlag1 = 0;
            }
            
        }
        
	}
}


//U�̶�д�ļ�����
void Udisk_task(void *pdata)
{	 
    u8 i;   
    pdata = pdata;
	while(1)
	{
        OSTimeDly(10);       
		while ( CH374DiskStatus < DISK_CONNECT ) {            /* ��ѯCH375�жϲ������ж�״̬,�ȴ�U�̲��� */
            FreeDogStructVar.UdiskTaskDog = 0;
//            printf( "ON\n" );  
            OSTimeDly(10);
			if ( CH374DiskConnect( ) == ERR_SUCCESS ) break;  /* ���豸�����򷵻سɹ�,CH375DiskConnectͬʱ�����ȫ�ֱ���CH375DiskStatus */
			
		}
		
//		printf("Disk Init\n");
		for ( i = 0; i < 5; i ++ ) {                          /* �е�U�����Ƿ���δ׼����,�������Ա����� */
            OSTimeDly(5);
//			printf( "Ready ?\n" );
			if ( CH374DiskReady( ) == ERR_SUCCESS ) break;    /* ��ѯ�����Ƿ�׼���� */
		}
		
/* ��ѯ������������ */
//		printf( "DiskSize\n" );
		i = CH374DiskSize( );  
		mStopIfError( i );
//		printf( "TotalSize = %u MB \n", (unsigned int)( mCmdParam.DiskSize.mDiskSizeSec * (CH374vSectorSize/512) / 2048 ) );  // ��ʾΪ��MBΪ��λ������
		// ԭ���㷽�� (unsigned int)( mCmdParam.DiskSize.mDiskSizeSec * CH374vSectorSize / 1000000 ) �п���ǰ����������˺������, �����޸ĳ���ʽ
		SYS_LED_ON;
/* �������ļ� */
//		printf( "Create\n" );                                 //���ڴ���ʱ��ᳬ�� 
		mCmdParam.Create.mPathName[0] = '/';
        mCmdParam.Create.mPathName[1] = 'D';
        mCmdParam.Create.mPathName[2] = 'A';
        mCmdParam.Create.mPathName[3] = 'T';
        mCmdParam.Create.mPathName[4] = 'A';
		mCmdParam.Create.mPathName[5] = '.';
		mCmdParam.Create.mPathName[6] = 'C';
        mCmdParam.Create.mPathName[7] = 'S';
        mCmdParam.Create.mPathName[8] = 'V';
		mCmdParam.Create.mPathName[9] = 0;
		
		i = CH374FileCreate( );                               /* �½��ļ�����,����ļ��Ѿ���������ɾ�������½� */
		mStopIfError( i );

        WriteTail();   //д������
		
//		mCmdParam.ByteWrite.mByteCount = 5;                   /* ָ������д����ֽ���,���ζ�д�ĳ��Ȳ��ܳ���MAX_BYTE_IO */
//		i = CH374ByteWrite( );                                /* ���ֽ�Ϊ��λ���ļ�д������,���ζ�д�ĳ��Ȳ��ܳ���MAX_BYTE_IO */
//		mStopIfError( i );
        SYS_LED_OFF;
//		printf( "Close\n" );
		mCmdParam.Close.mUpdateLen = 1;                       /* ��Ҫ�Զ������ļ�����,����Զ�����,��ô�ó�������CH374vSectorSize�ı��� */
		i = CH374FileClose( );
		mStopIfError( i );		
		
//		printf( "Take out\n" );
		while ( 1 ) { 
                               
            OSTimeDly(200); /* û��ҪƵ����ѯ */
            FreeDogStructVar.UdiskTaskDog = 0;
			if ( CH374DiskConnect( ) != ERR_SUCCESS ) break;  /* ��ѯ��ʽ: �������Ƿ����Ӳ����´���״̬,���سɹ�˵������ */
		}
	};
}

//TIME����
void Time_task(void *pdata)
{	  
    pdata = pdata;
	while(1)
	{
        OSTimeDly(2); 
        FreeDogStructVar.TimeTaskDog = 0;
        TypeC_UartDriver();        //����λ��ͨ�Ž������� 
        
	};
}

//����������
void HIM_task(void *pdata)
{	 
    float ProductCoefficient_value = 0;//����ʵ��ϵ�� 
    u8 BoltCoefficient_type = 0;     //��˨���    
	u8 KEY_State = 0;
    u32 Torque_val = 0;
    u8 Gear_level = 0;
    u32 Gear_parameter = 0;
    u32 Gear_val = 0;
    float Pressure_val = 0;
    static u32 Display_Press_Times = 0;
    pdata = pdata;
	while(1)
	{
        OSTimeDly(5);  
        FreeDogStructVar.HiMTaskDog = 0;
        WANING_LED_ON;        
        HIM_Home_Switcher(current_screen_id);    //��ʾ�л����� 
        Spanner_Coefficien =  Spanner_type_parameter(Set_ProductStandard_Type); //��ȡ��Ʒϵ��  
        ProductCoefficient_value = Spanner_Coefficien/10000.0;//����ʵ��ϵ��ֵ
        Get_Bolttorque_parameter(Set_ProductStandard_Type);
        BoltCoefficient_value = Select_Bolttorque_parameter(Set_Chart_Standard_Type,Set_Bolt_Garder,Set_Bolt_Diameter);//�����˨Ŀ��Ť��
        //��λ����
        Gear_level = Gears_data[Set_Target_Gear].Levels;  //��λ�ȼ�
        Gear_parameter = Gears_data[Set_Target_Gear].parameter;//��λ��Ӧѹ��
        GearTorque_value = Get_GearTorqueValue(ProductCoefficient_value,Gear_parameter);//��λŤ��
        Gear_val = Get_GearTorque(Set_Torque_Unit,GearTorque_value);  //��λŤ�ص�λ����          
        Send_DriverParameter();   //�������巢��һ�β���
        //�жϰ���״̬        
        if(KEY_Type == 0)
        {
            KEY_State = KEY_Scan(1);  //��е����
        }
        else if(KEY_Type ==1)
        {
            KEY_State =EC11_status;   //��ť����
            KEY_Type = 0;               
        }
        switch(current_screen_id)
        {
/*--------------------------------------------�˵�����----------------------------------------------------------------------------*/
            case Menu_Main_Interface:                                                                    //�˵����� 
                        Set_Control_Mode = 0;  //�˵�ģʽ
                        HIM_Language_display(Set_Language_Type);//��ʾ��ǰ����
                        HIM_ChartStandard_display(Set_Chart_Standard_Type,Set_Language_Type);//��ʾ��ǰͼ���׼         
                        DisplayTime();//ˢ����ʾRTCʱ��
                        HIM_Cursor(Menu_Main_Interface, Cursor_id);//��ʾ���
                        SetTextInt32(0,9,UsageCounter,0,0);//��ʾʹ�ô���
                        switch(KEY_State)
                        {				 
                            case KEY2_PRES:                     //�����Ϸ�							
                                if(Cursor_id >= Menu_interface_options) Cursor_id = 1;	   
                                else Cursor_id++;                   
                                break;
                            case KEY1_PRES:                     //�����·�
                                if(Cursor_id <= 1) Cursor_id = Menu_interface_options;	  
                                else Cursor_id--;              
                                break;
                            case EKEY3_PRES:                     //��ť˳ʱ��							
                                if(Cursor_id >= Menu_interface_options) Cursor_id = 1;	   
                                else Cursor_id++;                   
                                break;
                            case EKEY4_PRES:                     //��ť��ʱ��
                                if(Cursor_id <= 1) Cursor_id = Menu_interface_options;	  
                                else Cursor_id--;              
                                break;
                            case KEY3_PRES:                     //ȷ�ϼ�
                                    switch(Cursor_id)
                                    {
                                        case 1:
                                            current_screen_id = S_Main_language;        //��ת�������ý���
                                            SetTextFlickerCycle(0,1,50); //�ı���˸
                                        break;
                                        case 2:
                                            current_screen_id = S_Main_ChartStandard;  //��תͼ���׼���ý���
                                            SetTextFlickerCycle(0,2,50); //�ı���˸
                                        break;
                                        case 3:
                                            current_screen_id = S_Main_year;        //��ת�������ý���
                                            SetTextFlickerCycle(0,3,50); //�ı���˸
                                        break;
                                        case 4:
                                            current_screen_id = S_Main_month;        //��ת�������ý���
                                            SetTextFlickerCycle(0,4,50); //�ı���˸
                                        break;
                                        case 5:
                                            current_screen_id = S_Main_day;        //��ת�������ý���
                                            SetTextFlickerCycle(0,5,50); //�ı���˸
                                        break;
                                        case 6:
                                            current_screen_id = S_Main_hour;        //��ת�������ý���
                                            SetTextFlickerCycle(0,6,50); //�ı���˸
                                        break;
                                        case 7:
                                            current_screen_id = S_Main_minute;        //��ת�������ý���
                                            SetTextFlickerCycle(0,7,50); //�ı���˸
                                        break;
                                        case 8:
                                            current_screen_id = S_Main_second;        //��ת�������ý���
                                            SetTextFlickerCycle(0,8,50); //�ı���˸
                                        break;
                                    } 
                                    
                                break;	
                            case KEY4_Long_PRES:				             //�˵���                                      
                                    current_screen_id = Menu_Torque_Interface;//�л�Ť��ģʽ����
                                    Cursor_id = 0;                  //�������־λ
                                break;	                
                        }          
            break;
            case S_Main_language:                               //�������ý���
                        HIM_Language_display(Set_Language_Type);   //��ʾ����
                        HIM_ChartStandard_display(Set_Chart_Standard_Type,Set_Language_Type);//��ʾ��ǰͼ���׼   
                        switch(KEY_State)
                        {				 
                            case KEY2_PRES:                     //�Ϸ���
                                if(Set_Language_Type >= 1) Set_Language_Type = 0;	   
                                else Set_Language_Type++;
                                break;
                            case KEY1_PRES:                     //�·���
                                if(Set_Language_Type <= 0) Set_Language_Type = 1;	   
                                else Set_Language_Type--;
                                break;
                            case EKEY3_PRES:                     //�Ϸ���							
                                if(Set_Language_Type >= 1) Set_Language_Type = 0;	   
                                else Set_Language_Type++;                  
                                break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_Language_Type <= 0) Set_Language_Type = 1;	   
                                else Set_Language_Type--;            
                                break;
                            case KEY3_PRES:                     //ȷ�ϼ�
                                SetTextFlickerCycle(0,1,0); //ֹͣ�ı���˸
                                current_screen_id = Menu_Main_Interface;   //���ز˵�����
                                break;	
                        }                                         
            break; 
            case S_Main_ChartStandard:                               //ͼ���׼���ý���
                        HIM_ChartStandard_display(Set_Chart_Standard_Type,Set_Language_Type);//��ʾ��ǰͼ���׼   
                        switch(Set_Chart_Standard_Type)
                        {				 
                            case 0:                   
                                Set_Torque_Unit = 1;//n.m
                                Set_Pressure_Unit_Type = 2;  //PSi
                                break;
                            case 1:              
                                Set_Torque_Unit = 0;//n.m
                                Set_Pressure_Unit_Type = 0;  //bar
                                break;
                        }
                        switch(KEY_State)
                        {				 
                            case KEY2_PRES:                     //�Ϸ���
                                if(Set_Chart_Standard_Type >= 1) Set_Chart_Standard_Type = 0;	   
                                else Set_Chart_Standard_Type++;
                                break;
                            case KEY1_PRES:                     //�·���
                                if(Set_Chart_Standard_Type <= 0) Set_Chart_Standard_Type = 1;	  
                                else Set_Chart_Standard_Type--;
                                break;
                             case EKEY3_PRES:                     //�Ϸ���							
                                if(Set_Chart_Standard_Type >= 1) Set_Chart_Standard_Type = 0;	   
                                else Set_Chart_Standard_Type++;                 
                                break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_Chart_Standard_Type <= 0) Set_Chart_Standard_Type = 1;	  
                                else Set_Chart_Standard_Type--;          
                                break;
                            case KEY3_PRES:                     //ȷ�ϼ�
                                SetTextFlickerCycle(0,2,0); //ֹͣ�ı���˸
                                current_screen_id = Menu_Main_Interface;   //���ز˵�����
                                Set_Bolt_Diameter = 0;   //�����˨ֱ���ͺű�־λ
                                Set_Bolt_Garder = 0;		 //�����˨�ȼ��ͺű�־λ
                                break;	
                        }                                         
            break;  
            case S_Main_year:                               //������ý���
                    RTC_EnterConfigMode();     //�������� 
                    switch(KEY_State)
                    {	
                        //����
                        case KEY1_PRES:                     //�Ϸ���
                            if(calendar.w_year >= 2099) calendar.w_year = 1970;	//��ķ�ΧΪ1970-2099
                            else calendar.w_year++;
                            SetTextInt32(0,3,calendar.w_year,0,4);// ��ʾ���
                            break;
                        case KEY2_PRES:                     //�·���
                            if(calendar.w_year <= 1970) calendar.w_year = 2099;
                            else calendar.w_year--;
                            SetTextInt32(0,3,calendar.w_year,0,4);// ��ʾ���
                            break;
                        case KEY2_Long_PRES:                     //�Ϸ���
                            if(calendar.w_year >= 2099) calendar.w_year = 1970;	//��ķ�ΧΪ1970-2099
                            else calendar.w_year+= 10;
                            SetTextInt32(0,3,calendar.w_year,0,4);// ��ʾ���
                            break;
                        case KEY1_Long_PRES:                     //�·���
                            if(calendar.w_year <= 1970) calendar.w_year = 2099;
                            else calendar.w_year-= 10;
                            SetTextInt32(0,3,calendar.w_year,0,4);// ��ʾ���
                            break;
                        //��ť
                        case EKEY3_PRES:                     //�Ϸ���
                            if(calendar.w_year >= 2099) calendar.w_year = 1970;	//��ķ�ΧΪ1970-2099
                            else calendar.w_year++;
                            SetTextInt32(0,3,calendar.w_year,0,4);// ��ʾ���
                            break;
                        case EKEY4_PRES:                     //�·���
                            if(calendar.w_year <= 1970) calendar.w_year = 2099;
                            else calendar.w_year--;
                            SetTextInt32(0,3,calendar.w_year,0,4);// ��ʾ���
                            break;
                        case EKEY2_PRES:                     //EC11 �Ϸ���
                            if(calendar.w_year >= 2099) calendar.w_year = 1970;	//��ķ�ΧΪ1970-2099
                            else calendar.w_year += 10;
                            SetTextInt32(0,3,calendar.w_year,0,4);// ��ʾ���
                            break;
                        case EKEY1_PRES:                     //EC11  �·���
                            if(calendar.w_year <= 1970) calendar.w_year = 2099;
                            else calendar.w_year -= 10;
                            SetTextInt32(0,3,calendar.w_year,0,4);// ��ʾ���
                            break;
                        case KEY3_PRES:                     //ȷ�ϼ�
                            SetTextFlickerCycle(0,3,0);              //ֹͣ�ı���˸                            
                            current_screen_id = Menu_Main_Interface;   //���ز˵�����
                            break;	
                    }
                    RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);//RTCʱ������                    
                    RTC_ExitConfigMode();      //�˳�����ģʽ                          
            break;    
            case S_Main_month:                               //�·����ý���
                        RTC_EnterConfigMode();     //�������� 
                        switch(KEY_State)
                        {				 
                            case KEY1_PRES:                     //�Ϸ���
                                if(calendar.w_month >= 12) calendar.w_month = 1;   //�·ݵķ�ΧΪ1-12
                                else calendar.w_month++;
                                SetTextInt32(0,4,calendar.w_month,0,2);// ��ʾ�·�
                                break;
                            case KEY2_PRES:                     //�·���
                                if(calendar.w_month <= 1) calendar.w_month = 12;
                                else calendar.w_month--;
                                SetTextInt32(0,4,calendar.w_month,0,2);// ��ʾ�·�
                                break;
                            case KEY2_Long_PRES:                     //�Ϸ���
                                if(calendar.w_month >= 12) calendar.w_month = 1;   //�·ݵķ�ΧΪ1-12
                                else calendar.w_month++;
                                SetTextInt32(0,4,calendar.w_month,0,2);// ��ʾ�·�
                            break;
                        case KEY1_Long_PRES:                     //�·���
                                if(calendar.w_month <= 1) calendar.w_month = 12;
                                else calendar.w_month--;
                                SetTextInt32(0,4,calendar.w_month,0,2);// ��ʾ�·�
                            break;
                            case EKEY3_PRES:                     //�Ϸ���
                                if(calendar.w_month >= 12) calendar.w_month = 1;   //�·ݵķ�ΧΪ1-12
                                else calendar.w_month++;
                                SetTextInt32(0,4,calendar.w_month,0,2);// ��ʾ�·�
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(calendar.w_month <= 1) calendar.w_month = 12;
                                else calendar.w_month--;
                                SetTextInt32(0,4,calendar.w_month,0,2);// ��ʾ�·�
                            break;
                            case EKEY2_PRES:                     //EC11 �Ϸ���
                                if(calendar.w_month >= 12) calendar.w_month = 1;   //�·ݵķ�ΧΪ1-12
                                else calendar.w_month++;
                                SetTextInt32(0,4,calendar.w_month,0,2);// ��ʾ�·�
                            break;
                        case EKEY1_PRES:                     //EC11  �·���
                                if(calendar.w_month <= 1) calendar.w_month = 12;
                                else calendar.w_month--;
                                SetTextInt32(0,4,calendar.w_month,0,2);// ��ʾ�·�
                            break;
                            case KEY3_PRES:                     //ȷ�ϼ�  
                                SetTextFlickerCycle(0,4,0);              //ֹͣ�ı���˸                                  
                                current_screen_id = Menu_Main_Interface;  //���ز˵�����
                                break;	
                        }  
                        RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
                        RTC_ExitConfigMode();      //�˳�����ģʽ                          
            break;  
            case S_Main_day:                               //�����ý���
                        RTC_EnterConfigMode();     //�������� 
                        switch(KEY_State)
                        {				                               
                            case KEY1_PRES:                     //�Ϸ���
                                if(Is_Leap_Year(calendar.w_year)&&calendar.w_month == 2)  //�������·�Ϊ2ʱ��ΧΪ1-29
                                {	
                                    if(calendar.w_date >= 29) calendar.w_date = 1;
                                    else calendar.w_date++;	
                                }
                                else 											   //��ͨ������·ݱ������Χ
                                {	
                                    if(calendar.w_date >= mon_table[calendar.w_month - 1]) calendar.w_date = 1;
                                    else calendar.w_date++;
                                }
                                SetTextInt32(0,5,calendar.w_date,0,2);// ��ʾ  ��
                                break;
                            case KEY2_PRES:                     //�·���
                                if(Is_Leap_Year(calendar.w_year)&&calendar.w_month == 2)
                                {	
                                    if(calendar.w_date <= 1) calendar.w_date = 29;
                                    else calendar.w_date--;	
                                }
                                else 
                                {	
                                    if(calendar.w_date <= 1) calendar.w_date = mon_table[calendar.w_month - 1];
                                    else calendar.w_date--;
                                }
                                SetTextInt32(0,5,calendar.w_date,0,2);//��ʾ  ��
                                break;
                            case KEY2_Long_PRES:                     //�Ϸ���
                                if(Is_Leap_Year(calendar.w_year)&&calendar.w_month == 2)  //�������·�Ϊ2ʱ��ΧΪ1-29
                                {	
                                    if(calendar.w_date >= 29) calendar.w_date = 1;
                                    else calendar.w_date++;	
                                }
                                else 											   //��ͨ������·ݱ������Χ
                                {	
                                    if(calendar.w_date >= mon_table[calendar.w_month - 1]) calendar.w_date = 1;
                                    else calendar.w_date++;
                                }
                                SetTextInt32(0,5,calendar.w_date,0,2);// ��ʾ  ��
                            break;
                        case KEY1_Long_PRES:                     //�·���
                                if(Is_Leap_Year(calendar.w_year)&&calendar.w_month == 2)
                                {	
                                    if(calendar.w_date <= 1) calendar.w_date = 29;
                                    else calendar.w_date--;	
                                }
                                else 
                                {	
                                    if(calendar.w_date <= 1) calendar.w_date = mon_table[calendar.w_month - 1];
                                    else calendar.w_date--;
                                }
                                SetTextInt32(0,5,calendar.w_date,0,2);//��ʾ  ��
                            break;
                            case EKEY3_PRES:                     //�Ϸ���
                                if(Is_Leap_Year(calendar.w_year)&&calendar.w_month == 2)  //�������·�Ϊ2ʱ��ΧΪ1-29
                                {	
                                    if(calendar.w_date >= 29) calendar.w_date = 1;
                                    else calendar.w_date++;	
                                }
                                else 											   //��ͨ������·ݱ������Χ
                                {	
                                    if(calendar.w_date >= mon_table[calendar.w_month - 1]) calendar.w_date = 1;
                                    else calendar.w_date++;
                                }
                                SetTextInt32(0,5,calendar.w_date,0,2);// ��ʾ  ��
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(Is_Leap_Year(calendar.w_year)&&calendar.w_month == 2)
                                {	
                                    if(calendar.w_date <= 1) calendar.w_date = 29;
                                    else calendar.w_date--;	
                                }
                                else 
                                {	
                                    if(calendar.w_date <= 1) calendar.w_date = mon_table[calendar.w_month - 1];
                                    else calendar.w_date--;
                                }
                                SetTextInt32(0,5,calendar.w_date,0,2);//��ʾ  ��
                            break;
                            case EKEY2_PRES:                     //EC11 �Ϸ���
                                if(Is_Leap_Year(calendar.w_year)&&calendar.w_month == 2)  //�������·�Ϊ2ʱ��ΧΪ1-29
                                {	
                                    if(calendar.w_date >= 29) calendar.w_date = 1;
                                    else calendar.w_date++;	
                                }
                                else 											   //��ͨ������·ݱ������Χ
                                {	
                                    if(calendar.w_date >= mon_table[calendar.w_month - 1]) calendar.w_date = 1;
                                    else calendar.w_date++;
                                }
                                SetTextInt32(0,5,calendar.w_date,0,2);// ��ʾ  ��
                            break;
                        case EKEY1_PRES:                     //EC11  �·���
                                if(Is_Leap_Year(calendar.w_year)&&calendar.w_month == 2)
                                {	
                                    if(calendar.w_date <= 1) calendar.w_date = 29;
                                    else calendar.w_date--;	
                                }
                                else 
                                {	
                                    if(calendar.w_date <= 1) calendar.w_date = mon_table[calendar.w_month - 1];
                                    else calendar.w_date--;
                                }
                                SetTextInt32(0,5,calendar.w_date,0,2);//��ʾ  ��
                            break;
                            case KEY3_PRES:                     //ȷ�ϼ�
                                SetTextFlickerCycle(0,5,0);              //ֹͣ�ı���˸                                  
                                current_screen_id = Menu_Main_Interface;  //���ز˵�����
                                break;	
                        } 
                        RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);                        
                        RTC_ExitConfigMode();      //�˳�����ģʽ                          
            break;          
            case Date_Set_Hour:                               //ʱ���ý���
                        RTC_EnterConfigMode();     //�������� 
                        switch(KEY_State)
                        {				   
                             //����
                        case KEY1_PRES:                     //�Ϸ���
                            if(calendar.hour >= 23) calendar.hour = 0;	   //Сʱ�ķ�ΧΪ0-23
                            else calendar.hour++;
                            SetTextInt32(0,6,calendar.hour,0,2);// ��ʾСʱ    
                            break;
                        case KEY2_PRES:                     //�·���
                            if(calendar.hour <= 0) calendar.hour = 23;
                            else calendar.hour--;
                            SetTextInt32(0,6,calendar.hour,0,2);// ��ʾСʱ  
                            break;
                        case KEY2_Long_PRES:                     //�Ϸ���
                            if(calendar.hour >= 23) calendar.hour = 0;	   //Сʱ�ķ�ΧΪ0-23
                            else calendar.hour++;
                            SetTextInt32(0,6,calendar.hour,0,2);// ��ʾСʱ    
                            break;
                        case KEY1_Long_PRES:                     //�·���
                            if(calendar.hour <= 0) calendar.hour = 23;
                            else calendar.hour--;
                            SetTextInt32(0,6,calendar.hour,0,2);// ��ʾСʱ  
                            break;
                        //��ť
                        case EKEY3_PRES:                     //�Ϸ���
                            if(calendar.hour >= 23) calendar.hour = 0;	   //Сʱ�ķ�ΧΪ0-23
                            else calendar.hour++;
                            SetTextInt32(0,6,calendar.hour,0,2);// ��ʾСʱ    
                            break;
                        case EKEY4_PRES:                     //�·���
                            if(calendar.hour <= 0) calendar.hour = 23;
                            else calendar.hour--;
                            SetTextInt32(0,6,calendar.hour,0,2);// ��ʾСʱ  
                            break;
                        case EKEY2_PRES:                     //EC11 �Ϸ���
                            if(calendar.hour >= 23) calendar.hour = 0;	   //Сʱ�ķ�ΧΪ0-23
                            else calendar.hour++;
                            SetTextInt32(0,6,calendar.hour,0,2);// ��ʾСʱ    
                            break;
                        case EKEY1_PRES:                     //EC11  �·���
                            if(calendar.hour <= 0) calendar.hour = 23;
                            else calendar.hour--;
                            SetTextInt32(0,6,calendar.hour,0,2);// ��ʾСʱ  
                            break;
                            case KEY3_PRES:                     //ȷ�ϼ�                   
                                SetTextFlickerCycle(0,6,0);              //ֹͣ�ı���˸                                  
                                current_screen_id = Menu_Main_Interface;  //���ز˵�����
                                break;	
                        } 
                        RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
                        RTC_ExitConfigMode();      //�˳�����ģʽ                         
            break;   
            case Date_Set_Minute:                               //�����ý���
                        RTC_EnterConfigMode();     //�������� 
                        switch(KEY_State)
                        {				 
                                 //����
                        case KEY1_PRES:                     //�Ϸ���
                            if(calendar.min >= 59) calendar.min = 0;	   //���ӵķ�ΧΪ0-59
                            else calendar.min++;
                            SetTextInt32(0,7,calendar.min,0,2);// ��ʾ��     
                            break;
                        case KEY2_PRES:                     //�·���
                            if(calendar.min <= 0) calendar.min = 59;
                            else calendar.min--;
                            SetTextInt32(0,7,calendar.min,0,2);// ��ʾ��  
                            break;
                        case KEY2_Long_PRES:                     //�Ϸ���
                            if(calendar.min >= 59) calendar.min = 0;	   //���ӵķ�ΧΪ0-59
                            else calendar.min++;
                            SetTextInt32(0,7,calendar.min,0,2);// ��ʾ��    
                            break;
                        case KEY1_Long_PRES:                     //�·���
                            if(calendar.min <= 0) calendar.min = 59;
                            else calendar.min--;
                            SetTextInt32(0,7,calendar.min,0,2);// ��ʾ�� 
                            break;
                        //��ť
                        case EKEY3_PRES:                     //�Ϸ���
                            if(calendar.min >= 59) calendar.min = 0;	   //���ӵķ�ΧΪ0-59
                            else calendar.min++;
                            SetTextInt32(0,7,calendar.min,0,2);// ��ʾ��    
                            break;
                        case EKEY4_PRES:                     //�·���
                            if(calendar.min <= 0) calendar.min = 59;
                            else calendar.min--;
                            SetTextInt32(0,7,calendar.min,0,2);// ��ʾ��   
                            break;
                        case EKEY2_PRES:                     //EC11 �Ϸ���
                            if(calendar.min >= 59) calendar.min = 0;	   //���ӵķ�ΧΪ0-59
                            else calendar.min++;
                            SetTextInt32(0,7,calendar.min,0,2);// ��ʾ��     
                            break;
                        case EKEY1_PRES:                     //EC11  �·���
                            if(calendar.min <= 0) calendar.min = 59;
                            else calendar.min--;
                            SetTextInt32(0,7,calendar.min,0,2);// ��ʾ��  
                            break;
                            
                            case KEY3_PRES:                     //ȷ�ϼ�                                           
                            SetTextFlickerCycle(0,7,0);              //ֹͣ�ı���˸                                  
                            current_screen_id = Menu_Main_Interface;  //���ز˵�����
                            break;	
                        }  
                        RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
                        RTC_ExitConfigMode();      //�˳�����ģʽ                          
            break;                        
            case Date_Set_Second:                               //�����ý���
                        RTC_EnterConfigMode();     //�������� 
                        switch(KEY_State)
                        {				                  
                             //����
                            case KEY1_PRES:                     //�Ϸ���
                                if(calendar.sec >= 59) calendar.sec = 0;	   //��ķ�ΧΪ0-59
                                else calendar.sec++;
                                SetTextInt32(0,8,calendar.sec,0,2);// ��ʾ��       
                                break;
                            case KEY2_PRES:                     //�·���
                                if(calendar.sec <= 0) calendar.sec = 59;
                                else calendar.sec--;
                                SetTextInt32(0,8,calendar.sec,0,2);// ��ʾ��  
                                break;
                            case KEY2_Long_PRES:                     //�Ϸ���
                               if(calendar.sec >= 59) calendar.sec = 0;	   //��ķ�ΧΪ0-59
                                else calendar.sec++;
                                SetTextInt32(0,8,calendar.sec,0,2);// ��ʾ��     
                                break;
                            case KEY1_Long_PRES:                     //�·���
                                if(calendar.sec <= 0) calendar.sec = 59;
                                else calendar.sec--;
                                SetTextInt32(0,8,calendar.sec,0,2);// ��ʾ��  
                                break;
                            //��ť
                            case EKEY3_PRES:                     //�Ϸ���
                                if(calendar.sec >= 59) calendar.sec = 0;	   //��ķ�ΧΪ0-59
                                else calendar.sec++;
                                SetTextInt32(0,8,calendar.sec,0,2);// ��ʾ��     
                                break;
                            case EKEY4_PRES:                     //�·���
                                if(calendar.sec <= 0) calendar.sec = 59;
                                else calendar.sec--;
                                SetTextInt32(0,8,calendar.sec,0,2);// ��ʾ��   
                                break;
                            case EKEY2_PRES:                     //EC11 �Ϸ���
                                if(calendar.sec >= 59) calendar.sec = 0;	   //��ķ�ΧΪ0-59
                                else calendar.sec++;
                                SetTextInt32(0,8,calendar.sec,0,2);// ��ʾ��    
                                break;
                            case EKEY1_PRES:                     //EC11  �·���
                                if(calendar.sec <= 0) calendar.sec = 59;
                                else calendar.sec--;
                                SetTextInt32(0,8,calendar.sec,0,2);// ��ʾ��  
                                break;
                            case KEY3_PRES:                     //ȷ�ϼ�                           
                                SetTextFlickerCycle(0,8,0);              //ֹͣ�ı���˸                                  
                                current_screen_id = Menu_Main_Interface;  //���ز˵�����
                                break;	
                        } 
                        RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);                          
                        RTC_ExitConfigMode();      //�˳�����ģʽ                          
            break;  
                       
/*--------------------------------------------Ť�ؽ���----------------------------------------------------------------------------*/    
            case Menu_Torque_Interface:                                                                                 //Ť�ؽ���  
                        Set_Control_Mode = 1;  //Ť��ģʽ
                        HIM_Standard_display(1,1,Set_ProductStandard_Type);   //��ʾ�ͺ�
                        HIM_MaxTorque_display(Set_Torque_Unit,ProductCoefficient_value);//��ʾ���Ť��
                        HIM_MaxTorqueUnit_display(Set_Torque_Unit);  //��ʾ���Ť�ص�λ 
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);  //���Ŀ��Ť��
                        HIM_TargetTorque_display(1,4,Torque_val);//��ʾĿ��Ť��
                        HIM_TargetTorqueuint_display(1,5,Set_Torque_Unit);//��ʾĿ��Ť�ص�λ          
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(1,6,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        HIM_ActualPressureuint_display(1,7,Set_Pressure_Unit_Type);//��ʾѹ����λ            
                        HIM_Cursor(Menu_Torque_Interface, Cursor_id); //��ʾ���
                        switch(KEY_State)
                        {
                            case KEY2_PRES:                     //�Ϸ���							
                                if(Cursor_id >= Torque_interface_options) Cursor_id = 1;	 
                                else Cursor_id++;                    
                                break;
                            case KEY1_PRES:                     //�·���
                                if(Cursor_id <= 1) Cursor_id = Torque_interface_options;	  
                                else Cursor_id--;              
                                break;
                            case EKEY3_PRES:                     //�Ϸ���							
                                if(Cursor_id >= Torque_interface_options) Cursor_id = 1;	 
                                else Cursor_id++;                   
                                break;
                            case EKEY4_PRES:                     //�·���
                                if(Cursor_id <= 1) Cursor_id = Torque_interface_options;	  
                                else Cursor_id--;             
                                break;
                            case KEY3_PRES:                     //ȷ�ϼ�
                                    switch(Cursor_id)
                                    {
                                        case 1:
                                            current_screen_id = S_Torque_ProductType;        //��ת��Ʒ�ͺ�����
                                            SetTextFlickerCycle(1,1,50); //�ı���˸
                                        break;
                                        case 2:
                                            current_screen_id = S_Torque_MaxTorqueuint;      //��ת���Ť������
                                            SetTextFlickerCycle(1,3,50); //�ı���˸
                                        break;
                                        case 3:
                                            current_screen_id = S_Torque_TargetTorque;       //��תĿ��Ť������
                                            SetTextFlickerCycle(1,4,50); //�ı���˸
                                        break;
                                        case 4:
                                            current_screen_id = S_Torque_TargetTorqueuint;   //��תĿ��Ť�ص�λ����
                                            SetTextFlickerCycle(1,5,50); //�ı���˸
                                        break;
                                        case 5:
                                            current_screen_id = S_Torque_ActualPressureuint; //��תѹ����λ���ý���
                                            SetTextFlickerCycle(1,7,50); //�ı���˸
                                        break;
                                    }
                                break;	
                            case KEY4_Long_PRES:				             //�˵���                               
                                current_screen_id = Menu_Bolt_Interface;  //�л���˨ģʽ����
                                Cursor_id = 0;
                            break;	                
                        }                    
            break;        
            case S_Torque_ProductType:                                                                              //��Ʒ�ͺ�����
                        HIM_MaxTorque_display(Set_Torque_Unit,ProductCoefficient_value);//��ʾ���Ť��
                        HIM_Standard_display(1,1,Set_ProductStandard_Type);         //��ʾ��Ʒ�ͺ�   
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(1,6,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				             
                            //����
                            case KEY1_PRES:                     //�Ϸ���
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //�ͺ�
                                else Set_ProductStandard_Type++;    
                            break;
                            case KEY2_PRES:                     //�·���
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;   
                            break;
                            case KEY2_Long_PRES:                     //�Ϸ���
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //�ͺ�
                                else Set_ProductStandard_Type++;    
                            break;
                            case KEY1_Long_PRES:                     //�·���
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;  
                            break;
                            //��ť
                            case EKEY3_PRES:                     //�Ϸ���
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //�ͺ�
                                else Set_ProductStandard_Type++;   
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;  
                            break;
                            case EKEY2_PRES:                     //EC11 �Ϸ���
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //�ͺ�
                                else Set_ProductStandard_Type++;    
                            break;
                            case EKEY1_PRES:                     //EC11  �·���
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;   
                            break;
                            case KEY3_PRES:                     //ȷ�ϼ�   
                                    SetTextFlickerCycle(1,1,0); //�ı�ֹͣ��˸                                               
                                    current_screen_id = Menu_Torque_Interface;     //���ز�Ʒ�ͺ�ѡ�����  
                                break;	
                        }                        
            break;   
            case S_Torque_MaxTorqueuint:                                                                           //���Ť�ص�λ����
                        HIM_MaxTorqueUnit_display(Set_Torque_Unit);          //��ʾ���Ť�ص�λ
                        HIM_MaxTorque_display(Set_Torque_Unit,ProductCoefficient_value);//��ʾ���Ť��
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(1,6,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				                            
                            //����
                            case KEY1_PRES:                     //�Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;    
                            break;
                            case KEY2_PRES:                     //�·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY2_Long_PRES:                     //�Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case KEY1_Long_PRES:                     //�·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            //��ť
                            case EKEY3_PRES:                     //�Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--; 
                            break;
                            case EKEY2_PRES:                     //EC11 �Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY1_PRES:                     //EC11  �·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY3_PRES:                     //ȷ�ϼ�                             
                                SetTextFlickerCycle(1,3,0); //�ı�ֹͣ��˸ 
                                current_screen_id = Menu_Torque_Interface;   //����Ť�ؽ���
                                break;	
                        }                         
            break;  
            case S_Torque_TargetTorque:                               //Ŀ��Ť�����ý���
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);  //���Ŀ��Ť��
                        HIM_TargetTorque_display(1,4,Torque_val);//��ʾĿ��Ť��
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(1,6,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                            case KEY1_PRES:                     //�Ϸ���  
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //��ΧΪ0-���Ť��
                                else TargetTorque_value++;                                      
                                break;
                            case KEY2_PRES:                     //�·���                       
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //��ΧΪ0-���Ť��
                                else TargetTorque_value--;                                                           
                                break;
                            case KEY2_Long_PRES:                     //�Ͽ��ٷ���  
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //��ΧΪ0-���Ť��
                                else TargetTorque_value +=111 ;                                      
                                break;
                            case KEY1_Long_PRES:                     //�¿��ٷ���                       
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //��ΧΪ0-���Ť��
                                else TargetTorque_value -= 111;                                                           
                                break;
                            case EKEY3_PRES:                     //�Ϸ���
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //��ΧΪ0-���Ť��
                                else TargetTorque_value++;       
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //��ΧΪ0-���Ť��
                                else TargetTorque_value--; 
                            break;
                            case EKEY2_PRES:                     //��ť˳ʱ��
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //��ΧΪ0-���Ť��
                                else TargetTorque_value +=ROUND_TO_UINT32(ProductCoefficient_value/2);                                   
                                break;
                            case EKEY1_PRES:                     //��ť��ʱ��                      
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //��ΧΪ0-���Ť��
                                else TargetTorque_value -= ROUND_TO_UINT32(ProductCoefficient_value/2);                                                           
                                break;   
                            case KEY3_PRES:                     //ȷ�ϼ�   
                                SetTextFlickerCycle(1,4,0); //�ı�ֹͣ��˸                                  
                                current_screen_id = Menu_Torque_Interface;    //�������ý���
                                break;	
                        }     
            break;
            case S_Torque_TargetTorqueuint:                               //Ŀ��Ť�ص�λ���ý���
                        HIM_TargetTorqueuint_display(1,5,Set_Torque_Unit);  //��ʾĿ��Ť�ص�λ
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);  //���Ŀ��Ť��
                        HIM_TargetTorque_display(1,4,Torque_val);//��ʾĿ��Ť��
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(1,6,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				                   
                             //����
                            case KEY1_PRES:                     //�Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;    
                            break;
                            case KEY2_PRES:                     //�·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY2_Long_PRES:                     //�Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case KEY1_Long_PRES:                     //�·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            //��ť
                            case EKEY3_PRES:                     //�Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--; 
                            break;
                            case EKEY2_PRES:                     //EC11 �Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY1_PRES:                     //EC11  �·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY3_PRES:                     //ȷ�ϼ�    
                                SetTextFlickerCycle(1,5,0); //�ı�ֹͣ��˸                                 
                                current_screen_id = Menu_Torque_Interface;     //�������ý���
                                break;	
                        }                          
            break;   
            case S_Torque_ActualPressureuint:                                       //ѹ����λ����
                        HIM_ActualPressureuint_display(1,7,Set_Pressure_Unit_Type);
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(1,6,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                             //����
                            case KEY1_PRES:                     //�Ϸ���
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type = 0;	  
                                else Set_Pressure_Unit_Type++;    
                            break;
                            case KEY2_PRES:                     //�·���
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type = 2;	   
                                else Set_Pressure_Unit_Type--;  
                            break;
                            case KEY2_Long_PRES:                     //�Ϸ���
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type = 0;	  
                                else Set_Pressure_Unit_Type++;    
                            break;
                            case KEY1_Long_PRES:                     //�·���
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type = 2;	   
                                else Set_Pressure_Unit_Type--;    
                            break;
                            //��ť
                            case EKEY3_PRES:                     //�Ϸ���
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type = 0;	  
                                else Set_Pressure_Unit_Type++;  
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type = 2;	   
                                else Set_Pressure_Unit_Type--;  
                            break;
                            case EKEY2_PRES:                     //EC11 �Ϸ���
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type = 0;	  
                                else Set_Pressure_Unit_Type++;      
                            break;
                            case EKEY1_PRES:                     //EC11  �·���
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type = 2;	   
                                else Set_Pressure_Unit_Type--;    
                            break;
                            case KEY3_PRES:                     //ȷ�ϼ�
                                SetTextFlickerCycle(1,7,0); //�ı�ֹͣ��˸                                 
                                current_screen_id = Menu_Torque_Interface;     //�������ý���
                                break;
                        }                      
            break;                         
// /*--------------------------------------------��˨����----------------------------------------------------------------------------*/        
            case Bolt_Interface:                                                            //��˨���� 
                        Set_Control_Mode = 2;  //��˨ģʽ
                        Bolt_Standard_display(Set_Chart_Standard_Type,Set_Bolt_Diameter);    //��ʾ��˨ֱ��
                        Bolt_Grade_display(Set_Chart_Standard_Type,Set_Bolt_Garder);//��ʾ��˨�ȼ�
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,BoltCoefficient_value);  //���Ŀ��Ť��
                        HIM_TargetTorque_display(2,3,Torque_val);//��ʾ��˨Ŀ��Ť�� 
                        HIM_TargetTorqueuint_display(2,4,Set_Torque_Unit);  //��ʾĿ��Ť�ص�λ
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(2,5,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        HIM_ActualPressureuint_display(2,6,Set_Pressure_Unit_Type);   //��ʾѹ����λ
                        HIM_Cursor(Bolt_Interface, Cursor_id);//��ʾ���
                        switch(KEY_State)
                        {				                
                            case KEY2_PRES:                     //�Ϸ���							
                                if(Cursor_id >= Bolt_interface_options) Cursor_id = 1;	   
                                else Cursor_id++; 
                                HIM_Cursor(Bolt_Interface, Cursor_id);                   
                                break;
                            case KEY1_PRES:                     //�·���
                                if(Cursor_id <= 1) Cursor_id = Bolt_interface_options;	  
                                else Cursor_id--; 
                                HIM_Cursor(Bolt_Interface, Cursor_id);               
                                break;
                            case EKEY3_PRES:                     //�Ϸ���							
                                if(Cursor_id >= Bolt_interface_options) Cursor_id = 1;	   
                                else Cursor_id++; 
                                HIM_Cursor(Bolt_Interface, Cursor_id);                   
                                break;
                            case EKEY4_PRES:                     //�·���
                                if(Cursor_id <= 1) Cursor_id = Bolt_interface_options;	  
                                else Cursor_id--; 
                                HIM_Cursor(Bolt_Interface, Cursor_id);              
                                break;
                            case KEY3_PRES:                     //ȷ�ϼ�
                                   switch(Cursor_id)
                                    {
                                        case 1:
                                            current_screen_id = S_Bolt_Diameter;           //��ת��˨�ȼ�����
                                            SetTextFlickerCycle(2,1,50); //�ı���˸ 
                                        break;
                                        case 2:
                                            current_screen_id = S_Bolt_Grade;              //��ת��˨�ȼ�����
                                            SetTextFlickerCycle(2,2,50); //�ı���˸
                                        break;
                                        case 3:
                                            current_screen_id = S_Bolt_TargetTorqueuint;   //��תĿ��Ť�ص�λ����
                                            SetTextFlickerCycle(2,4,50); //�ı���˸
                                        break;
                                        case 4:
                                            current_screen_id = S_Bolt_ActualPressureuint; //��תѹ����λ���ý���
                                            SetTextFlickerCycle(2,6,50); //�ı���˸
                                        break;
                                    }
                                break;	
                            case KEY4_Long_PRES:				           //�˵���                               
                                current_screen_id = Menu_Gear_Interface;  //�л��Ƕ�ģʽ����
                                Cursor_id = 0;
                            break;	                
                        }                    
            break;   
            case S_Bolt_Diameter:                               //��˨ֱ�����ý���
                        Bolt_Standard_display(Set_Chart_Standard_Type,Set_Bolt_Diameter);//��ʾ��˨ֱ��
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,BoltCoefficient_value);  //���Ŀ��Ť��
                        HIM_TargetTorque_display(2,3,Torque_val);//��ʾ��˨Ŀ��Ť�� 
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(2,5,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }          
                        switch(KEY_State)
                        {				                            
                            //����
                            case KEY1_PRES:                     //�Ϸ���
                                if(Set_Bolt_Diameter >= Bolt_DiameterNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1) Set_Bolt_Diameter=0;	   
                                else Set_Bolt_Diameter++;     
                            break;
                            case KEY2_PRES:                     //�·���
                                if(Set_Bolt_Diameter <= 0) Set_Bolt_Diameter=Bolt_DiameterNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1;	   
                                else Set_Bolt_Diameter--;  
                            break;
                            case KEY2_Long_PRES:                     //�Ϸ���
                                if(Set_Bolt_Diameter >= Bolt_DiameterNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1) Set_Bolt_Diameter=0;	   
                                else Set_Bolt_Diameter++;     
                            break;
                            case KEY1_Long_PRES:                     //�·���
                                if(Set_Bolt_Diameter <= 0) Set_Bolt_Diameter=Bolt_DiameterNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1;	   
                                else Set_Bolt_Diameter--; 
                            break;
                            //��ť
                            case EKEY3_PRES:                     //�Ϸ���
                                if(Set_Bolt_Diameter >= Bolt_DiameterNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1) Set_Bolt_Diameter=0;	   
                                else Set_Bolt_Diameter++;    
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_Bolt_Diameter <= 0) Set_Bolt_Diameter=Bolt_DiameterNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1;	   
                                else Set_Bolt_Diameter--; 
                            break;
                            case EKEY2_PRES:                     //EC11 �Ϸ���
                                if(Set_Bolt_Diameter >= Bolt_DiameterNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1) Set_Bolt_Diameter=0;	   
                                else Set_Bolt_Diameter++;      
                            break;
                            case EKEY1_PRES:                     //EC11  �·���
                                if(Set_Bolt_Diameter <= 0) Set_Bolt_Diameter=Bolt_DiameterNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1;	   
                                else Set_Bolt_Diameter--;  
                            break;
                            case KEY3_PRES:                     //ȷ�ϼ� 
                                SetTextFlickerCycle(2,1,0); //�ı�ֹͣ��˸                                                          
                                current_screen_id = Bolt_Interface;   //������ĸģʽ����
                                break;	
                        }                         
            break; 
            case S_Bolt_Grade:                               //��˨�ȼ����ý���
                        Bolt_Grade_display(Set_Chart_Standard_Type,Set_Bolt_Garder);//��ʾ��˨�ȼ�
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,BoltCoefficient_value);  //���Ŀ��Ť��
                        HIM_TargetTorque_display(2,3,Torque_val);//��ʾ��˨Ŀ��Ť�� 
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(2,5,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                            //����
                            case KEY1_PRES:                     //�Ϸ���
                                if(Set_Bolt_Garder >= Bolt_GradeNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1) Set_Bolt_Garder=0;	   //��ΧΪ1-4
                                else Set_Bolt_Garder++;     
                            break;
                            case KEY2_PRES:                     //�·���
                                if(Set_Bolt_Garder <= 0) Set_Bolt_Garder=Bolt_GradeNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1;	   //��ΧΪ1-4
                                else Set_Bolt_Garder--;   
                            break;
                            case KEY2_Long_PRES:                     //�Ϸ���
                                if(Set_Bolt_Garder >= Bolt_GradeNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1) Set_Bolt_Garder=0;	   //��ΧΪ1-4
                                else Set_Bolt_Garder++;      
                            break;
                            case KEY1_Long_PRES:                     //�·���
                                if(Set_Bolt_Garder <= 0) Set_Bolt_Garder=Bolt_GradeNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1;	   //��ΧΪ1-4
                                else Set_Bolt_Garder--;  
                            break;
                            //��ť
                            case EKEY3_PRES:                     //�Ϸ���
                                if(Set_Bolt_Garder >= Bolt_GradeNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1) Set_Bolt_Garder=0;	   //��ΧΪ1-4
                                else Set_Bolt_Garder++;    
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_Bolt_Garder <= 0) Set_Bolt_Garder=Bolt_GradeNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1;	   //��ΧΪ1-4
                                else Set_Bolt_Garder--;  
                            break;
                            case EKEY2_PRES:                     //EC11 �Ϸ���
                                if(Set_Bolt_Garder >= Bolt_GradeNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1) Set_Bolt_Garder=0;	   //��ΧΪ1-4
                                else Set_Bolt_Garder++;       
                            break;
                            case EKEY1_PRES:                     //EC11  �·���
                                if(Set_Bolt_Garder <= 0) Set_Bolt_Garder=Bolt_GradeNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1;	   //��ΧΪ1-4
                                else Set_Bolt_Garder--;   
                            break;
                            case KEY3_PRES:                     //ȷ�ϼ�
                                SetTextFlickerCycle(2,2,0); //�ı�ֹͣ��˸                                           
                                current_screen_id = Bolt_Interface;    //������ĸģʽ����
                                break;	
                        }                       
            break;   
            case S_Bolt_TargetTorqueuint:                                               //Ŀ��Ť�ص�λ����
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,BoltCoefficient_value);  //���Ŀ��Ť��
                        HIM_TargetTorque_display(2,3,Torque_val);//��ʾ��˨Ŀ��Ť�� 
                        HIM_TargetTorqueuint_display(2,4,Set_Torque_Unit);  //��ʾĿ��Ť�ص�λ
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(2,5,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                            //����
                            case KEY1_PRES:                     //�Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;    
                            break;
                            case KEY2_PRES:                     //�·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY2_Long_PRES:                     //�Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case KEY1_Long_PRES:                     //�·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            //��ť
                            case EKEY3_PRES:                     //�Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--; 
                            break;
                            case EKEY2_PRES:                     //EC11 �Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY1_PRES:                     //EC11  �·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY3_PRES:                     //ȷ�ϼ�                       
                                SetTextFlickerCycle(2,4,0); //�ı�ֹͣ��˸
                                current_screen_id = Bolt_Interface;    //������ĸģʽ����
                                break;	
                        }   
                     
            break;
            case S_Bolt_ActualPressureuint:                                               //ʵʱѹ����λ����
                        HIM_ActualPressureuint_display(2,6,Set_Pressure_Unit_Type);   //��ʾѹ����λ
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(2,5,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                             //����
                            case KEY1_PRES:                     //�Ϸ���
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;  
                            break;
                            case KEY2_PRES:                     //�·���
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--;   
                            break;
                            case KEY2_Long_PRES:                     //�Ϸ���
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;        
                            break;
                            case KEY1_Long_PRES:                     //�·���
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--; 
                            break;
                            //��ť
                            case EKEY3_PRES:                     //�Ϸ���
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;       
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--;   
                            break;
                            case EKEY2_PRES:                     //EC11 �Ϸ���
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;           
                            break;
                            case EKEY1_PRES:                     //EC11  �·���
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--;   
                            break;
                            case KEY3_PRES:                     //ȷ�ϼ�
                                SetTextFlickerCycle(2,6,0); //�ı�ֹͣ��˸                                 
                                current_screen_id = Bolt_Interface;      //������ĸģʽ����
                                break;
                        } 
            break;                                            
 /*--------------------------------------------�ǶȽ���----------------------------------------------------------------------------*/        
            case Menu_Angle_Interface:                                                     //�ǶȽ���
                        Set_Control_Mode = 4;  //�Ƕ�ģʽ
                        HIM_Standard_display(3,1,Set_ProductStandard_Type);   //��ʾ�ͺ�
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);  //���Ŀ��Ť��
                        HIM_TargetTorque_display(3,2,Torque_val);//��ʾĿ��Ť�� 
                        HIM_TargetTorqueuint_display(3,3,Set_Torque_Unit);  //��ʾĿ��Ť�ص�λ
                        SetTextInt32(3,4,Set_Target_Angle,0,0);  //��ʾĿ��Ƕ�
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(3,5,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        HIM_ActualPressureuint_display(3,6,Set_Pressure_Unit_Type);  //ѹ����λ��ʾ
                        HIM_Cursor(Menu_Angle_Interface, Cursor_id);                  
                        switch(KEY_State)
                        {				 
                            case KEY2_PRES:                     //�Ϸ���							
                                if(Cursor_id >= 5) Cursor_id = 1;	  
                                else Cursor_id++; 
                                break;
                            case KEY1_PRES:                     //�·���
                                if(Cursor_id <= 1) Cursor_id = 5;	   
                                else Cursor_id--;              
                                break;
                            case EKEY3_PRES:                     //�Ϸ���							
                                if(Cursor_id >= 5) Cursor_id = 1;	  
                                else Cursor_id++;                  
                                break;
                            case EKEY4_PRES:                     //�·���
                                if(Cursor_id <= 1) Cursor_id = 5;	   
                                else Cursor_id--;             
                                break;
                            case KEY3_PRES:                     //ȷ�ϼ�
                                    switch(Cursor_id)
                                    {
                                        case 1:
                                            current_screen_id = S_Angle_ProductType;     //��ת��Ӧ���ý���
                                            SetTextFlickerCycle(3,1,50); //�ı���˸ 
                                        break;
                                        case 2:
                                            current_screen_id = S_Angle_PresetTorque;   //��ת��Ӧ���ý���
                                            SetTextFlickerCycle(3,2,50); //�ı���˸ 
                                        break;
                                        case 3:
                                            current_screen_id = S_Angle_PresetTorqueuint;   //��ת��Ӧ���ý���
                                            SetTextFlickerCycle(3,3,50); //�ı���˸ 
                                        break;
                                        case 4:
                                            current_screen_id = S_Angle_TargetAngle;     //��ת��Ӧ���ý���
                                            SetTextFlickerCycle(3,4,50); //�ı���˸ 
                                        break;
                                        case 5:
                                            current_screen_id = S_Angle_ActualPressureuint;   //��ת��Ӧ���ý���
                                            SetTextFlickerCycle(3,6,50); //�ı���˸ 
                                        break;
                                    }
                                break;	
                            case KEY4_Long_PRES:				            //�˵���                               
                                current_screen_id = Menu_Sensor_Interface;        //�л���λģʽ����
                                Cursor_id = 0;   
                                break;	                
                        }                                           
            break;  
            case S_Angle_ProductType:                                                                              //��Ʒ�ͺ�����
                        HIM_Standard_display(3,1,Set_ProductStandard_Type);         //��ʾ��Ʒ�ͺ� 
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(3,5,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }           
                        switch(KEY_State)
                        {				                       
                             //����
                            case KEY1_PRES:                     //�Ϸ���
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //�ͺ�
                                else Set_ProductStandard_Type++;    
                            break;
                            case KEY2_PRES:                     //�·���
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;   
                            break;
                            case KEY2_Long_PRES:                     //�Ϸ���
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //�ͺ�
                                else Set_ProductStandard_Type++;    
                            break;
                            case KEY1_Long_PRES:                     //�·���
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;  
                            break;
                            //��ť
                            case EKEY3_PRES:                     //�Ϸ���
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //�ͺ�
                                else Set_ProductStandard_Type++;   
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;  
                            break;
                            case EKEY2_PRES:                     //EC11 �Ϸ���
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //�ͺ�
                                else Set_ProductStandard_Type++;    
                            break;
                            case EKEY1_PRES:                     //EC11  �·���
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;   
                            break;
                            case KEY3_PRES:                     //ȷ�ϼ�   
                                SetTextFlickerCycle(3,1,0); //�ı�ֹͣ��˸                                               
                                current_screen_id = Menu_Angle_Interface;     //���ز�Ʒ�ͺ�ѡ�����  
                            break;	
                        }                              
            break;
            case S_Angle_PresetTorque:                               //Ŀ��Ť�����ý���
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);  //���Ŀ��Ť��
                        HIM_TargetTorque_display(3,2,Torque_val);//��ʾĿ��Ť�� 
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(3,5,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				                                   
                            case KEY1_PRES:                     //�Ϸ���  
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //��ΧΪ0-���Ť��
                                else TargetTorque_value++;                                      
                                break;
                            case KEY2_PRES:                     //�·���                       
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //��ΧΪ0-���Ť��
                                else TargetTorque_value--;                                                           
                                break;
                            case KEY2_Long_PRES:                     //�Ͽ��ٷ���  
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //��ΧΪ0-���Ť��
                                else TargetTorque_value +=111 ;                                      
                                break;
                            case KEY1_Long_PRES:                     //�¿��ٷ���                       
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //��ΧΪ0-���Ť��
                                else TargetTorque_value -= 111;                                                           
                                break;
                            case EKEY3_PRES:                     //�Ϸ���
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //��ΧΪ0-���Ť��
                                else TargetTorque_value++;       
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //��ΧΪ0-���Ť��
                                else TargetTorque_value--; 
                            break;
                            case EKEY2_PRES:                     //��ť˳ʱ��
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //��ΧΪ0-���Ť��
                                else TargetTorque_value +=ROUND_TO_UINT32(ProductCoefficient_value/2);                                   
                                break;
                            case EKEY1_PRES:                     //��ť��ʱ��                      
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //��ΧΪ0-���Ť��
                                else TargetTorque_value -= ROUND_TO_UINT32(ProductCoefficient_value/2);                                                           
                                break;  
                            case KEY3_PRES:                     //ȷ�ϼ�  
                                SetTextFlickerCycle(3,2,0); //�ı�ֹͣ��˸  
                                current_screen_id = Menu_Angle_Interface;     //���ؽǶ�ģʽ����
                                break;	
                        }                        
            break;
            case S_Angle_PresetTorqueuint:                               //Ŀ��Ť�ص�λ���ý���
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);  //���Ŀ��Ť��
                        HIM_TargetTorque_display(3,2,Torque_val);//��ʾĿ��Ť�� 
                        HIM_TargetTorqueuint_display(3,3,Set_Torque_Unit);  //��ʾĿ��Ť�ص�λ
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(3,5,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                            //����
                            case KEY1_PRES:                     //�Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;    
                            break;
                            case KEY2_PRES:                     //�·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY2_Long_PRES:                     //�Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case KEY1_Long_PRES:                     //�·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            //��ť
                            case EKEY3_PRES:                     //�Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--; 
                            break;
                            case EKEY2_PRES:                     //EC11 �Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY1_PRES:                     //EC11  �·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY3_PRES:                     //ȷ�ϼ�  
                                SetTextFlickerCycle(3,3,0); //�ı�ֹͣ��˸  
                                current_screen_id = Menu_Angle_Interface;     //���ؽǶ�ģʽ����
                                break;	
                        }                        
            break;                    
            case S_Angle_TargetAngle:                               //Ŀ��Ƕ����ý���
                        SetTextInt32(3,4,Set_Target_Angle,0,0);
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(3,5,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				                           
                           case KEY1_PRES:                     //�Ϸ���  
                                if(Set_Target_Angle >= Max_Angle) Set_Target_Angle = 0;  
                                else Set_Target_Angle++;                                       
                                break;
                            case KEY2_PRES:                     //�·���                       
                                if(Set_Target_Angle <= 0) Set_Target_Angle = Max_Angle;  
                                else Set_Target_Angle--;                                                           
                                break;
                            case KEY2_Long_PRES:                     //�Ͽ��ٷ���  
                                if(Set_Target_Angle >= Max_Angle) Set_Target_Angle = 0;  
                                else Set_Target_Angle+= 11;                                     
                                break;
                            case KEY1_Long_PRES:                     //�¿��ٷ���                       
                                if(Set_Target_Angle <= 0) Set_Target_Angle = Max_Angle;  
                                else Set_Target_Angle-= 11;                                                            
                                break;
                            case EKEY3_PRES:                     //�Ϸ���
                                if(Set_Target_Angle >= Max_Angle) Set_Target_Angle = 0;  
                                else Set_Target_Angle++;        
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_Target_Angle <= 0) Set_Target_Angle = Max_Angle;  
                                else Set_Target_Angle--; 
                            break;
                            case EKEY2_PRES:                     //��ť˳ʱ��
                                if(Set_Target_Angle >= Max_Angle) Set_Target_Angle = 0;  
                                else Set_Target_Angle+= 11;                                   
                                break;
                            case EKEY1_PRES:                     //��ť��ʱ��                      
                                if(Set_Target_Angle <= 0) Set_Target_Angle = Max_Angle;  
                                else Set_Target_Angle-= 11;                                                         
                                break;  
                            case KEY3_PRES:                     //ȷ�ϼ�  
                                SetTextFlickerCycle(3,4,0); //�ı�ֹͣ��˸  
                                current_screen_id = Menu_Angle_Interface;     //���ؽǶ�ģʽ����
                                break;	
                        }                        
            break;                        
            case S_Angle_ActualPressureuint:                                       //ѹ����λ����
                        HIM_ActualPressureuint_display(3,6,Set_Pressure_Unit_Type);  //ѹ����λ��ʾ
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(3,5,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				                         
                              //����
                            case KEY1_PRES:                     //�Ϸ���
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;  
                            break;
                            case KEY2_PRES:                     //�·���
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--;   
                            break;
                            case KEY2_Long_PRES:                     //�Ϸ���
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;        
                            break;
                            case KEY1_Long_PRES:                     //�·���
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--; 
                            break;
                            //��ť
                            case EKEY3_PRES:                     //�Ϸ���
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;       
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--;   
                            break;
                            case EKEY2_PRES:                     //EC11 �Ϸ���
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;           
                            break;
                            case EKEY1_PRES:                     //EC11  �·���
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--;   
                            break;
                            case KEY3_PRES:                     //ȷ�ϼ�
                                SetTextFlickerCycle(3,6,0); //�ı�ֹͣ��˸                                 
                                current_screen_id = Menu_Angle_Interface;     //�������ý���
                                break;
                        }                      
            break;
                        
/*--------------------------------------------��λ����----------------------------------------------------------------------------*/              
            case Menu_Gear_Interface:                                                //��λ����
                        Set_Control_Mode = 3;  //��λģʽ
                        HIM_Standard_display(4,1,Set_ProductStandard_Type);         //��ʾ��Ʒ�ͺ� 
                        SetTextInt32(4,2,Gear_level,0,0);  //��ʾĿ�굵λ 
//                        printf( "GearTorque_value:%d\r\n",GearTorque_value );
//                        printf( "ProductCoefficient_value:%f\r\n",ProductCoefficient_value );
//                        printf( "Set_Target_Gear:%d\r\n",Set_Target_Gear );          
                        HIM_GearTorqueValue(4,3,Gear_val);//��ʾ��λŤ��
                        HIM_TargetTorqueuint_display(4,4,Set_Torque_Unit);  //��ʾŤ�ص�λ
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(4,5,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        HIM_ActualPressureuint_display(4,6,Set_Pressure_Unit_Type);  //ѹ����λ��ʾ
                        HIM_Cursor(Menu_Gear_Interface, Cursor_id);                   
                        switch(KEY_State)
                        {				 
                            case KEY2_PRES:                     //�Ϸ���							
                                if(Cursor_id >= 4) Cursor_id = 1;	  
                                else Cursor_id++; 	                  
                                break;
                            case KEY1_PRES:                     //�·���
                                if(Cursor_id <= 1) Cursor_id = 4;	  
                                else Cursor_id--;             
                                break;
                            case EKEY3_PRES:                     //�Ϸ���							
                                if(Cursor_id >= 4) Cursor_id = 1;	  
                                else Cursor_id++;                    
                                break;
                            case EKEY4_PRES:                     //�·���
                                if(Cursor_id <= 1) Cursor_id = 4;	  
                                else Cursor_id--;                
                                break;
                            case KEY3_PRES:                     //ȷ�ϼ�
                                    switch(Cursor_id)
                                    {
                                        case 1:
                                            current_screen_id = S_Gear_ProductType;      //��Ʒ�ͺ�����
                                            SetTextFlickerCycle(4,1,50); //�ı���˸ 
                                        break;
                                        case 2:
                                            current_screen_id = S_Gear_TargetGear;       //��ת��Ӧ���ý���
                                            SetTextFlickerCycle(4,2,50); //�ı���˸ 
                                        break;
                                        case 3:
                                            current_screen_id = S_Gear_TargetTorqueuint;     //��ת��Ӧ���ý���
                                            SetTextFlickerCycle(4,4,50); //�ı���˸ 
                                        break;
                                        case 4:
                                            current_screen_id = S_Gear_ActualPressureuint;     //��ת��Ӧ���ý���
                                            SetTextFlickerCycle(4,6,50); //�ı���˸ 
                                        break;
                                    }
                                break;	
                            case KEY4_Long_PRES:				            //�˵���                               
                                current_screen_id = Menu_Angle_Interface;     //�л�������ģʽ����
                                Cursor_id = 0;
                                break;	                
                        }                     
            break;
            case S_Gear_ProductType:                                                                              //��Ʒ�ͺ�����
                        HIM_Standard_display(4,1,Set_ProductStandard_Type);         //��ʾ��Ʒ�ͺ�  
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(4,5,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }           
                        switch(KEY_State)
                        {				 
                             //����
                            case KEY1_PRES:                     //�Ϸ���
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //�ͺ�
                                else Set_ProductStandard_Type++;    
                            break;
                            case KEY2_PRES:                     //�·���
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;   
                            break;
                            case KEY2_Long_PRES:                     //�Ϸ���
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //�ͺ�
                                else Set_ProductStandard_Type++;    
                            break;
                            case KEY1_Long_PRES:                     //�·���
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;  
                            break;
                            //��ť
                            case EKEY3_PRES:                     //�Ϸ���
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //�ͺ�
                                else Set_ProductStandard_Type++;   
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;  
                            break;
                            case EKEY2_PRES:                     //EC11 �Ϸ���
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //�ͺ�
                                else Set_ProductStandard_Type++;    
                            break;
                            case EKEY1_PRES:                     //EC11  �·���
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;   
                            break;
                            case KEY3_PRES:                     //ȷ�ϼ�   
                                    SetTextFlickerCycle(4,1,0); //�ı�ֹͣ��˸                                               
                                    current_screen_id = Menu_Gear_Interface;     //���ز�Ʒ�ͺ�ѡ�����  
                                break;	
                        }                              
            break; 
            case S_Gear_TargetGear:                               //Ŀ�굵λ���ý���
                        SetTextInt32(4,2,Gear_level,0,0);  //��ʾĿ�굵λ
                        HIM_GearTorqueValue(4,3,Gear_val);//��ʾ��λŤ��
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(4,5,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                             //����
                            case KEY1_PRES:                     //�Ϸ���
                                if(Set_Target_Gear >= Gear_length -1 ) Set_Target_Gear=0;	  
                                else Set_Target_Gear++;  
                            break;
                            case KEY2_PRES:                     //�·���
                                if(Set_Target_Gear <= 0) Set_Target_Gear=Gear_length -1 ;	   
                                else Set_Target_Gear--;  
                            break;
                            case KEY2_Long_PRES:                     //�Ϸ���
                                if(Set_Target_Gear >= Gear_length -1 ) Set_Target_Gear=0;	  
                                else Set_Target_Gear++;  
                            break;
                            case KEY1_Long_PRES:                     //�·���
                                if(Set_Target_Gear <= 0) Set_Target_Gear=Gear_length -1 ;	   
                                else Set_Target_Gear--;  
                            break;
                            //��ť
                            case EKEY3_PRES:                     //�Ϸ���
                                if(Set_Target_Gear >= Gear_length -1 ) Set_Target_Gear=0;	  
                                else Set_Target_Gear++;   
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_Target_Gear <= 0) Set_Target_Gear=Gear_length -1 ;	   
                                else Set_Target_Gear--;  
                            break;
                            case EKEY2_PRES:                     //EC11 �Ϸ���
                                if(Set_Target_Gear >= Gear_length -1 ) Set_Target_Gear=0;	  
                                else Set_Target_Gear++;    
                            break;
                            case EKEY1_PRES:                     //EC11  �·���
                                if(Set_Target_Gear <= 0) Set_Target_Gear=Gear_length -1 ;	   
                                else Set_Target_Gear--;   
                            break;
                            case KEY3_PRES:                     //ȷ�ϼ�                 
                               SetTextFlickerCycle(4,2,0); //�ı�ֹͣ��˸  
                               current_screen_id = Menu_Gear_Interface;    //���ص�λģʽ����
                                break;	
                        }   
                     
            break;
            case S_Gear_TargetTorqueuint:                               //Ŀ��Ť�ص�λ���ý���
                        HIM_GearTorqueValue(4,3,Gear_val);//��ʾ��λŤ��
                        HIM_TargetTorqueuint_display(4,4,Set_Torque_Unit);  //��ʾŤ�ص�λ
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(4,5,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                             //����
                            case KEY1_PRES:                     //�Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;    
                            break;
                            case KEY2_PRES:                     //�·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY2_Long_PRES:                     //�Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case KEY1_Long_PRES:                     //�·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            //��ť
                            case EKEY3_PRES:                     //�Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--; 
                            break;
                            case EKEY2_PRES:                     //EC11 �Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY1_PRES:                     //EC11  �·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY3_PRES:                     //ȷ�ϼ�    
                                SetTextFlickerCycle(4,4,0); //�ı�ֹͣ��˸                                 
                                current_screen_id = Menu_Gear_Interface;     //�������ý���
                                break;	
                        }                          
            break;
            case S_Gear_ActualPressureuint:                                       //ѹ����λ����
                        HIM_ActualPressureuint_display(4,6,Set_Pressure_Unit_Type);  //ѹ����λ��ʾ
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(4,5,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                            //����
                            case KEY1_PRES:                     //�Ϸ���
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   
                                else Set_Pressure_Unit_Type++;   
                            break;
                            case KEY2_PRES:                     //�·���
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   
                                else Set_Pressure_Unit_Type--;  
                            break;
                            case KEY2_Long_PRES:                     //�Ϸ���
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   
                                else Set_Pressure_Unit_Type++;     
                            break;
                            case KEY1_Long_PRES:                     //�·���
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   
                                else Set_Pressure_Unit_Type--;  
                            break;
                            //��ť
                            case EKEY3_PRES:                     //�Ϸ���
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   
                                else Set_Pressure_Unit_Type++;     
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   
                                else Set_Pressure_Unit_Type--;  
                            break;
                            case EKEY2_PRES:                     //EC11 �Ϸ���
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   
                                else Set_Pressure_Unit_Type++;      
                            break;
                            case EKEY1_PRES:                     //EC11  �·���
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   
                                else Set_Pressure_Unit_Type--;     
                            break;
                            case KEY3_PRES:                     //ȷ�ϼ�
                                SetTextFlickerCycle(4,6,0); //�ı�ֹͣ��˸                                 
                                current_screen_id = Menu_Gear_Interface;     //�������ý���
                                break;
                        }                      
            break;
/*--------------------------------------------����������----------------------------------------------------------------------------*/              
            case Menu_Sensor_Interface:                                                
                        Set_Control_Mode = 5;  //������ģʽ
                        HIM_Standard_display(5,1,Set_ProductStandard_Type);         //��ʾ��Ʒ�ͺ�
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,Set_SensorTorque);  //���Ԥ��Ť��
                        HIM_TargetTorque_display(5,2,Torque_val);//��ʾԤ��Ť��
                        HIM_TargetTorqueuint_display(5,3,Set_Torque_Unit);  //��ʾԤ��Ť�ص�λ
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);  //���Ŀ��Ť��
                        HIM_TargetTorque_display(5,4,Torque_val);//��ʾĿ��Ť��
                        HIM_TargetTorqueuint_display(5,5,Set_Torque_Unit);  //��ʾĿ��Ť�ص�λ                       
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(5,6,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        HIM_ActualPressureuint_display(5,7,Set_Pressure_Unit_Type);  //ѹ����λ��ʾ
                        HIM_Cursor(Menu_Sensor_Interface, Cursor_id);                   
                        switch(KEY_State)
                        {				 
                            case KEY2_PRES:                     //�Ϸ���							
                                if(Cursor_id >= 6) Cursor_id = 1;	   
                                else Cursor_id++; 		                  
                                break;
                            case KEY1_PRES:                     //�·���
                                if(Cursor_id <= 1) Cursor_id = 6;	  
                                else Cursor_id--;             
                                break;
                            case EKEY3_PRES:                     //�Ϸ���							
                                if(Cursor_id >= 6) Cursor_id = 1;	   
                                else Cursor_id++;                     
                                break;
                            case EKEY4_PRES:                     //�·���
                                if(Cursor_id <= 1) Cursor_id = 6;	  
                                else Cursor_id--;                 
                                break;
                            case KEY3_PRES:                     //ȷ�ϼ�
                                    switch(Cursor_id)
                                    {
                                        case 1:
                                            current_screen_id = S_Sensor_ProductType; //��ת��Ӧ���ý���
                                            SetTextFlickerCycle(5,1,50); //�ı���˸ 
                                        break;
                                        case 2:
                                            current_screen_id = S_Sensor_PresetTorque;       //��ת��Ӧ���ý���
                                            SetTextFlickerCycle(5,2,50); //�ı���˸ 
                                        break;
                                        case 3:
                                            current_screen_id = S_Sensor_PresetTorqueuint;     //��ת��Ӧ���ý���
                                            SetTextFlickerCycle(5,3,50); //�ı���˸ 
                                        break;
                                        case 4:
                                            current_screen_id = S_Sensor_TargetTorque;     //��ת��Ӧ���ý���
                                            SetTextFlickerCycle(5,4,50); //�ı���˸ 
                                        break;
                                        case 5:
                                            current_screen_id = S_Sensor_TargetTorqueuint;     //��ת��Ӧ���ý���
                                            SetTextFlickerCycle(5,5,50); //�ı���˸ 
                                        break;
                                        case 6:
                                            current_screen_id = S_Sensor_ActualPressureuint;     //��ת��Ӧ���ý���
                                            SetTextFlickerCycle(5,7,50); //�ı���˸ 
                                        break;
                                    }
                                break;	
                            case KEY4_Long_PRES:				            //�˵���                               
                                current_screen_id = Menu_Main_Interface;     //�л��˵�ģʽ����
                                Cursor_id = 0;
                                break;	                
                        }                     
            break;
            case S_Sensor_ProductType:                                                                              //��Ʒ�ͺ�����
                        HIM_Standard_display(5,1,Set_ProductStandard_Type);         //��ʾ��Ʒ�ͺ�  
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(5,6,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                             //����
                            case KEY1_PRES:                     //�Ϸ���
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //�ͺ�
                                else Set_ProductStandard_Type++;    
                            break;
                            case KEY2_PRES:                     //�·���
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;   
                            break;
                            case KEY2_Long_PRES:                     //�Ϸ���
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //�ͺ�
                                else Set_ProductStandard_Type++;    
                            break;
                            case KEY1_Long_PRES:                     //�·���
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;  
                            break;
                            //��ť
                            case EKEY3_PRES:                     //�Ϸ���
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //�ͺ�
                                else Set_ProductStandard_Type++;   
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;  
                            break;
                            case EKEY2_PRES:                     //EC11 �Ϸ���
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //�ͺ�
                                else Set_ProductStandard_Type++;    
                            break;
                            case EKEY1_PRES:                     //EC11  �·���
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;   
                            break;
                            case KEY3_PRES:                     //ȷ�ϼ�   
                                    SetTextFlickerCycle(5,1,0); //�ı�ֹͣ��˸                                               
                                    current_screen_id = Menu_Sensor_Interface;     //���ز�Ʒ�ͺ�ѡ����� 
                                break;	
                        }                              
            break;
            case S_Sensor_PresetTorque:                               //Ԥ��Ť�ز�������
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,Set_SensorTorque);  //���Ԥ��Ť��
                        HIM_TargetTorque_display(5,2,Torque_val);//��ʾԤ��Ť��
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(5,6,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				                      
                             case KEY1_PRES:                     //�Ϸ���  
                                if(Set_SensorTorque >= Spanner_Sensor_parameter(Set_ProductStandard_Type)) Set_SensorTorque=0;	   //��ΧΪ0-���Ť��
                                else Set_SensorTorque++;                                     
                                break;
                            case KEY2_PRES:                     //�·���                       
                                if(Set_SensorTorque <= 0) Set_SensorTorque = Spanner_Sensor_parameter(Set_ProductStandard_Type);	   //��ΧΪ0-���Ť��
                                else Set_SensorTorque--;                                                           
                                break;
                            case KEY2_Long_PRES:                     //�Ͽ��ٷ���  
                                if(Set_SensorTorque >= Spanner_Sensor_parameter(Set_ProductStandard_Type)) Set_SensorTorque=0;	   //��ΧΪ0-���Ť��
                                else Set_SensorTorque+= 111;                                      
                                break;
                            case KEY1_Long_PRES:                     //�¿��ٷ���                       
                                if(Set_SensorTorque <= 0) Set_SensorTorque = Spanner_Sensor_parameter(Set_ProductStandard_Type);	   //��ΧΪ0-���Ť��
                                else Set_SensorTorque-= 111;                                                            
                                break;
                            case EKEY3_PRES:                     //�Ϸ���
                                if(Set_SensorTorque >= Spanner_Sensor_parameter(Set_ProductStandard_Type)) Set_SensorTorque=0;	   //��ΧΪ0-���Ť��
                                else Set_SensorTorque++;      
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_SensorTorque <= 0) Set_SensorTorque = Spanner_Sensor_parameter(Set_ProductStandard_Type);	   //��ΧΪ0-���Ť��
                                else Set_SensorTorque--; 
                            break;
                            case EKEY2_PRES:                     //��ť˳ʱ��
                                if(Set_SensorTorque >= Spanner_Sensor_parameter(Set_ProductStandard_Type)) Set_SensorTorque=0;	   //��ΧΪ0-���Ť��
                                else Set_SensorTorque+= ROUND_TO_UINT32(ProductCoefficient_value/2);                                     
                                break;
                            case EKEY1_PRES:                     //��ť��ʱ��                      
                                if(Set_SensorTorque <= 0) Set_SensorTorque = Spanner_Sensor_parameter(Set_ProductStandard_Type);	   //��ΧΪ0-���Ť��
                                else Set_SensorTorque-= ROUND_TO_UINT32(ProductCoefficient_value/2);                                                           
                                break;
                            case KEY3_PRES:                     //ȷ�ϼ�   
                                SetTextFlickerCycle(5,2,0); //�ı�ֹͣ��˸                                  
                                current_screen_id = Menu_Sensor_Interface;    //�������ý���
                                break;	
                        }     
            break;
            case S_Sensor_PresetTorqueuint:                               //Ԥ��Ť�ص�λ����
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,Set_SensorTorque);  //���Ԥ��Ť��
                        HIM_TargetTorque_display(5,2,Torque_val);//��ʾԤ��Ť��
                        HIM_TargetTorqueuint_display(5,3,Set_Torque_Unit);  //��ʾԤ��Ť�ص�λ
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(5,6,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                             //����
                            case KEY1_PRES:                     //�Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;    
                            break;
                            case KEY2_PRES:                     //�·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY2_Long_PRES:                     //�Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case KEY1_Long_PRES:                     //�·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            //��ť
                            case EKEY3_PRES:                     //�Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--; 
                            break;
                            case EKEY2_PRES:                     //EC11 �Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY1_PRES:                     //EC11  �·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY3_PRES:                     //ȷ�ϼ�    
                                SetTextFlickerCycle(5,3,0); //�ı�ֹͣ��˸                                 
                                current_screen_id = Menu_Sensor_Interface;     //�������ý���
                                break;	
                        }                          
            break;
            case S_Sensor_TargetTorque:                               //Ŀ��Ť�ز������ý���
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);  //���Ŀ��Ť��
                        HIM_TargetTorque_display(5,4,Torque_val);//��ʾĿ��Ť��
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(5,6,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                             case KEY1_PRES:                     //�Ϸ���  
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //��ΧΪ0-���Ť��
                                else TargetTorque_value++;                                      
                                break;
                            case KEY2_PRES:                     //�·���                       
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //��ΧΪ0-���Ť��
                                else TargetTorque_value--;                                                           
                                break;
                            case KEY2_Long_PRES:                     //�Ͽ��ٷ���  
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //��ΧΪ0-���Ť��
                                else TargetTorque_value +=111 ;                                      
                                break;
                            case KEY1_Long_PRES:                     //�¿��ٷ���                       
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //��ΧΪ0-���Ť��
                                else TargetTorque_value -= 111;                                                           
                                break;
                            case EKEY3_PRES:                     //�Ϸ���
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //��ΧΪ0-���Ť��
                                else TargetTorque_value++;       
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //��ΧΪ0-���Ť��
                                else TargetTorque_value--; 
                            break;
                            case EKEY2_PRES:                     //��ť˳ʱ��
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //��ΧΪ0-���Ť��
                                else TargetTorque_value +=ROUND_TO_UINT32(ProductCoefficient_value/2);                                   
                                break;
                            case EKEY1_PRES:                     //��ť��ʱ��                      
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //��ΧΪ0-���Ť��
                                else TargetTorque_value -= ROUND_TO_UINT32(ProductCoefficient_value/2);                                                           
                                break;
                            case KEY3_PRES:                     //ȷ�ϼ�   
                                SetTextFlickerCycle(5,4,0); //�ı�ֹͣ��˸                                  
                                current_screen_id = Menu_Sensor_Interface;    //�������ý���
                                break;	
                        }     
            break;
            case S_Sensor_TargetTorqueuint:                               //Ŀ��Ť�ص�λ���ý���
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);  //���Ŀ��Ť��
                        HIM_TargetTorque_display(5,4,Torque_val);//��ʾĿ��Ť��
                        HIM_TargetTorqueuint_display(5,5,Set_Torque_Unit);  //��ʾĿ��Ť�ص�λ
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(5,6,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                              //����
                            case KEY1_PRES:                     //�Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;    
                            break;
                            case KEY2_PRES:                     //�·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY2_Long_PRES:                     //�Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case KEY1_Long_PRES:                     //�·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            //��ť
                            case EKEY3_PRES:                     //�Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--; 
                            break;
                            case EKEY2_PRES:                     //EC11 �Ϸ���
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY1_PRES:                     //EC11  �·���
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY3_PRES:                     //ȷ�ϼ�    
                                SetTextFlickerCycle(5,5,0); //�ı�ֹͣ��˸                                 
                                current_screen_id = Menu_Sensor_Interface;     //�������ý���
                                break;	
                        }                          
            break;
            case S_Sensor_ActualPressureuint:                                       //ѹ����λ����
                        HIM_ActualPressureuint_display(5,7,Set_Pressure_Unit_Type);  //ѹ����λ��ʾ
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(5,6,Pressure_Value,Set_Pressure_Unit_Type);//��ʾѹ��ֵ
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                              //����
                            case KEY1_PRES:                     //�Ϸ���
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;  
                            break;
                            case KEY2_PRES:                     //�·���
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--;   
                            break;
                            case KEY2_Long_PRES:                     //�Ϸ���
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;        
                            break;
                            case KEY1_Long_PRES:                     //�·���
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--; 
                            break;
                            //��ť
                            case EKEY3_PRES:                     //�Ϸ���
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;       
                            break;
                            case EKEY4_PRES:                     //�·���
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--;   
                            break;
                            case EKEY2_PRES:                     //EC11 �Ϸ���
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;           
                            break;
                            case EKEY1_PRES:                     //EC11  �·���
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--;   
                            break;
                            case KEY3_PRES:                     //ȷ�ϼ�
                                SetTextFlickerCycle(5,7,0); //�ı�ֹͣ��˸                                 
                                current_screen_id = Menu_Sensor_Interface;     //�������ý���
                                break;
                        }                      
            break;                        

        }       
	}
}

