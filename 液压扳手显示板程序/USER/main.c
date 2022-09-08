#include "includes.h"
#include "gpio.h"
#include "delay.h"
#include "usart.h"
#include "rtc.h"
#include "timer.h"
#include "exti.h"
#include "iwdg.h"
//START 任务
//设置任务优先级
#define START_TASK_PRIO      			10 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				100
//创建任务堆栈空间	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数接口
void start_task(void *pdata);	

//SYS测试任务
//设置任务优先级
#define SYS_TASK_PRIO       			9
//设置任务堆栈大小
#define SYS_STK_SIZE  		    		100
//任务堆栈	
OS_STK SYS_TASK_STK[SYS_STK_SIZE];
//任务函数
void SYS_task(void *pdata);

//U盘任务
//设置任务优先级
#define Udisk_task_PRIO       			8
//设置任务堆栈大小
#define Udisk_STK_SIZE  		        300
//任务堆栈	
OS_STK Udisk_TASK_STK[Udisk_STK_SIZE];
//任务函数
void Udisk_task(void *pdata);

//flsh存储任务
//设置任务优先级
#define Flash_Task_PRIO       			7
//设置任务堆栈大小
#define Flash_STK_SIZE  		        2300
//任务堆栈	
OS_STK Flash_TASK_STK[Flash_STK_SIZE];
//任务函数
void Flash_task(void *pdata);

//Time任务
//设置任务优先级
#define Time_TASK_PRIO       			5
//设置任务堆栈大小
#define Time_STK_SIZE  		    		2000   //3958
//任务堆栈	
OS_STK Time_TASK_STK[Time_STK_SIZE];
//任务函数
void Time_task(void *pdata);
//__align(8) static


//驱动板接收发送任务
//设置任务优先级
#define UsartDriverRev_TASK_PRIO       	6
//设置任务堆栈大小
#define UsartDriverRev_STK_SIZE  		300
//任务堆栈	
OS_STK UsartDriverRev_TASK_STK[UsartDriverRev_STK_SIZE];
//任务函数
void UsartDriverRev_task(void *pdata);


//串口屏任务
//设置任务优先级
#define HIM_TASK_PRIO       			4
//设置任务堆栈大小
#define HIM_STK_SIZE  		    		300
//任务堆栈	
 OS_STK HIM_TASK_STK[HIM_STK_SIZE];
//任务函数
void HIM_task(void *pdata);




 int main(void)
 {
    u8 i;
    NVIC_Configuration(); 	            //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    Uart1_Init(115200);	 				//串口1初始化为115200
    Hmi_UartInit(115200);               //与显示屏通信串口初始化为115200
    Driver_UartInit(115200);            //与驱动板通信串口初始化为115200
    TypeC_UartInit(115200);             //与上位机通信串口初始化为115200
    delay_init();	                    //延时初始化	  
    gpio_Init();		  	            //GPIO初始化
    TIM3_Int_Init(1000-1,72-1);         //定时器初始化（1ms）
    EXTIX_Init();                       //旋钮按键外部中断初始化
    MX25L6406_Init();                   //FLASH MX25L6406 SPI初始化
    RTC_Init();                         //RTC初始化 
     
//    MX25L6406_Erase(0*4096,0xC7);   //擦除整片
//    delay_ms(300); 
//    printf("Erase Flash OK\r");
//    MX25L6406_Erase(0*4096,0x20);   //擦除flash存储数据
//    MX25L6406_WriteU32(0,17*4096);  //擦除记录数据次数
    Get_Flash_Parameters();             //获取flash存储数据
    delay_ms(300);                       //当flash未被使用,写入一些默认数据，需要延迟                
    IWDG_Init();                        //先存储在开看门狗防止一直复位
    CH374_Init();                       //U盘读取芯片初始化
	i = CH374LibInit( );                                      /* 初始化CH375程序库和CH375芯片,操作成功返回0 */
    mStopIfError( i );	
     
    printf("轶诺科技\r"); 
	OSInit();  
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();
	 
 }

//开始任务
void start_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
	OSStatInit();					//初始化统计任务.这里会延时1秒钟左右	
 	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)         
    OSTaskCreate(Time_task,(void *)0,(OS_STK*)&Time_TASK_STK[Time_STK_SIZE-1],Time_TASK_PRIO);    
    OSTaskCreate(HIM_task,(void *)0,(OS_STK*)&HIM_TASK_STK[HIM_STK_SIZE-1],HIM_TASK_PRIO);
	 OSTaskCreate(UsartDriverRev_task,(void *)0,(OS_STK*)&UsartDriverRev_TASK_STK[UsartDriverRev_STK_SIZE-1],UsartDriverRev_TASK_PRIO); 
    OSTaskCreate(Flash_task,(void *)0,(OS_STK*)&Flash_TASK_STK[Flash_STK_SIZE-1],Flash_Task_PRIO); 	
    OSTaskCreate(SYS_task,(void *)0,(OS_STK*)&SYS_TASK_STK[SYS_STK_SIZE-1],SYS_TASK_PRIO); 
    OSTaskCreate(Udisk_task,(void *)0,(OS_STK*)&Udisk_TASK_STK[Udisk_STK_SIZE-1],Udisk_task_PRIO);    
    OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
    OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
} 

