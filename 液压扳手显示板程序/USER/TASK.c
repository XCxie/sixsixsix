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

char  Cursor_id = 0;       //光标编号
char Set_Language_Type = 0;       //设置语言类型   
char Set_Chart_Standard_Type = 0; //设置图表标准类型
char Set_ProductStandard_Type = 0;//设置产品规格类型
char Set_Bolt_Diameter = 0;       //设置螺栓直径
char Set_Bolt_Garder = 0;         //设置螺栓等级
char Set_Torque_Unit = 0;   //设置扭矩单位类型
char Set_Pressure_Unit_Type = 0;  //设置压力单位类型 

int TargetTorque_value = 0;     //目标扭矩值
short int Set_Target_Angle = 0;       //设置预设角度值
char Set_Target_Gear = 0;         //设置目标档位
int GearTorque_value = 0;//档位扭矩

int Spanner_Coefficien = 0;  //扳手保存系数
int BoltCoefficient_value = 0;//螺栓扭矩
int Set_SensorTorque = 0;     //设置传感器预设扭矩值
u32 Set_Target_Usage = 0;     //设置限制使用的次数
char  Set_Control_Mode = 0;       //设置控制模式
u32 UsageCounter = 0;  //使用次数
float Pressure_Value = 0; //压力值
u16 Angle_Value  = 0; //实时角度值
u16 Max_Angle = 360; //最大限制角度

//SYS任务
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
        //看门狗监测任务        
		 if(!(--ucCount200ms))
			 {
				 ucCount200ms = 2;
			   if(NeedToResT==0)//如果需要复位，将不再进行喂狗，等待看门狗进行复位
			    {	
				   IWDG_Feed();
			    }else
                {
                    printf("程序复位\r\n");	
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
		if(FreeDogStructVar.FlashTaskDog>60)//这里计时，
			{
			  NeedToResT=1;
                	printf("FLASH \r\n");
			}
		if(FreeDogStructVar.HiMTaskDog>5)//这里计时，
			{
			  NeedToResT=1;
                	printf("HIM\r\n");
			}
			
		if(FreeDogStructVar.TimeTaskDog>5)//这里计时，
			{
			  NeedToResT=1;
                printf("TIME\r\n");
			}
		if(FreeDogStructVar.UdiskTaskDog>60)//这里计时，
			{
			  NeedToResT=1;
                	printf("UDISK\r\n");
			}
   			
		if(FreeDogStructVar.UsartTaskDog>5)//这里计时，
			{
			  NeedToResT=1;
                	printf("USART\r\n");
			}
						
		}
	
	}
}
//驱动板接收发送任务
void UsartDriverRev_task(void *pdata)
{	
    pdata = pdata;
	while(1)
	{	
        OSTimeDly(2);
        FreeDogStructVar.UsartTaskDog = 0;
        UartDriver();   //解析指令   
	};
}

u8 AutoDatabuff[4][120] ={0};
u8 AutoNumber = 0;
//存储任务
void Flash_task(void *pdata)
{	   
    static u8 RecordFlag = 0;  //记录数据标志位（1：开始记录，0：未记录）
    static u8 RecordFlag1 = 0;  //记录数据标志位（1：开始记录，0：未记录）
    static u8 Start_StorageFlag = 0;
    pdata = pdata;
	while(1)
	{
        OSTimeDly(2);
        FreeDogStructVar.FlashTaskDog = 0;
        Flash_Model_parameters(&Data_Storage);    //存储参数	
        if(DriveControl_status == 1 )
        {
            
            if(Set_Control_Mode ==4)  //当为角度模式时
            {
                if(RecordCount == 0)   
                {
                    Data_Record_Storage(Databuff,RecorddataCount,sizeof(Databuff));	  //记录使用数据
                    RecordFlag1 = 1;
                }
                else if(RecordCount ==2 &&RecordFlag1 ==1)
                {    
                     
                    RecorddataCount ++;       //数据记录次数加1 
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
                    Data_Record_Storage(AutoDatabuff[AutoNumber],RecorddataCount,sizeof(Databuff));	  //记录使用数据
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
                        RecorddataCount  +=  AutoNumber ;    //数据记录次数 
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
                Data_Record_Storage(Databuff,RecorddataCount,sizeof(Databuff));	  //记录使用数据
                RecordFlag1 = 1;
            }
            else if(RecordCount ==1 &&RecordFlag1 ==1)
            {    
                 
                RecorddataCount ++;       //数据记录次数加1                
                Data_Storage = 9;                 
                RecordFlag1 = 0;
            }
            
        }
        
	}
}


//U盘读写文件任务
void Udisk_task(void *pdata)
{	 
    u8 i;   
    pdata = pdata;
	while(1)
	{
        OSTimeDly(10);       
		while ( CH374DiskStatus < DISK_CONNECT ) {            /* 查询CH375中断并更新中断状态,等待U盘插入 */
            FreeDogStructVar.UdiskTaskDog = 0;
//            printf( "ON\n" );  
            OSTimeDly(10);
			if ( CH374DiskConnect( ) == ERR_SUCCESS ) break;  /* 有设备连接则返回成功,CH375DiskConnect同时会更新全局变量CH375DiskStatus */
			
		}
		
//		printf("Disk Init\n");
		for ( i = 0; i < 5; i ++ ) {                          /* 有的U盘总是返回未准备好,不过可以被忽略 */
            OSTimeDly(5);
//			printf( "Ready ?\n" );
			if ( CH374DiskReady( ) == ERR_SUCCESS ) break;    /* 查询磁盘是否准备好 */
		}
		
/* 查询磁盘物理容量 */
//		printf( "DiskSize\n" );
		i = CH374DiskSize( );  
		mStopIfError( i );
//		printf( "TotalSize = %u MB \n", (unsigned int)( mCmdParam.DiskSize.mDiskSizeSec * (CH374vSectorSize/512) / 2048 ) );  // 显示为以MB为单位的容量
		// 原计算方法 (unsigned int)( mCmdParam.DiskSize.mDiskSizeSec * CH374vSectorSize / 1000000 ) 有可能前两个数据相乘后导致溢出, 所以修改成上式
		SYS_LED_ON;
/* 产生新文件 */
//		printf( "Create\n" );                                 //串口创建时间会超长 
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
		
		i = CH374FileCreate( );                               /* 新建文件并打开,如果文件已经存在则先删除后再新建 */
		mStopIfError( i );

        WriteTail();   //写入数据
		
//		mCmdParam.ByteWrite.mByteCount = 5;                   /* 指定本次写入的字节数,单次读写的长度不能超过MAX_BYTE_IO */
//		i = CH374ByteWrite( );                                /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
//		mStopIfError( i );
        SYS_LED_OFF;
//		printf( "Close\n" );
		mCmdParam.Close.mUpdateLen = 1;                       /* 不要自动计算文件长度,如果自动计算,那么该长度总是CH374vSectorSize的倍数 */
		i = CH374FileClose( );
		mStopIfError( i );		
		
//		printf( "Take out\n" );
		while ( 1 ) { 
                               
            OSTimeDly(200); /* 没必要频繁查询 */
            FreeDogStructVar.UdiskTaskDog = 0;
			if ( CH374DiskConnect( ) != ERR_SUCCESS ) break;  /* 查询方式: 检查磁盘是否连接并更新磁盘状态,返回成功说明连接 */
		}
	};
}

//TIME任务
void Time_task(void *pdata)
{	  
    pdata = pdata;
	while(1)
	{
        OSTimeDly(2); 
        FreeDogStructVar.TimeTaskDog = 0;
        TypeC_UartDriver();        //与上位机通信解析函数 
        
	};
}

//串口屏任务
void HIM_task(void *pdata)
{	 
    float ProductCoefficient_value = 0;//扳手实际系数 
    u8 BoltCoefficient_type = 0;     //螺栓序号    
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
        HIM_Home_Switcher(current_screen_id);    //显示切换界面 
        Spanner_Coefficien =  Spanner_type_parameter(Set_ProductStandard_Type); //获取产品系数  
        ProductCoefficient_value = Spanner_Coefficien/10000.0;//计算实际系数值
        Get_Bolttorque_parameter(Set_ProductStandard_Type);
        BoltCoefficient_value = Select_Bolttorque_parameter(Set_Chart_Standard_Type,Set_Bolt_Garder,Set_Bolt_Diameter);//获得螺栓目标扭矩
        //档位参数
        Gear_level = Gears_data[Set_Target_Gear].Levels;  //档位等级
        Gear_parameter = Gears_data[Set_Target_Gear].parameter;//档位对应压力
        GearTorque_value = Get_GearTorqueValue(ProductCoefficient_value,Gear_parameter);//档位扭矩
        Gear_val = Get_GearTorque(Set_Torque_Unit,GearTorque_value);  //档位扭矩单位换算          
        Send_DriverParameter();   //向驱动板发送一次参数
        //判断按键状态        
        if(KEY_Type == 0)
        {
            KEY_State = KEY_Scan(1);  //机械按键
        }
        else if(KEY_Type ==1)
        {
            KEY_State =EC11_status;   //旋钮按键
            KEY_Type = 0;               
        }
        switch(current_screen_id)
        {
/*--------------------------------------------菜单界面----------------------------------------------------------------------------*/
            case Menu_Main_Interface:                                                                    //菜单界面 
                        Set_Control_Mode = 0;  //菜单模式
                        HIM_Language_display(Set_Language_Type);//显示当前语言
                        HIM_ChartStandard_display(Set_Chart_Standard_Type,Set_Language_Type);//显示当前图表标准         
                        DisplayTime();//刷新显示RTC时间
                        HIM_Cursor(Menu_Main_Interface, Cursor_id);//显示光标
                        SetTextInt32(0,9,UsageCounter,0,0);//显示使用次数
                        switch(KEY_State)
                        {				 
                            case KEY2_PRES:                     //按键上翻							
                                if(Cursor_id >= Menu_interface_options) Cursor_id = 1;	   
                                else Cursor_id++;                   
                                break;
                            case KEY1_PRES:                     //按键下翻
                                if(Cursor_id <= 1) Cursor_id = Menu_interface_options;	  
                                else Cursor_id--;              
                                break;
                            case EKEY3_PRES:                     //旋钮顺时针							
                                if(Cursor_id >= Menu_interface_options) Cursor_id = 1;	   
                                else Cursor_id++;                   
                                break;
                            case EKEY4_PRES:                     //旋钮逆时针
                                if(Cursor_id <= 1) Cursor_id = Menu_interface_options;	  
                                else Cursor_id--;              
                                break;
                            case KEY3_PRES:                     //确认键
                                    switch(Cursor_id)
                                    {
                                        case 1:
                                            current_screen_id = S_Main_language;        //跳转语言设置界面
                                            SetTextFlickerCycle(0,1,50); //文本闪烁
                                        break;
                                        case 2:
                                            current_screen_id = S_Main_ChartStandard;  //跳转图表标准设置界面
                                            SetTextFlickerCycle(0,2,50); //文本闪烁
                                        break;
                                        case 3:
                                            current_screen_id = S_Main_year;        //跳转日期设置界面
                                            SetTextFlickerCycle(0,3,50); //文本闪烁
                                        break;
                                        case 4:
                                            current_screen_id = S_Main_month;        //跳转日期设置界面
                                            SetTextFlickerCycle(0,4,50); //文本闪烁
                                        break;
                                        case 5:
                                            current_screen_id = S_Main_day;        //跳转日期设置界面
                                            SetTextFlickerCycle(0,5,50); //文本闪烁
                                        break;
                                        case 6:
                                            current_screen_id = S_Main_hour;        //跳转日期设置界面
                                            SetTextFlickerCycle(0,6,50); //文本闪烁
                                        break;
                                        case 7:
                                            current_screen_id = S_Main_minute;        //跳转日期设置界面
                                            SetTextFlickerCycle(0,7,50); //文本闪烁
                                        break;
                                        case 8:
                                            current_screen_id = S_Main_second;        //跳转日期设置界面
                                            SetTextFlickerCycle(0,8,50); //文本闪烁
                                        break;
                                    } 
                                    
                                break;	
                            case KEY4_Long_PRES:				             //菜单键                                      
                                    current_screen_id = Menu_Torque_Interface;//切换扭矩模式界面
                                    Cursor_id = 0;                  //清除光标标志位
                                break;	                
                        }          
            break;
            case S_Main_language:                               //语言设置界面
                        HIM_Language_display(Set_Language_Type);   //显示语言
                        HIM_ChartStandard_display(Set_Chart_Standard_Type,Set_Language_Type);//显示当前图表标准   
                        switch(KEY_State)
                        {				 
                            case KEY2_PRES:                     //上翻键
                                if(Set_Language_Type >= 1) Set_Language_Type = 0;	   
                                else Set_Language_Type++;
                                break;
                            case KEY1_PRES:                     //下翻键
                                if(Set_Language_Type <= 0) Set_Language_Type = 1;	   
                                else Set_Language_Type--;
                                break;
                            case EKEY3_PRES:                     //上翻键							
                                if(Set_Language_Type >= 1) Set_Language_Type = 0;	   
                                else Set_Language_Type++;                  
                                break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_Language_Type <= 0) Set_Language_Type = 1;	   
                                else Set_Language_Type--;            
                                break;
                            case KEY3_PRES:                     //确认键
                                SetTextFlickerCycle(0,1,0); //停止文本闪烁
                                current_screen_id = Menu_Main_Interface;   //返回菜单界面
                                break;	
                        }                                         
            break; 
            case S_Main_ChartStandard:                               //图表标准设置界面
                        HIM_ChartStandard_display(Set_Chart_Standard_Type,Set_Language_Type);//显示当前图表标准   
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
                            case KEY2_PRES:                     //上翻键
                                if(Set_Chart_Standard_Type >= 1) Set_Chart_Standard_Type = 0;	   
                                else Set_Chart_Standard_Type++;
                                break;
                            case KEY1_PRES:                     //下翻键
                                if(Set_Chart_Standard_Type <= 0) Set_Chart_Standard_Type = 1;	  
                                else Set_Chart_Standard_Type--;
                                break;
                             case EKEY3_PRES:                     //上翻键							
                                if(Set_Chart_Standard_Type >= 1) Set_Chart_Standard_Type = 0;	   
                                else Set_Chart_Standard_Type++;                 
                                break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_Chart_Standard_Type <= 0) Set_Chart_Standard_Type = 1;	  
                                else Set_Chart_Standard_Type--;          
                                break;
                            case KEY3_PRES:                     //确认键
                                SetTextFlickerCycle(0,2,0); //停止文本闪烁
                                current_screen_id = Menu_Main_Interface;   //返回菜单界面
                                Set_Bolt_Diameter = 0;   //清除螺栓直径型号标志位
                                Set_Bolt_Garder = 0;		 //清除螺栓等级型号标志位
                                break;	
                        }                                         
            break;  
            case S_Main_year:                               //年份设置界面
                    RTC_EnterConfigMode();     //允许配置 
                    switch(KEY_State)
                    {	
                        //按键
                        case KEY1_PRES:                     //上翻键
                            if(calendar.w_year >= 2099) calendar.w_year = 1970;	//年的范围为1970-2099
                            else calendar.w_year++;
                            SetTextInt32(0,3,calendar.w_year,0,4);// 显示年份
                            break;
                        case KEY2_PRES:                     //下翻键
                            if(calendar.w_year <= 1970) calendar.w_year = 2099;
                            else calendar.w_year--;
                            SetTextInt32(0,3,calendar.w_year,0,4);// 显示年份
                            break;
                        case KEY2_Long_PRES:                     //上翻键
                            if(calendar.w_year >= 2099) calendar.w_year = 1970;	//年的范围为1970-2099
                            else calendar.w_year+= 10;
                            SetTextInt32(0,3,calendar.w_year,0,4);// 显示年份
                            break;
                        case KEY1_Long_PRES:                     //下翻键
                            if(calendar.w_year <= 1970) calendar.w_year = 2099;
                            else calendar.w_year-= 10;
                            SetTextInt32(0,3,calendar.w_year,0,4);// 显示年份
                            break;
                        //旋钮
                        case EKEY3_PRES:                     //上翻键
                            if(calendar.w_year >= 2099) calendar.w_year = 1970;	//年的范围为1970-2099
                            else calendar.w_year++;
                            SetTextInt32(0,3,calendar.w_year,0,4);// 显示年份
                            break;
                        case EKEY4_PRES:                     //下翻键
                            if(calendar.w_year <= 1970) calendar.w_year = 2099;
                            else calendar.w_year--;
                            SetTextInt32(0,3,calendar.w_year,0,4);// 显示年份
                            break;
                        case EKEY2_PRES:                     //EC11 上翻键
                            if(calendar.w_year >= 2099) calendar.w_year = 1970;	//年的范围为1970-2099
                            else calendar.w_year += 10;
                            SetTextInt32(0,3,calendar.w_year,0,4);// 显示年份
                            break;
                        case EKEY1_PRES:                     //EC11  下翻键
                            if(calendar.w_year <= 1970) calendar.w_year = 2099;
                            else calendar.w_year -= 10;
                            SetTextInt32(0,3,calendar.w_year,0,4);// 显示年份
                            break;
                        case KEY3_PRES:                     //确认键
                            SetTextFlickerCycle(0,3,0);              //停止文本闪烁                            
                            current_screen_id = Menu_Main_Interface;   //返回菜单界面
                            break;	
                    }
                    RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);//RTC时钟设置                    
                    RTC_ExitConfigMode();      //退出配置模式                          
            break;    
            case S_Main_month:                               //月份设置界面
                        RTC_EnterConfigMode();     //允许配置 
                        switch(KEY_State)
                        {				 
                            case KEY1_PRES:                     //上翻键
                                if(calendar.w_month >= 12) calendar.w_month = 1;   //月份的范围为1-12
                                else calendar.w_month++;
                                SetTextInt32(0,4,calendar.w_month,0,2);// 显示月份
                                break;
                            case KEY2_PRES:                     //下翻键
                                if(calendar.w_month <= 1) calendar.w_month = 12;
                                else calendar.w_month--;
                                SetTextInt32(0,4,calendar.w_month,0,2);// 显示月份
                                break;
                            case KEY2_Long_PRES:                     //上翻键
                                if(calendar.w_month >= 12) calendar.w_month = 1;   //月份的范围为1-12
                                else calendar.w_month++;
                                SetTextInt32(0,4,calendar.w_month,0,2);// 显示月份
                            break;
                        case KEY1_Long_PRES:                     //下翻键
                                if(calendar.w_month <= 1) calendar.w_month = 12;
                                else calendar.w_month--;
                                SetTextInt32(0,4,calendar.w_month,0,2);// 显示月份
                            break;
                            case EKEY3_PRES:                     //上翻键
                                if(calendar.w_month >= 12) calendar.w_month = 1;   //月份的范围为1-12
                                else calendar.w_month++;
                                SetTextInt32(0,4,calendar.w_month,0,2);// 显示月份
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(calendar.w_month <= 1) calendar.w_month = 12;
                                else calendar.w_month--;
                                SetTextInt32(0,4,calendar.w_month,0,2);// 显示月份
                            break;
                            case EKEY2_PRES:                     //EC11 上翻键
                                if(calendar.w_month >= 12) calendar.w_month = 1;   //月份的范围为1-12
                                else calendar.w_month++;
                                SetTextInt32(0,4,calendar.w_month,0,2);// 显示月份
                            break;
                        case EKEY1_PRES:                     //EC11  下翻键
                                if(calendar.w_month <= 1) calendar.w_month = 12;
                                else calendar.w_month--;
                                SetTextInt32(0,4,calendar.w_month,0,2);// 显示月份
                            break;
                            case KEY3_PRES:                     //确认键  
                                SetTextFlickerCycle(0,4,0);              //停止文本闪烁                                  
                                current_screen_id = Menu_Main_Interface;  //返回菜单界面
                                break;	
                        }  
                        RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
                        RTC_ExitConfigMode();      //退出配置模式                          
            break;  
            case S_Main_day:                               //日设置界面
                        RTC_EnterConfigMode();     //允许配置 
                        switch(KEY_State)
                        {				                               
                            case KEY1_PRES:                     //上翻键
                                if(Is_Leap_Year(calendar.w_year)&&calendar.w_month == 2)  //闰年且月份为2时范围为1-29
                                {	
                                    if(calendar.w_date >= 29) calendar.w_date = 1;
                                    else calendar.w_date++;	
                                }
                                else 											   //普通情况由月份表决定范围
                                {	
                                    if(calendar.w_date >= mon_table[calendar.w_month - 1]) calendar.w_date = 1;
                                    else calendar.w_date++;
                                }
                                SetTextInt32(0,5,calendar.w_date,0,2);// 显示  日
                                break;
                            case KEY2_PRES:                     //下翻键
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
                                SetTextInt32(0,5,calendar.w_date,0,2);//显示  日
                                break;
                            case KEY2_Long_PRES:                     //上翻键
                                if(Is_Leap_Year(calendar.w_year)&&calendar.w_month == 2)  //闰年且月份为2时范围为1-29
                                {	
                                    if(calendar.w_date >= 29) calendar.w_date = 1;
                                    else calendar.w_date++;	
                                }
                                else 											   //普通情况由月份表决定范围
                                {	
                                    if(calendar.w_date >= mon_table[calendar.w_month - 1]) calendar.w_date = 1;
                                    else calendar.w_date++;
                                }
                                SetTextInt32(0,5,calendar.w_date,0,2);// 显示  日
                            break;
                        case KEY1_Long_PRES:                     //下翻键
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
                                SetTextInt32(0,5,calendar.w_date,0,2);//显示  日
                            break;
                            case EKEY3_PRES:                     //上翻键
                                if(Is_Leap_Year(calendar.w_year)&&calendar.w_month == 2)  //闰年且月份为2时范围为1-29
                                {	
                                    if(calendar.w_date >= 29) calendar.w_date = 1;
                                    else calendar.w_date++;	
                                }
                                else 											   //普通情况由月份表决定范围
                                {	
                                    if(calendar.w_date >= mon_table[calendar.w_month - 1]) calendar.w_date = 1;
                                    else calendar.w_date++;
                                }
                                SetTextInt32(0,5,calendar.w_date,0,2);// 显示  日
                            break;
                            case EKEY4_PRES:                     //下翻键
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
                                SetTextInt32(0,5,calendar.w_date,0,2);//显示  日
                            break;
                            case EKEY2_PRES:                     //EC11 上翻键
                                if(Is_Leap_Year(calendar.w_year)&&calendar.w_month == 2)  //闰年且月份为2时范围为1-29
                                {	
                                    if(calendar.w_date >= 29) calendar.w_date = 1;
                                    else calendar.w_date++;	
                                }
                                else 											   //普通情况由月份表决定范围
                                {	
                                    if(calendar.w_date >= mon_table[calendar.w_month - 1]) calendar.w_date = 1;
                                    else calendar.w_date++;
                                }
                                SetTextInt32(0,5,calendar.w_date,0,2);// 显示  日
                            break;
                        case EKEY1_PRES:                     //EC11  下翻键
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
                                SetTextInt32(0,5,calendar.w_date,0,2);//显示  日
                            break;
                            case KEY3_PRES:                     //确认键
                                SetTextFlickerCycle(0,5,0);              //停止文本闪烁                                  
                                current_screen_id = Menu_Main_Interface;  //返回菜单界面
                                break;	
                        } 
                        RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);                        
                        RTC_ExitConfigMode();      //退出配置模式                          
            break;          
            case Date_Set_Hour:                               //时设置界面
                        RTC_EnterConfigMode();     //允许配置 
                        switch(KEY_State)
                        {				   
                             //按键
                        case KEY1_PRES:                     //上翻键
                            if(calendar.hour >= 23) calendar.hour = 0;	   //小时的范围为0-23
                            else calendar.hour++;
                            SetTextInt32(0,6,calendar.hour,0,2);// 显示小时    
                            break;
                        case KEY2_PRES:                     //下翻键
                            if(calendar.hour <= 0) calendar.hour = 23;
                            else calendar.hour--;
                            SetTextInt32(0,6,calendar.hour,0,2);// 显示小时  
                            break;
                        case KEY2_Long_PRES:                     //上翻键
                            if(calendar.hour >= 23) calendar.hour = 0;	   //小时的范围为0-23
                            else calendar.hour++;
                            SetTextInt32(0,6,calendar.hour,0,2);// 显示小时    
                            break;
                        case KEY1_Long_PRES:                     //下翻键
                            if(calendar.hour <= 0) calendar.hour = 23;
                            else calendar.hour--;
                            SetTextInt32(0,6,calendar.hour,0,2);// 显示小时  
                            break;
                        //旋钮
                        case EKEY3_PRES:                     //上翻键
                            if(calendar.hour >= 23) calendar.hour = 0;	   //小时的范围为0-23
                            else calendar.hour++;
                            SetTextInt32(0,6,calendar.hour,0,2);// 显示小时    
                            break;
                        case EKEY4_PRES:                     //下翻键
                            if(calendar.hour <= 0) calendar.hour = 23;
                            else calendar.hour--;
                            SetTextInt32(0,6,calendar.hour,0,2);// 显示小时  
                            break;
                        case EKEY2_PRES:                     //EC11 上翻键
                            if(calendar.hour >= 23) calendar.hour = 0;	   //小时的范围为0-23
                            else calendar.hour++;
                            SetTextInt32(0,6,calendar.hour,0,2);// 显示小时    
                            break;
                        case EKEY1_PRES:                     //EC11  下翻键
                            if(calendar.hour <= 0) calendar.hour = 23;
                            else calendar.hour--;
                            SetTextInt32(0,6,calendar.hour,0,2);// 显示小时  
                            break;
                            case KEY3_PRES:                     //确认键                   
                                SetTextFlickerCycle(0,6,0);              //停止文本闪烁                                  
                                current_screen_id = Menu_Main_Interface;  //返回菜单界面
                                break;	
                        } 
                        RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
                        RTC_ExitConfigMode();      //退出配置模式                         
            break;   
            case Date_Set_Minute:                               //分设置界面
                        RTC_EnterConfigMode();     //允许配置 
                        switch(KEY_State)
                        {				 
                                 //按键
                        case KEY1_PRES:                     //上翻键
                            if(calendar.min >= 59) calendar.min = 0;	   //分钟的范围为0-59
                            else calendar.min++;
                            SetTextInt32(0,7,calendar.min,0,2);// 显示分     
                            break;
                        case KEY2_PRES:                     //下翻键
                            if(calendar.min <= 0) calendar.min = 59;
                            else calendar.min--;
                            SetTextInt32(0,7,calendar.min,0,2);// 显示分  
                            break;
                        case KEY2_Long_PRES:                     //上翻键
                            if(calendar.min >= 59) calendar.min = 0;	   //分钟的范围为0-59
                            else calendar.min++;
                            SetTextInt32(0,7,calendar.min,0,2);// 显示分    
                            break;
                        case KEY1_Long_PRES:                     //下翻键
                            if(calendar.min <= 0) calendar.min = 59;
                            else calendar.min--;
                            SetTextInt32(0,7,calendar.min,0,2);// 显示分 
                            break;
                        //旋钮
                        case EKEY3_PRES:                     //上翻键
                            if(calendar.min >= 59) calendar.min = 0;	   //分钟的范围为0-59
                            else calendar.min++;
                            SetTextInt32(0,7,calendar.min,0,2);// 显示分    
                            break;
                        case EKEY4_PRES:                     //下翻键
                            if(calendar.min <= 0) calendar.min = 59;
                            else calendar.min--;
                            SetTextInt32(0,7,calendar.min,0,2);// 显示分   
                            break;
                        case EKEY2_PRES:                     //EC11 上翻键
                            if(calendar.min >= 59) calendar.min = 0;	   //分钟的范围为0-59
                            else calendar.min++;
                            SetTextInt32(0,7,calendar.min,0,2);// 显示分     
                            break;
                        case EKEY1_PRES:                     //EC11  下翻键
                            if(calendar.min <= 0) calendar.min = 59;
                            else calendar.min--;
                            SetTextInt32(0,7,calendar.min,0,2);// 显示分  
                            break;
                            
                            case KEY3_PRES:                     //确认键                                           
                            SetTextFlickerCycle(0,7,0);              //停止文本闪烁                                  
                            current_screen_id = Menu_Main_Interface;  //返回菜单界面
                            break;	
                        }  
                        RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
                        RTC_ExitConfigMode();      //退出配置模式                          
            break;                        
            case Date_Set_Second:                               //秒设置界面
                        RTC_EnterConfigMode();     //允许配置 
                        switch(KEY_State)
                        {				                  
                             //按键
                            case KEY1_PRES:                     //上翻键
                                if(calendar.sec >= 59) calendar.sec = 0;	   //秒的范围为0-59
                                else calendar.sec++;
                                SetTextInt32(0,8,calendar.sec,0,2);// 显示秒       
                                break;
                            case KEY2_PRES:                     //下翻键
                                if(calendar.sec <= 0) calendar.sec = 59;
                                else calendar.sec--;
                                SetTextInt32(0,8,calendar.sec,0,2);// 显示秒  
                                break;
                            case KEY2_Long_PRES:                     //上翻键
                               if(calendar.sec >= 59) calendar.sec = 0;	   //秒的范围为0-59
                                else calendar.sec++;
                                SetTextInt32(0,8,calendar.sec,0,2);// 显示秒     
                                break;
                            case KEY1_Long_PRES:                     //下翻键
                                if(calendar.sec <= 0) calendar.sec = 59;
                                else calendar.sec--;
                                SetTextInt32(0,8,calendar.sec,0,2);// 显示秒  
                                break;
                            //旋钮
                            case EKEY3_PRES:                     //上翻键
                                if(calendar.sec >= 59) calendar.sec = 0;	   //秒的范围为0-59
                                else calendar.sec++;
                                SetTextInt32(0,8,calendar.sec,0,2);// 显示秒     
                                break;
                            case EKEY4_PRES:                     //下翻键
                                if(calendar.sec <= 0) calendar.sec = 59;
                                else calendar.sec--;
                                SetTextInt32(0,8,calendar.sec,0,2);// 显示秒   
                                break;
                            case EKEY2_PRES:                     //EC11 上翻键
                                if(calendar.sec >= 59) calendar.sec = 0;	   //秒的范围为0-59
                                else calendar.sec++;
                                SetTextInt32(0,8,calendar.sec,0,2);// 显示秒    
                                break;
                            case EKEY1_PRES:                     //EC11  下翻键
                                if(calendar.sec <= 0) calendar.sec = 59;
                                else calendar.sec--;
                                SetTextInt32(0,8,calendar.sec,0,2);// 显示秒  
                                break;
                            case KEY3_PRES:                     //确认键                           
                                SetTextFlickerCycle(0,8,0);              //停止文本闪烁                                  
                                current_screen_id = Menu_Main_Interface;  //返回菜单界面
                                break;	
                        } 
                        RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);                          
                        RTC_ExitConfigMode();      //退出配置模式                          
            break;  
                       
/*--------------------------------------------扭矩界面----------------------------------------------------------------------------*/    
            case Menu_Torque_Interface:                                                                                 //扭矩界面  
                        Set_Control_Mode = 1;  //扭矩模式
                        HIM_Standard_display(1,1,Set_ProductStandard_Type);   //显示型号
                        HIM_MaxTorque_display(Set_Torque_Unit,ProductCoefficient_value);//显示最大扭矩
                        HIM_MaxTorqueUnit_display(Set_Torque_Unit);  //显示最大扭矩单位 
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);  //获得目标扭矩
                        HIM_TargetTorque_display(1,4,Torque_val);//显示目标扭矩
                        HIM_TargetTorqueuint_display(1,5,Set_Torque_Unit);//显示目标扭矩单位          
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(1,6,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        HIM_ActualPressureuint_display(1,7,Set_Pressure_Unit_Type);//显示压力单位            
                        HIM_Cursor(Menu_Torque_Interface, Cursor_id); //显示光标
                        switch(KEY_State)
                        {
                            case KEY2_PRES:                     //上翻键							
                                if(Cursor_id >= Torque_interface_options) Cursor_id = 1;	 
                                else Cursor_id++;                    
                                break;
                            case KEY1_PRES:                     //下翻键
                                if(Cursor_id <= 1) Cursor_id = Torque_interface_options;	  
                                else Cursor_id--;              
                                break;
                            case EKEY3_PRES:                     //上翻键							
                                if(Cursor_id >= Torque_interface_options) Cursor_id = 1;	 
                                else Cursor_id++;                   
                                break;
                            case EKEY4_PRES:                     //下翻键
                                if(Cursor_id <= 1) Cursor_id = Torque_interface_options;	  
                                else Cursor_id--;             
                                break;
                            case KEY3_PRES:                     //确认键
                                    switch(Cursor_id)
                                    {
                                        case 1:
                                            current_screen_id = S_Torque_ProductType;        //跳转产品型号设置
                                            SetTextFlickerCycle(1,1,50); //文本闪烁
                                        break;
                                        case 2:
                                            current_screen_id = S_Torque_MaxTorqueuint;      //跳转最大扭矩设置
                                            SetTextFlickerCycle(1,3,50); //文本闪烁
                                        break;
                                        case 3:
                                            current_screen_id = S_Torque_TargetTorque;       //跳转目标扭矩设置
                                            SetTextFlickerCycle(1,4,50); //文本闪烁
                                        break;
                                        case 4:
                                            current_screen_id = S_Torque_TargetTorqueuint;   //跳转目标扭矩单位设置
                                            SetTextFlickerCycle(1,5,50); //文本闪烁
                                        break;
                                        case 5:
                                            current_screen_id = S_Torque_ActualPressureuint; //跳转压力单位设置界面
                                            SetTextFlickerCycle(1,7,50); //文本闪烁
                                        break;
                                    }
                                break;	
                            case KEY4_Long_PRES:				             //菜单键                               
                                current_screen_id = Menu_Bolt_Interface;  //切换螺栓模式界面
                                Cursor_id = 0;
                            break;	                
                        }                    
            break;        
            case S_Torque_ProductType:                                                                              //产品型号设置
                        HIM_MaxTorque_display(Set_Torque_Unit,ProductCoefficient_value);//显示最大扭矩
                        HIM_Standard_display(1,1,Set_ProductStandard_Type);         //显示产品型号   
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(1,6,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				             
                            //按键
                            case KEY1_PRES:                     //上翻键
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //型号
                                else Set_ProductStandard_Type++;    
                            break;
                            case KEY2_PRES:                     //下翻键
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;   
                            break;
                            case KEY2_Long_PRES:                     //上翻键
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //型号
                                else Set_ProductStandard_Type++;    
                            break;
                            case KEY1_Long_PRES:                     //下翻键
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;  
                            break;
                            //旋钮
                            case EKEY3_PRES:                     //上翻键
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //型号
                                else Set_ProductStandard_Type++;   
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;  
                            break;
                            case EKEY2_PRES:                     //EC11 上翻键
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //型号
                                else Set_ProductStandard_Type++;    
                            break;
                            case EKEY1_PRES:                     //EC11  下翻键
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;   
                            break;
                            case KEY3_PRES:                     //确认键   
                                    SetTextFlickerCycle(1,1,0); //文本停止闪烁                                               
                                    current_screen_id = Menu_Torque_Interface;     //返回产品型号选择界面  
                                break;	
                        }                        
            break;   
            case S_Torque_MaxTorqueuint:                                                                           //最大扭矩单位设置
                        HIM_MaxTorqueUnit_display(Set_Torque_Unit);          //显示最大扭矩单位
                        HIM_MaxTorque_display(Set_Torque_Unit,ProductCoefficient_value);//显示最大扭矩
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(1,6,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				                            
                            //按键
                            case KEY1_PRES:                     //上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;    
                            break;
                            case KEY2_PRES:                     //下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY2_Long_PRES:                     //上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case KEY1_Long_PRES:                     //下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            //旋钮
                            case EKEY3_PRES:                     //上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--; 
                            break;
                            case EKEY2_PRES:                     //EC11 上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY1_PRES:                     //EC11  下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY3_PRES:                     //确认键                             
                                SetTextFlickerCycle(1,3,0); //文本停止闪烁 
                                current_screen_id = Menu_Torque_Interface;   //返回扭矩界面
                                break;	
                        }                         
            break;  
            case S_Torque_TargetTorque:                               //目标扭矩设置界面
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);  //获得目标扭矩
                        HIM_TargetTorque_display(1,4,Torque_val);//显示目标扭矩
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(1,6,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                            case KEY1_PRES:                     //上翻键  
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //范围为0-最大扭矩
                                else TargetTorque_value++;                                      
                                break;
                            case KEY2_PRES:                     //下翻键                       
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //范围为0-最大扭矩
                                else TargetTorque_value--;                                                           
                                break;
                            case KEY2_Long_PRES:                     //上快速翻键  
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //范围为0-最大扭矩
                                else TargetTorque_value +=111 ;                                      
                                break;
                            case KEY1_Long_PRES:                     //下快速翻键                       
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //范围为0-最大扭矩
                                else TargetTorque_value -= 111;                                                           
                                break;
                            case EKEY3_PRES:                     //上翻键
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //范围为0-最大扭矩
                                else TargetTorque_value++;       
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //范围为0-最大扭矩
                                else TargetTorque_value--; 
                            break;
                            case EKEY2_PRES:                     //旋钮顺时针
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //范围为0-最大扭矩
                                else TargetTorque_value +=ROUND_TO_UINT32(ProductCoefficient_value/2);                                   
                                break;
                            case EKEY1_PRES:                     //旋钮逆时针                      
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //范围为0-最大扭矩
                                else TargetTorque_value -= ROUND_TO_UINT32(ProductCoefficient_value/2);                                                           
                                break;   
                            case KEY3_PRES:                     //确认键   
                                SetTextFlickerCycle(1,4,0); //文本停止闪烁                                  
                                current_screen_id = Menu_Torque_Interface;    //返回设置界面
                                break;	
                        }     
            break;
            case S_Torque_TargetTorqueuint:                               //目标扭矩单位设置界面
                        HIM_TargetTorqueuint_display(1,5,Set_Torque_Unit);  //显示目标扭矩单位
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);  //获得目标扭矩
                        HIM_TargetTorque_display(1,4,Torque_val);//显示目标扭矩
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(1,6,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				                   
                             //按键
                            case KEY1_PRES:                     //上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;    
                            break;
                            case KEY2_PRES:                     //下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY2_Long_PRES:                     //上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case KEY1_Long_PRES:                     //下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            //旋钮
                            case EKEY3_PRES:                     //上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--; 
                            break;
                            case EKEY2_PRES:                     //EC11 上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY1_PRES:                     //EC11  下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY3_PRES:                     //确认键    
                                SetTextFlickerCycle(1,5,0); //文本停止闪烁                                 
                                current_screen_id = Menu_Torque_Interface;     //返回设置界面
                                break;	
                        }                          
            break;   
            case S_Torque_ActualPressureuint:                                       //压力单位设置
                        HIM_ActualPressureuint_display(1,7,Set_Pressure_Unit_Type);
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(1,6,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                             //按键
                            case KEY1_PRES:                     //上翻键
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type = 0;	  
                                else Set_Pressure_Unit_Type++;    
                            break;
                            case KEY2_PRES:                     //下翻键
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type = 2;	   
                                else Set_Pressure_Unit_Type--;  
                            break;
                            case KEY2_Long_PRES:                     //上翻键
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type = 0;	  
                                else Set_Pressure_Unit_Type++;    
                            break;
                            case KEY1_Long_PRES:                     //下翻键
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type = 2;	   
                                else Set_Pressure_Unit_Type--;    
                            break;
                            //旋钮
                            case EKEY3_PRES:                     //上翻键
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type = 0;	  
                                else Set_Pressure_Unit_Type++;  
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type = 2;	   
                                else Set_Pressure_Unit_Type--;  
                            break;
                            case EKEY2_PRES:                     //EC11 上翻键
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type = 0;	  
                                else Set_Pressure_Unit_Type++;      
                            break;
                            case EKEY1_PRES:                     //EC11  下翻键
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type = 2;	   
                                else Set_Pressure_Unit_Type--;    
                            break;
                            case KEY3_PRES:                     //确认键
                                SetTextFlickerCycle(1,7,0); //文本停止闪烁                                 
                                current_screen_id = Menu_Torque_Interface;     //返回设置界面
                                break;
                        }                      
            break;                         
// /*--------------------------------------------螺栓界面----------------------------------------------------------------------------*/        
            case Bolt_Interface:                                                            //螺栓界面 
                        Set_Control_Mode = 2;  //螺栓模式
                        Bolt_Standard_display(Set_Chart_Standard_Type,Set_Bolt_Diameter);    //显示螺栓直径
                        Bolt_Grade_display(Set_Chart_Standard_Type,Set_Bolt_Garder);//显示螺栓等级
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,BoltCoefficient_value);  //获得目标扭矩
                        HIM_TargetTorque_display(2,3,Torque_val);//显示螺栓目标扭矩 
                        HIM_TargetTorqueuint_display(2,4,Set_Torque_Unit);  //显示目标扭矩单位
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(2,5,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        HIM_ActualPressureuint_display(2,6,Set_Pressure_Unit_Type);   //显示压力单位
                        HIM_Cursor(Bolt_Interface, Cursor_id);//显示光标
                        switch(KEY_State)
                        {				                
                            case KEY2_PRES:                     //上翻键							
                                if(Cursor_id >= Bolt_interface_options) Cursor_id = 1;	   
                                else Cursor_id++; 
                                HIM_Cursor(Bolt_Interface, Cursor_id);                   
                                break;
                            case KEY1_PRES:                     //下翻键
                                if(Cursor_id <= 1) Cursor_id = Bolt_interface_options;	  
                                else Cursor_id--; 
                                HIM_Cursor(Bolt_Interface, Cursor_id);               
                                break;
                            case EKEY3_PRES:                     //上翻键							
                                if(Cursor_id >= Bolt_interface_options) Cursor_id = 1;	   
                                else Cursor_id++; 
                                HIM_Cursor(Bolt_Interface, Cursor_id);                   
                                break;
                            case EKEY4_PRES:                     //下翻键
                                if(Cursor_id <= 1) Cursor_id = Bolt_interface_options;	  
                                else Cursor_id--; 
                                HIM_Cursor(Bolt_Interface, Cursor_id);              
                                break;
                            case KEY3_PRES:                     //确认键
                                   switch(Cursor_id)
                                    {
                                        case 1:
                                            current_screen_id = S_Bolt_Diameter;           //跳转螺栓等级设置
                                            SetTextFlickerCycle(2,1,50); //文本闪烁 
                                        break;
                                        case 2:
                                            current_screen_id = S_Bolt_Grade;              //跳转螺栓等级设置
                                            SetTextFlickerCycle(2,2,50); //文本闪烁
                                        break;
                                        case 3:
                                            current_screen_id = S_Bolt_TargetTorqueuint;   //跳转目标扭矩单位设置
                                            SetTextFlickerCycle(2,4,50); //文本闪烁
                                        break;
                                        case 4:
                                            current_screen_id = S_Bolt_ActualPressureuint; //跳转压力单位设置界面
                                            SetTextFlickerCycle(2,6,50); //文本闪烁
                                        break;
                                    }
                                break;	
                            case KEY4_Long_PRES:				           //菜单键                               
                                current_screen_id = Menu_Gear_Interface;  //切换角度模式界面
                                Cursor_id = 0;
                            break;	                
                        }                    
            break;   
            case S_Bolt_Diameter:                               //螺栓直径设置界面
                        Bolt_Standard_display(Set_Chart_Standard_Type,Set_Bolt_Diameter);//显示螺栓直径
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,BoltCoefficient_value);  //获得目标扭矩
                        HIM_TargetTorque_display(2,3,Torque_val);//显示螺栓目标扭矩 
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(2,5,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }          
                        switch(KEY_State)
                        {				                            
                            //按键
                            case KEY1_PRES:                     //上翻键
                                if(Set_Bolt_Diameter >= Bolt_DiameterNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1) Set_Bolt_Diameter=0;	   
                                else Set_Bolt_Diameter++;     
                            break;
                            case KEY2_PRES:                     //下翻键
                                if(Set_Bolt_Diameter <= 0) Set_Bolt_Diameter=Bolt_DiameterNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1;	   
                                else Set_Bolt_Diameter--;  
                            break;
                            case KEY2_Long_PRES:                     //上翻键
                                if(Set_Bolt_Diameter >= Bolt_DiameterNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1) Set_Bolt_Diameter=0;	   
                                else Set_Bolt_Diameter++;     
                            break;
                            case KEY1_Long_PRES:                     //下翻键
                                if(Set_Bolt_Diameter <= 0) Set_Bolt_Diameter=Bolt_DiameterNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1;	   
                                else Set_Bolt_Diameter--; 
                            break;
                            //旋钮
                            case EKEY3_PRES:                     //上翻键
                                if(Set_Bolt_Diameter >= Bolt_DiameterNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1) Set_Bolt_Diameter=0;	   
                                else Set_Bolt_Diameter++;    
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_Bolt_Diameter <= 0) Set_Bolt_Diameter=Bolt_DiameterNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1;	   
                                else Set_Bolt_Diameter--; 
                            break;
                            case EKEY2_PRES:                     //EC11 上翻键
                                if(Set_Bolt_Diameter >= Bolt_DiameterNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1) Set_Bolt_Diameter=0;	   
                                else Set_Bolt_Diameter++;      
                            break;
                            case EKEY1_PRES:                     //EC11  下翻键
                                if(Set_Bolt_Diameter <= 0) Set_Bolt_Diameter=Bolt_DiameterNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1;	   
                                else Set_Bolt_Diameter--;  
                            break;
                            case KEY3_PRES:                     //确认键 
                                SetTextFlickerCycle(2,1,0); //文本停止闪烁                                                          
                                current_screen_id = Bolt_Interface;   //返回螺母模式界面
                                break;	
                        }                         
            break; 
            case S_Bolt_Grade:                               //螺栓等级设置界面
                        Bolt_Grade_display(Set_Chart_Standard_Type,Set_Bolt_Garder);//显示螺栓等级
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,BoltCoefficient_value);  //获得目标扭矩
                        HIM_TargetTorque_display(2,3,Torque_val);//显示螺栓目标扭矩 
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(2,5,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                            //按键
                            case KEY1_PRES:                     //上翻键
                                if(Set_Bolt_Garder >= Bolt_GradeNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1) Set_Bolt_Garder=0;	   //范围为1-4
                                else Set_Bolt_Garder++;     
                            break;
                            case KEY2_PRES:                     //下翻键
                                if(Set_Bolt_Garder <= 0) Set_Bolt_Garder=Bolt_GradeNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1;	   //范围为1-4
                                else Set_Bolt_Garder--;   
                            break;
                            case KEY2_Long_PRES:                     //上翻键
                                if(Set_Bolt_Garder >= Bolt_GradeNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1) Set_Bolt_Garder=0;	   //范围为1-4
                                else Set_Bolt_Garder++;      
                            break;
                            case KEY1_Long_PRES:                     //下翻键
                                if(Set_Bolt_Garder <= 0) Set_Bolt_Garder=Bolt_GradeNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1;	   //范围为1-4
                                else Set_Bolt_Garder--;  
                            break;
                            //旋钮
                            case EKEY3_PRES:                     //上翻键
                                if(Set_Bolt_Garder >= Bolt_GradeNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1) Set_Bolt_Garder=0;	   //范围为1-4
                                else Set_Bolt_Garder++;    
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_Bolt_Garder <= 0) Set_Bolt_Garder=Bolt_GradeNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1;	   //范围为1-4
                                else Set_Bolt_Garder--;  
                            break;
                            case EKEY2_PRES:                     //EC11 上翻键
                                if(Set_Bolt_Garder >= Bolt_GradeNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1) Set_Bolt_Garder=0;	   //范围为1-4
                                else Set_Bolt_Garder++;       
                            break;
                            case EKEY1_PRES:                     //EC11  下翻键
                                if(Set_Bolt_Garder <= 0) Set_Bolt_Garder=Bolt_GradeNumber(Set_Chart_Standard_Type,Set_ProductStandard_Type) - 1;	   //范围为1-4
                                else Set_Bolt_Garder--;   
                            break;
                            case KEY3_PRES:                     //确认键
                                SetTextFlickerCycle(2,2,0); //文本停止闪烁                                           
                                current_screen_id = Bolt_Interface;    //返回螺母模式界面
                                break;	
                        }                       
            break;   
            case S_Bolt_TargetTorqueuint:                                               //目标扭矩单位设置
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,BoltCoefficient_value);  //获得目标扭矩
                        HIM_TargetTorque_display(2,3,Torque_val);//显示螺栓目标扭矩 
                        HIM_TargetTorqueuint_display(2,4,Set_Torque_Unit);  //显示目标扭矩单位
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(2,5,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                            //按键
                            case KEY1_PRES:                     //上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;    
                            break;
                            case KEY2_PRES:                     //下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY2_Long_PRES:                     //上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case KEY1_Long_PRES:                     //下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            //旋钮
                            case EKEY3_PRES:                     //上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--; 
                            break;
                            case EKEY2_PRES:                     //EC11 上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY1_PRES:                     //EC11  下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY3_PRES:                     //确认键                       
                                SetTextFlickerCycle(2,4,0); //文本停止闪烁
                                current_screen_id = Bolt_Interface;    //返回螺母模式界面
                                break;	
                        }   
                     
            break;
            case S_Bolt_ActualPressureuint:                                               //实时压力单位设置
                        HIM_ActualPressureuint_display(2,6,Set_Pressure_Unit_Type);   //显示压力单位
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(2,5,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                             //按键
                            case KEY1_PRES:                     //上翻键
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;  
                            break;
                            case KEY2_PRES:                     //下翻键
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--;   
                            break;
                            case KEY2_Long_PRES:                     //上翻键
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;        
                            break;
                            case KEY1_Long_PRES:                     //下翻键
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--; 
                            break;
                            //旋钮
                            case EKEY3_PRES:                     //上翻键
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;       
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--;   
                            break;
                            case EKEY2_PRES:                     //EC11 上翻键
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;           
                            break;
                            case EKEY1_PRES:                     //EC11  下翻键
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--;   
                            break;
                            case KEY3_PRES:                     //确认键
                                SetTextFlickerCycle(2,6,0); //文本停止闪烁                                 
                                current_screen_id = Bolt_Interface;      //返回螺母模式界面
                                break;
                        } 
            break;                                            
 /*--------------------------------------------角度界面----------------------------------------------------------------------------*/        
            case Menu_Angle_Interface:                                                     //角度界面
                        Set_Control_Mode = 4;  //角度模式
                        HIM_Standard_display(3,1,Set_ProductStandard_Type);   //显示型号
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);  //获得目标扭矩
                        HIM_TargetTorque_display(3,2,Torque_val);//显示目标扭矩 
                        HIM_TargetTorqueuint_display(3,3,Set_Torque_Unit);  //显示目标扭矩单位
                        SetTextInt32(3,4,Set_Target_Angle,0,0);  //显示目标角度
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(3,5,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        HIM_ActualPressureuint_display(3,6,Set_Pressure_Unit_Type);  //压力单位显示
                        HIM_Cursor(Menu_Angle_Interface, Cursor_id);                  
                        switch(KEY_State)
                        {				 
                            case KEY2_PRES:                     //上翻键							
                                if(Cursor_id >= 5) Cursor_id = 1;	  
                                else Cursor_id++; 
                                break;
                            case KEY1_PRES:                     //下翻键
                                if(Cursor_id <= 1) Cursor_id = 5;	   
                                else Cursor_id--;              
                                break;
                            case EKEY3_PRES:                     //上翻键							
                                if(Cursor_id >= 5) Cursor_id = 1;	  
                                else Cursor_id++;                  
                                break;
                            case EKEY4_PRES:                     //下翻键
                                if(Cursor_id <= 1) Cursor_id = 5;	   
                                else Cursor_id--;             
                                break;
                            case KEY3_PRES:                     //确认键
                                    switch(Cursor_id)
                                    {
                                        case 1:
                                            current_screen_id = S_Angle_ProductType;     //跳转对应设置界面
                                            SetTextFlickerCycle(3,1,50); //文本闪烁 
                                        break;
                                        case 2:
                                            current_screen_id = S_Angle_PresetTorque;   //跳转对应设置界面
                                            SetTextFlickerCycle(3,2,50); //文本闪烁 
                                        break;
                                        case 3:
                                            current_screen_id = S_Angle_PresetTorqueuint;   //跳转对应设置界面
                                            SetTextFlickerCycle(3,3,50); //文本闪烁 
                                        break;
                                        case 4:
                                            current_screen_id = S_Angle_TargetAngle;     //跳转对应设置界面
                                            SetTextFlickerCycle(3,4,50); //文本闪烁 
                                        break;
                                        case 5:
                                            current_screen_id = S_Angle_ActualPressureuint;   //跳转对应设置界面
                                            SetTextFlickerCycle(3,6,50); //文本闪烁 
                                        break;
                                    }
                                break;	
                            case KEY4_Long_PRES:				            //菜单键                               
                                current_screen_id = Menu_Sensor_Interface;        //切换档位模式界面
                                Cursor_id = 0;   
                                break;	                
                        }                                           
            break;  
            case S_Angle_ProductType:                                                                              //产品型号设置
                        HIM_Standard_display(3,1,Set_ProductStandard_Type);         //显示产品型号 
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(3,5,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }           
                        switch(KEY_State)
                        {				                       
                             //按键
                            case KEY1_PRES:                     //上翻键
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //型号
                                else Set_ProductStandard_Type++;    
                            break;
                            case KEY2_PRES:                     //下翻键
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;   
                            break;
                            case KEY2_Long_PRES:                     //上翻键
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //型号
                                else Set_ProductStandard_Type++;    
                            break;
                            case KEY1_Long_PRES:                     //下翻键
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;  
                            break;
                            //旋钮
                            case EKEY3_PRES:                     //上翻键
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //型号
                                else Set_ProductStandard_Type++;   
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;  
                            break;
                            case EKEY2_PRES:                     //EC11 上翻键
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //型号
                                else Set_ProductStandard_Type++;    
                            break;
                            case EKEY1_PRES:                     //EC11  下翻键
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;   
                            break;
                            case KEY3_PRES:                     //确认键   
                                SetTextFlickerCycle(3,1,0); //文本停止闪烁                                               
                                current_screen_id = Menu_Angle_Interface;     //返回产品型号选择界面  
                            break;	
                        }                              
            break;
            case S_Angle_PresetTorque:                               //目标扭矩设置界面
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);  //获得目标扭矩
                        HIM_TargetTorque_display(3,2,Torque_val);//显示目标扭矩 
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(3,5,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				                                   
                            case KEY1_PRES:                     //上翻键  
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //范围为0-最大扭矩
                                else TargetTorque_value++;                                      
                                break;
                            case KEY2_PRES:                     //下翻键                       
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //范围为0-最大扭矩
                                else TargetTorque_value--;                                                           
                                break;
                            case KEY2_Long_PRES:                     //上快速翻键  
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //范围为0-最大扭矩
                                else TargetTorque_value +=111 ;                                      
                                break;
                            case KEY1_Long_PRES:                     //下快速翻键                       
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //范围为0-最大扭矩
                                else TargetTorque_value -= 111;                                                           
                                break;
                            case EKEY3_PRES:                     //上翻键
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //范围为0-最大扭矩
                                else TargetTorque_value++;       
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //范围为0-最大扭矩
                                else TargetTorque_value--; 
                            break;
                            case EKEY2_PRES:                     //旋钮顺时针
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //范围为0-最大扭矩
                                else TargetTorque_value +=ROUND_TO_UINT32(ProductCoefficient_value/2);                                   
                                break;
                            case EKEY1_PRES:                     //旋钮逆时针                      
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //范围为0-最大扭矩
                                else TargetTorque_value -= ROUND_TO_UINT32(ProductCoefficient_value/2);                                                           
                                break;  
                            case KEY3_PRES:                     //确认键  
                                SetTextFlickerCycle(3,2,0); //文本停止闪烁  
                                current_screen_id = Menu_Angle_Interface;     //返回角度模式界面
                                break;	
                        }                        
            break;
            case S_Angle_PresetTorqueuint:                               //目标扭矩单位设置界面
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);  //获得目标扭矩
                        HIM_TargetTorque_display(3,2,Torque_val);//显示目标扭矩 
                        HIM_TargetTorqueuint_display(3,3,Set_Torque_Unit);  //显示目标扭矩单位
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(3,5,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                            //按键
                            case KEY1_PRES:                     //上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;    
                            break;
                            case KEY2_PRES:                     //下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY2_Long_PRES:                     //上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case KEY1_Long_PRES:                     //下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            //旋钮
                            case EKEY3_PRES:                     //上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--; 
                            break;
                            case EKEY2_PRES:                     //EC11 上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY1_PRES:                     //EC11  下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY3_PRES:                     //确认键  
                                SetTextFlickerCycle(3,3,0); //文本停止闪烁  
                                current_screen_id = Menu_Angle_Interface;     //返回角度模式界面
                                break;	
                        }                        
            break;                    
            case S_Angle_TargetAngle:                               //目标角度设置界面
                        SetTextInt32(3,4,Set_Target_Angle,0,0);
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(3,5,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				                           
                           case KEY1_PRES:                     //上翻键  
                                if(Set_Target_Angle >= Max_Angle) Set_Target_Angle = 0;  
                                else Set_Target_Angle++;                                       
                                break;
                            case KEY2_PRES:                     //下翻键                       
                                if(Set_Target_Angle <= 0) Set_Target_Angle = Max_Angle;  
                                else Set_Target_Angle--;                                                           
                                break;
                            case KEY2_Long_PRES:                     //上快速翻键  
                                if(Set_Target_Angle >= Max_Angle) Set_Target_Angle = 0;  
                                else Set_Target_Angle+= 11;                                     
                                break;
                            case KEY1_Long_PRES:                     //下快速翻键                       
                                if(Set_Target_Angle <= 0) Set_Target_Angle = Max_Angle;  
                                else Set_Target_Angle-= 11;                                                            
                                break;
                            case EKEY3_PRES:                     //上翻键
                                if(Set_Target_Angle >= Max_Angle) Set_Target_Angle = 0;  
                                else Set_Target_Angle++;        
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_Target_Angle <= 0) Set_Target_Angle = Max_Angle;  
                                else Set_Target_Angle--; 
                            break;
                            case EKEY2_PRES:                     //旋钮顺时针
                                if(Set_Target_Angle >= Max_Angle) Set_Target_Angle = 0;  
                                else Set_Target_Angle+= 11;                                   
                                break;
                            case EKEY1_PRES:                     //旋钮逆时针                      
                                if(Set_Target_Angle <= 0) Set_Target_Angle = Max_Angle;  
                                else Set_Target_Angle-= 11;                                                         
                                break;  
                            case KEY3_PRES:                     //确认键  
                                SetTextFlickerCycle(3,4,0); //文本停止闪烁  
                                current_screen_id = Menu_Angle_Interface;     //返回角度模式界面
                                break;	
                        }                        
            break;                        
            case S_Angle_ActualPressureuint:                                       //压力单位设置
                        HIM_ActualPressureuint_display(3,6,Set_Pressure_Unit_Type);  //压力单位显示
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(3,5,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				                         
                              //按键
                            case KEY1_PRES:                     //上翻键
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;  
                            break;
                            case KEY2_PRES:                     //下翻键
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--;   
                            break;
                            case KEY2_Long_PRES:                     //上翻键
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;        
                            break;
                            case KEY1_Long_PRES:                     //下翻键
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--; 
                            break;
                            //旋钮
                            case EKEY3_PRES:                     //上翻键
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;       
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--;   
                            break;
                            case EKEY2_PRES:                     //EC11 上翻键
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;           
                            break;
                            case EKEY1_PRES:                     //EC11  下翻键
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--;   
                            break;
                            case KEY3_PRES:                     //确认键
                                SetTextFlickerCycle(3,6,0); //文本停止闪烁                                 
                                current_screen_id = Menu_Angle_Interface;     //返回设置界面
                                break;
                        }                      
            break;
                        
/*--------------------------------------------档位界面----------------------------------------------------------------------------*/              
            case Menu_Gear_Interface:                                                //档位界面
                        Set_Control_Mode = 3;  //档位模式
                        HIM_Standard_display(4,1,Set_ProductStandard_Type);         //显示产品型号 
                        SetTextInt32(4,2,Gear_level,0,0);  //显示目标档位 
//                        printf( "GearTorque_value:%d\r\n",GearTorque_value );
//                        printf( "ProductCoefficient_value:%f\r\n",ProductCoefficient_value );
//                        printf( "Set_Target_Gear:%d\r\n",Set_Target_Gear );          
                        HIM_GearTorqueValue(4,3,Gear_val);//显示档位扭矩
                        HIM_TargetTorqueuint_display(4,4,Set_Torque_Unit);  //显示扭矩单位
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(4,5,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        HIM_ActualPressureuint_display(4,6,Set_Pressure_Unit_Type);  //压力单位显示
                        HIM_Cursor(Menu_Gear_Interface, Cursor_id);                   
                        switch(KEY_State)
                        {				 
                            case KEY2_PRES:                     //上翻键							
                                if(Cursor_id >= 4) Cursor_id = 1;	  
                                else Cursor_id++; 	                  
                                break;
                            case KEY1_PRES:                     //下翻键
                                if(Cursor_id <= 1) Cursor_id = 4;	  
                                else Cursor_id--;             
                                break;
                            case EKEY3_PRES:                     //上翻键							
                                if(Cursor_id >= 4) Cursor_id = 1;	  
                                else Cursor_id++;                    
                                break;
                            case EKEY4_PRES:                     //下翻键
                                if(Cursor_id <= 1) Cursor_id = 4;	  
                                else Cursor_id--;                
                                break;
                            case KEY3_PRES:                     //确认键
                                    switch(Cursor_id)
                                    {
                                        case 1:
                                            current_screen_id = S_Gear_ProductType;      //产品型号设置
                                            SetTextFlickerCycle(4,1,50); //文本闪烁 
                                        break;
                                        case 2:
                                            current_screen_id = S_Gear_TargetGear;       //跳转对应设置界面
                                            SetTextFlickerCycle(4,2,50); //文本闪烁 
                                        break;
                                        case 3:
                                            current_screen_id = S_Gear_TargetTorqueuint;     //跳转对应设置界面
                                            SetTextFlickerCycle(4,4,50); //文本闪烁 
                                        break;
                                        case 4:
                                            current_screen_id = S_Gear_ActualPressureuint;     //跳转对应设置界面
                                            SetTextFlickerCycle(4,6,50); //文本闪烁 
                                        break;
                                    }
                                break;	
                            case KEY4_Long_PRES:				            //菜单键                               
                                current_screen_id = Menu_Angle_Interface;     //切换传感器模式界面
                                Cursor_id = 0;
                                break;	                
                        }                     
            break;
            case S_Gear_ProductType:                                                                              //产品型号设置
                        HIM_Standard_display(4,1,Set_ProductStandard_Type);         //显示产品型号  
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(4,5,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }           
                        switch(KEY_State)
                        {				 
                             //按键
                            case KEY1_PRES:                     //上翻键
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //型号
                                else Set_ProductStandard_Type++;    
                            break;
                            case KEY2_PRES:                     //下翻键
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;   
                            break;
                            case KEY2_Long_PRES:                     //上翻键
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //型号
                                else Set_ProductStandard_Type++;    
                            break;
                            case KEY1_Long_PRES:                     //下翻键
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;  
                            break;
                            //旋钮
                            case EKEY3_PRES:                     //上翻键
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //型号
                                else Set_ProductStandard_Type++;   
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;  
                            break;
                            case EKEY2_PRES:                     //EC11 上翻键
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //型号
                                else Set_ProductStandard_Type++;    
                            break;
                            case EKEY1_PRES:                     //EC11  下翻键
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;   
                            break;
                            case KEY3_PRES:                     //确认键   
                                    SetTextFlickerCycle(4,1,0); //文本停止闪烁                                               
                                    current_screen_id = Menu_Gear_Interface;     //返回产品型号选择界面  
                                break;	
                        }                              
            break; 
            case S_Gear_TargetGear:                               //目标档位设置界面
                        SetTextInt32(4,2,Gear_level,0,0);  //显示目标档位
                        HIM_GearTorqueValue(4,3,Gear_val);//显示档位扭矩
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(4,5,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                             //按键
                            case KEY1_PRES:                     //上翻键
                                if(Set_Target_Gear >= Gear_length -1 ) Set_Target_Gear=0;	  
                                else Set_Target_Gear++;  
                            break;
                            case KEY2_PRES:                     //下翻键
                                if(Set_Target_Gear <= 0) Set_Target_Gear=Gear_length -1 ;	   
                                else Set_Target_Gear--;  
                            break;
                            case KEY2_Long_PRES:                     //上翻键
                                if(Set_Target_Gear >= Gear_length -1 ) Set_Target_Gear=0;	  
                                else Set_Target_Gear++;  
                            break;
                            case KEY1_Long_PRES:                     //下翻键
                                if(Set_Target_Gear <= 0) Set_Target_Gear=Gear_length -1 ;	   
                                else Set_Target_Gear--;  
                            break;
                            //旋钮
                            case EKEY3_PRES:                     //上翻键
                                if(Set_Target_Gear >= Gear_length -1 ) Set_Target_Gear=0;	  
                                else Set_Target_Gear++;   
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_Target_Gear <= 0) Set_Target_Gear=Gear_length -1 ;	   
                                else Set_Target_Gear--;  
                            break;
                            case EKEY2_PRES:                     //EC11 上翻键
                                if(Set_Target_Gear >= Gear_length -1 ) Set_Target_Gear=0;	  
                                else Set_Target_Gear++;    
                            break;
                            case EKEY1_PRES:                     //EC11  下翻键
                                if(Set_Target_Gear <= 0) Set_Target_Gear=Gear_length -1 ;	   
                                else Set_Target_Gear--;   
                            break;
                            case KEY3_PRES:                     //确认键                 
                               SetTextFlickerCycle(4,2,0); //文本停止闪烁  
                               current_screen_id = Menu_Gear_Interface;    //返回档位模式界面
                                break;	
                        }   
                     
            break;
            case S_Gear_TargetTorqueuint:                               //目标扭矩单位设置界面
                        HIM_GearTorqueValue(4,3,Gear_val);//显示档位扭矩
                        HIM_TargetTorqueuint_display(4,4,Set_Torque_Unit);  //显示扭矩单位
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(4,5,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                             //按键
                            case KEY1_PRES:                     //上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;    
                            break;
                            case KEY2_PRES:                     //下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY2_Long_PRES:                     //上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case KEY1_Long_PRES:                     //下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            //旋钮
                            case EKEY3_PRES:                     //上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--; 
                            break;
                            case EKEY2_PRES:                     //EC11 上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY1_PRES:                     //EC11  下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY3_PRES:                     //确认键    
                                SetTextFlickerCycle(4,4,0); //文本停止闪烁                                 
                                current_screen_id = Menu_Gear_Interface;     //返回设置界面
                                break;	
                        }                          
            break;
            case S_Gear_ActualPressureuint:                                       //压力单位设置
                        HIM_ActualPressureuint_display(4,6,Set_Pressure_Unit_Type);  //压力单位显示
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(4,5,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                            //按键
                            case KEY1_PRES:                     //上翻键
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   
                                else Set_Pressure_Unit_Type++;   
                            break;
                            case KEY2_PRES:                     //下翻键
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   
                                else Set_Pressure_Unit_Type--;  
                            break;
                            case KEY2_Long_PRES:                     //上翻键
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   
                                else Set_Pressure_Unit_Type++;     
                            break;
                            case KEY1_Long_PRES:                     //下翻键
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   
                                else Set_Pressure_Unit_Type--;  
                            break;
                            //旋钮
                            case EKEY3_PRES:                     //上翻键
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   
                                else Set_Pressure_Unit_Type++;     
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   
                                else Set_Pressure_Unit_Type--;  
                            break;
                            case EKEY2_PRES:                     //EC11 上翻键
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   
                                else Set_Pressure_Unit_Type++;      
                            break;
                            case EKEY1_PRES:                     //EC11  下翻键
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   
                                else Set_Pressure_Unit_Type--;     
                            break;
                            case KEY3_PRES:                     //确认键
                                SetTextFlickerCycle(4,6,0); //文本停止闪烁                                 
                                current_screen_id = Menu_Gear_Interface;     //返回设置界面
                                break;
                        }                      
            break;
/*--------------------------------------------传感器界面----------------------------------------------------------------------------*/              
            case Menu_Sensor_Interface:                                                
                        Set_Control_Mode = 5;  //传感器模式
                        HIM_Standard_display(5,1,Set_ProductStandard_Type);         //显示产品型号
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,Set_SensorTorque);  //获得预设扭矩
                        HIM_TargetTorque_display(5,2,Torque_val);//显示预设扭矩
                        HIM_TargetTorqueuint_display(5,3,Set_Torque_Unit);  //显示预设扭矩单位
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);  //获得目标扭矩
                        HIM_TargetTorque_display(5,4,Torque_val);//显示目标扭矩
                        HIM_TargetTorqueuint_display(5,5,Set_Torque_Unit);  //显示目标扭矩单位                       
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(5,6,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        HIM_ActualPressureuint_display(5,7,Set_Pressure_Unit_Type);  //压力单位显示
                        HIM_Cursor(Menu_Sensor_Interface, Cursor_id);                   
                        switch(KEY_State)
                        {				 
                            case KEY2_PRES:                     //上翻键							
                                if(Cursor_id >= 6) Cursor_id = 1;	   
                                else Cursor_id++; 		                  
                                break;
                            case KEY1_PRES:                     //下翻键
                                if(Cursor_id <= 1) Cursor_id = 6;	  
                                else Cursor_id--;             
                                break;
                            case EKEY3_PRES:                     //上翻键							
                                if(Cursor_id >= 6) Cursor_id = 1;	   
                                else Cursor_id++;                     
                                break;
                            case EKEY4_PRES:                     //下翻键
                                if(Cursor_id <= 1) Cursor_id = 6;	  
                                else Cursor_id--;                 
                                break;
                            case KEY3_PRES:                     //确认键
                                    switch(Cursor_id)
                                    {
                                        case 1:
                                            current_screen_id = S_Sensor_ProductType; //跳转对应设置界面
                                            SetTextFlickerCycle(5,1,50); //文本闪烁 
                                        break;
                                        case 2:
                                            current_screen_id = S_Sensor_PresetTorque;       //跳转对应设置界面
                                            SetTextFlickerCycle(5,2,50); //文本闪烁 
                                        break;
                                        case 3:
                                            current_screen_id = S_Sensor_PresetTorqueuint;     //跳转对应设置界面
                                            SetTextFlickerCycle(5,3,50); //文本闪烁 
                                        break;
                                        case 4:
                                            current_screen_id = S_Sensor_TargetTorque;     //跳转对应设置界面
                                            SetTextFlickerCycle(5,4,50); //文本闪烁 
                                        break;
                                        case 5:
                                            current_screen_id = S_Sensor_TargetTorqueuint;     //跳转对应设置界面
                                            SetTextFlickerCycle(5,5,50); //文本闪烁 
                                        break;
                                        case 6:
                                            current_screen_id = S_Sensor_ActualPressureuint;     //跳转对应设置界面
                                            SetTextFlickerCycle(5,7,50); //文本闪烁 
                                        break;
                                    }
                                break;	
                            case KEY4_Long_PRES:				            //菜单键                               
                                current_screen_id = Menu_Main_Interface;     //切换菜单模式界面
                                Cursor_id = 0;
                                break;	                
                        }                     
            break;
            case S_Sensor_ProductType:                                                                              //产品型号设置
                        HIM_Standard_display(5,1,Set_ProductStandard_Type);         //显示产品型号  
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(5,6,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                             //按键
                            case KEY1_PRES:                     //上翻键
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //型号
                                else Set_ProductStandard_Type++;    
                            break;
                            case KEY2_PRES:                     //下翻键
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;   
                            break;
                            case KEY2_Long_PRES:                     //上翻键
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //型号
                                else Set_ProductStandard_Type++;    
                            break;
                            case KEY1_Long_PRES:                     //下翻键
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;  
                            break;
                            //旋钮
                            case EKEY3_PRES:                     //上翻键
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //型号
                                else Set_ProductStandard_Type++;   
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;  
                            break;
                            case EKEY2_PRES:                     //EC11 上翻键
                                if(Set_ProductStandard_Type >= Spanner_Number - 1) Set_ProductStandard_Type = 0;   //型号
                                else Set_ProductStandard_Type++;    
                            break;
                            case EKEY1_PRES:                     //EC11  下翻键
                                if(Set_ProductStandard_Type <= 0) Set_ProductStandard_Type = Spanner_Number - 1 ;
                                else Set_ProductStandard_Type--;   
                            break;
                            case KEY3_PRES:                     //确认键   
                                    SetTextFlickerCycle(5,1,0); //文本停止闪烁                                               
                                    current_screen_id = Menu_Sensor_Interface;     //返回产品型号选择界面 
                                break;	
                        }                              
            break;
            case S_Sensor_PresetTorque:                               //预设扭矩参数设置
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,Set_SensorTorque);  //获得预设扭矩
                        HIM_TargetTorque_display(5,2,Torque_val);//显示预设扭矩
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(5,6,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				                      
                             case KEY1_PRES:                     //上翻键  
                                if(Set_SensorTorque >= Spanner_Sensor_parameter(Set_ProductStandard_Type)) Set_SensorTorque=0;	   //范围为0-最大扭矩
                                else Set_SensorTorque++;                                     
                                break;
                            case KEY2_PRES:                     //下翻键                       
                                if(Set_SensorTorque <= 0) Set_SensorTorque = Spanner_Sensor_parameter(Set_ProductStandard_Type);	   //范围为0-最大扭矩
                                else Set_SensorTorque--;                                                           
                                break;
                            case KEY2_Long_PRES:                     //上快速翻键  
                                if(Set_SensorTorque >= Spanner_Sensor_parameter(Set_ProductStandard_Type)) Set_SensorTorque=0;	   //范围为0-最大扭矩
                                else Set_SensorTorque+= 111;                                      
                                break;
                            case KEY1_Long_PRES:                     //下快速翻键                       
                                if(Set_SensorTorque <= 0) Set_SensorTorque = Spanner_Sensor_parameter(Set_ProductStandard_Type);	   //范围为0-最大扭矩
                                else Set_SensorTorque-= 111;                                                            
                                break;
                            case EKEY3_PRES:                     //上翻键
                                if(Set_SensorTorque >= Spanner_Sensor_parameter(Set_ProductStandard_Type)) Set_SensorTorque=0;	   //范围为0-最大扭矩
                                else Set_SensorTorque++;      
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_SensorTorque <= 0) Set_SensorTorque = Spanner_Sensor_parameter(Set_ProductStandard_Type);	   //范围为0-最大扭矩
                                else Set_SensorTorque--; 
                            break;
                            case EKEY2_PRES:                     //旋钮顺时针
                                if(Set_SensorTorque >= Spanner_Sensor_parameter(Set_ProductStandard_Type)) Set_SensorTorque=0;	   //范围为0-最大扭矩
                                else Set_SensorTorque+= ROUND_TO_UINT32(ProductCoefficient_value/2);                                     
                                break;
                            case EKEY1_PRES:                     //旋钮逆时针                      
                                if(Set_SensorTorque <= 0) Set_SensorTorque = Spanner_Sensor_parameter(Set_ProductStandard_Type);	   //范围为0-最大扭矩
                                else Set_SensorTorque-= ROUND_TO_UINT32(ProductCoefficient_value/2);                                                           
                                break;
                            case KEY3_PRES:                     //确认键   
                                SetTextFlickerCycle(5,2,0); //文本停止闪烁                                  
                                current_screen_id = Menu_Sensor_Interface;    //返回设置界面
                                break;	
                        }     
            break;
            case S_Sensor_PresetTorqueuint:                               //预设扭矩单位设置
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,Set_SensorTorque);  //获得预设扭矩
                        HIM_TargetTorque_display(5,2,Torque_val);//显示预设扭矩
                        HIM_TargetTorqueuint_display(5,3,Set_Torque_Unit);  //显示预设扭矩单位
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(5,6,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                             //按键
                            case KEY1_PRES:                     //上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;    
                            break;
                            case KEY2_PRES:                     //下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY2_Long_PRES:                     //上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case KEY1_Long_PRES:                     //下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            //旋钮
                            case EKEY3_PRES:                     //上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--; 
                            break;
                            case EKEY2_PRES:                     //EC11 上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY1_PRES:                     //EC11  下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY3_PRES:                     //确认键    
                                SetTextFlickerCycle(5,3,0); //文本停止闪烁                                 
                                current_screen_id = Menu_Sensor_Interface;     //返回设置界面
                                break;	
                        }                          
            break;
            case S_Sensor_TargetTorque:                               //目标扭矩参数设置界面
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);  //获得目标扭矩
                        HIM_TargetTorque_display(5,4,Torque_val);//显示目标扭矩
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(5,6,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                             case KEY1_PRES:                     //上翻键  
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //范围为0-最大扭矩
                                else TargetTorque_value++;                                      
                                break;
                            case KEY2_PRES:                     //下翻键                       
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //范围为0-最大扭矩
                                else TargetTorque_value--;                                                           
                                break;
                            case KEY2_Long_PRES:                     //上快速翻键  
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //范围为0-最大扭矩
                                else TargetTorque_value +=111 ;                                      
                                break;
                            case KEY1_Long_PRES:                     //下快速翻键                       
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //范围为0-最大扭矩
                                else TargetTorque_value -= 111;                                                           
                                break;
                            case EKEY3_PRES:                     //上翻键
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //范围为0-最大扭矩
                                else TargetTorque_value++;       
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //范围为0-最大扭矩
                                else TargetTorque_value--; 
                            break;
                            case EKEY2_PRES:                     //旋钮顺时针
                                if(TargetTorque_value >= (ROUND_TO_UINT32(ProductCoefficient_value*70))) TargetTorque_value=0;	   //范围为0-最大扭矩
                                else TargetTorque_value +=ROUND_TO_UINT32(ProductCoefficient_value/2);                                   
                                break;
                            case EKEY1_PRES:                     //旋钮逆时针                      
                                if(TargetTorque_value <= 0) TargetTorque_value = (ROUND_TO_UINT32(ProductCoefficient_value*70));	   //范围为0-最大扭矩
                                else TargetTorque_value -= ROUND_TO_UINT32(ProductCoefficient_value/2);                                                           
                                break;
                            case KEY3_PRES:                     //确认键   
                                SetTextFlickerCycle(5,4,0); //文本停止闪烁                                  
                                current_screen_id = Menu_Sensor_Interface;    //返回设置界面
                                break;	
                        }     
            break;
            case S_Sensor_TargetTorqueuint:                               //目标扭矩单位设置界面
                        Torque_val = Get_TargetTorque(Set_Torque_Unit,TargetTorque_value);  //获得目标扭矩
                        HIM_TargetTorque_display(5,4,Torque_val);//显示目标扭矩
                        HIM_TargetTorqueuint_display(5,5,Set_Torque_Unit);  //显示目标扭矩单位
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(5,6,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                              //按键
                            case KEY1_PRES:                     //上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;    
                            break;
                            case KEY2_PRES:                     //下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY2_Long_PRES:                     //上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case KEY1_Long_PRES:                     //下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            //旋钮
                            case EKEY3_PRES:                     //上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--; 
                            break;
                            case EKEY2_PRES:                     //EC11 上翻键
                                if(Set_Torque_Unit >= 1) Set_Torque_Unit = 0;	   
                                else Set_Torque_Unit++;      
                            break;
                            case EKEY1_PRES:                     //EC11  下翻键
                                if(Set_Torque_Unit <= 0) Set_Torque_Unit = 1;	  
                                else Set_Torque_Unit--;  
                            break;
                            case KEY3_PRES:                     //确认键    
                                SetTextFlickerCycle(5,5,0); //文本停止闪烁                                 
                                current_screen_id = Menu_Sensor_Interface;     //返回设置界面
                                break;	
                        }                          
            break;
            case S_Sensor_ActualPressureuint:                                       //压力单位设置
                        HIM_ActualPressureuint_display(5,7,Set_Pressure_Unit_Type);  //压力单位显示
                        Display_Press_Times ++;
                        if(Display_Press_Times >=  5)
                        {
                            HIM_Pressure_display(5,6,Pressure_Value,Set_Pressure_Unit_Type);//显示压力值
                            Display_Press_Times = 0;
                        }
                        switch(KEY_State)
                        {				 
                              //按键
                            case KEY1_PRES:                     //上翻键
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;  
                            break;
                            case KEY2_PRES:                     //下翻键
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--;   
                            break;
                            case KEY2_Long_PRES:                     //上翻键
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;        
                            break;
                            case KEY1_Long_PRES:                     //下翻键
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--; 
                            break;
                            //旋钮
                            case EKEY3_PRES:                     //上翻键
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;       
                            break;
                            case EKEY4_PRES:                     //下翻键
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--;   
                            break;
                            case EKEY2_PRES:                     //EC11 上翻键
                                if(Set_Pressure_Unit_Type >= 2) Set_Pressure_Unit_Type=0;	   //
                                else Set_Pressure_Unit_Type++;           
                            break;
                            case EKEY1_PRES:                     //EC11  下翻键
                                if(Set_Pressure_Unit_Type <= 0) Set_Pressure_Unit_Type=2;	   //
                                else Set_Pressure_Unit_Type--;   
                            break;
                            case KEY3_PRES:                     //确认键
                                SetTextFlickerCycle(5,7,0); //文本停止闪烁                                 
                                current_screen_id = Menu_Sensor_Interface;     //返回设置界面
                                break;
                        }                      
            break;                        

        }       
	}
}

