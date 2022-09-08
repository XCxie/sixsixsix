#include "includes.h"
#include "gpio.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "adc.h"
#include "MX25L6406.h"
#include "hmi_user_uart.h"	
#include "iwdg.h"
#include "ads8688.h"

//START 任务
//设置任务优先级
#define START_TASK_PRIO      			10 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				300
//创建任务堆栈空间	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数接口
void start_task(void *pdata);	


//__align(8) static
//SYS测试任务
//设置任务优先级
#define SYS_TASK_PRIO       			9
//设置任务堆栈大小
#define SYS_STK_SIZE  		    		300
//任务堆栈	
__align(8) static OS_STK SYS_TASK_STK[SYS_STK_SIZE];
//任务函数
void SYS_task(void *pdata);




//flsh存储任务
//设置任务优先级
#define Flash_Task_PRIO       			7
//设置任务堆栈大小
#define Flash_STK_SIZE  		    		300
//任务堆栈	
OS_STK Flash_TASK_STK[Flash_STK_SIZE];
//任务函数
void Flash_task(void *pdata);


//串口发送任务
//设置任务优先级
#define UsartSend_Task_PRIO       			6
//设置任务堆栈大小
#define UsartSend_STK_SIZE  		    		300
//任务堆栈	
OS_STK UsartSend_TASK_STK[UsartSend_STK_SIZE];
//任务函数
void UsartSend_task(void *pdata);

//串口通信接受发送任务
//设置任务优先级
#define Usart4RevSend_Task_PRIO       			5
//设置任务堆栈大小
#define Usart4RevSend_STK_SIZE  		    		300
//任务堆栈	
OS_STK Usart4RevSend_TASK_STK[Usart4RevSend_STK_SIZE];
//任务函数
void Usart4RevSend_task(void *pdata);


//ADC采集任务
//设置任务优先级
#define ADC_Task_PRIO       			3
//设置任务堆栈大小
#define ADC_STK_SIZE  		    		1024
//任务堆栈	
OS_STK ADC_TASK_STK[ADC_STK_SIZE];
//任务函数
void ADC_task(void *pdata);

//控制任务
//设置任务优先级
#define Control_Task_PRIO       			2
//设置任务堆栈大小
#define Control_STK_SIZE  		        300
//任务堆栈	
__align(8) static OS_STK Control_TASK_STK[Control_STK_SIZE];
//任务函数
void Control_task(void *pdata);

//监听任务
//设置任务优先级
#define Monitor_Task_PRIO       			1
//设置任务堆栈大小
#define Monitor_STK_SIZE  		    		100
//任务堆栈	
OS_STK Monitor_TASK_STK[Monitor_STK_SIZE];
//任务函数
void Monitor_task(void *pdata);

 int main(void)
 {
	delay_init();	        //延时初始化	  
	NVIC_Configuration(); 	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	gpio_Init();		  	//初始化与LED连接的硬件接口
    Uart1_Init(115200);	 	//串口1初始化为115200
    Uart4Init(115200);      //串口4初始化为115200
    TIM3_Int_Init(1000,72-1);
    ExtiInit();//外部中断初始化
    ADS8688_Init(CH2_EN|CH3_EN); //测试的时候关闭
   //  printf("轶诺科技\r\n");
    Set_CH_Range(CHIR_2,ADS8688_IR_2_5V);//设置通道输入范围：0~5.12V   ±1.25*ref  ref=4.096V 
    Set_CH_Range(CHIR_3,ADS8688_IR_2_5V);
    AUTO_RST_Mode();//进入自动扫描模式
    MX25L6406_Init();       //FLASH初始化
 //  MX25L6406_Erase(12*4096,0x20);
    Get_Flash_Parameters();    //存储默认参数
    IWDG_Init();     //先存储在开看门狗防止一直复位
    printf("轶诺科技\r\n");
     
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
    OSTaskCreate(SYS_task,(void *)0,(OS_STK*)&SYS_TASK_STK[SYS_STK_SIZE-1],SYS_TASK_PRIO);
    OSTaskCreate(ADC_task,(void *)0,(OS_STK*)&ADC_TASK_STK[ADC_STK_SIZE-1],ADC_Task_PRIO); 
    OSTaskCreate(Usart4RevSend_task,(void *)0,(OS_STK*)&Usart4RevSend_TASK_STK[Usart4RevSend_STK_SIZE-1],Usart4RevSend_Task_PRIO); 
    OSTaskCreate(Flash_task,(void *)0,(OS_STK*)&Flash_TASK_STK[Flash_STK_SIZE-1],Flash_Task_PRIO);
    OSTaskCreate(Control_task,(void *)0,(OS_STK*)&Control_TASK_STK[Control_STK_SIZE-1],Control_Task_PRIO); 
    OSTaskCreate(UsartSend_task,(void *)0,(OS_STK*)&UsartSend_TASK_STK[UsartSend_STK_SIZE-1],UsartSend_Task_PRIO); 
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
} 
