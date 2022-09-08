#include "includes.h"
#include "gpio.h"
#include "delay.h"
#include "usart.h"
#include "rtc.h"
#include "timer.h"
#include "exti.h"
#include "iwdg.h"
//START ����
//�����������ȼ�
#define START_TASK_PRIO      			10 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				100
//���������ջ�ռ�	
OS_STK START_TASK_STK[START_STK_SIZE];
//�������ӿ�
void start_task(void *pdata);	

//SYS��������
//�����������ȼ�
#define SYS_TASK_PRIO       			9
//���������ջ��С
#define SYS_STK_SIZE  		    		100
//�����ջ	
OS_STK SYS_TASK_STK[SYS_STK_SIZE];
//������
void SYS_task(void *pdata);

//U������
//�����������ȼ�
#define Udisk_task_PRIO       			8
//���������ջ��С
#define Udisk_STK_SIZE  		        300
//�����ջ	
OS_STK Udisk_TASK_STK[Udisk_STK_SIZE];
//������
void Udisk_task(void *pdata);

//flsh�洢����
//�����������ȼ�
#define Flash_Task_PRIO       			7
//���������ջ��С
#define Flash_STK_SIZE  		        2300
//�����ջ	
OS_STK Flash_TASK_STK[Flash_STK_SIZE];
//������
void Flash_task(void *pdata);

//Time����
//�����������ȼ�
#define Time_TASK_PRIO       			5
//���������ջ��С
#define Time_STK_SIZE  		    		2000   //3958
//�����ջ	
OS_STK Time_TASK_STK[Time_STK_SIZE];
//������
void Time_task(void *pdata);
//__align(8) static


//��������շ�������
//�����������ȼ�
#define UsartDriverRev_TASK_PRIO       	6
//���������ջ��С
#define UsartDriverRev_STK_SIZE  		300
//�����ջ	
OS_STK UsartDriverRev_TASK_STK[UsartDriverRev_STK_SIZE];
//������
void UsartDriverRev_task(void *pdata);


//����������
//�����������ȼ�
#define HIM_TASK_PRIO       			4
//���������ջ��С
#define HIM_STK_SIZE  		    		300
//�����ջ	
 OS_STK HIM_TASK_STK[HIM_STK_SIZE];
//������
void HIM_task(void *pdata);




 int main(void)
 {
    u8 i;
    NVIC_Configuration(); 	            //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    Uart1_Init(115200);	 				//����1��ʼ��Ϊ115200
    Hmi_UartInit(115200);               //����ʾ��ͨ�Ŵ��ڳ�ʼ��Ϊ115200
    Driver_UartInit(115200);            //��������ͨ�Ŵ��ڳ�ʼ��Ϊ115200
    TypeC_UartInit(115200);             //����λ��ͨ�Ŵ��ڳ�ʼ��Ϊ115200
    delay_init();	                    //��ʱ��ʼ��	  
    gpio_Init();		  	            //GPIO��ʼ��
    TIM3_Int_Init(1000-1,72-1);         //��ʱ����ʼ����1ms��
    EXTIX_Init();                       //��ť�����ⲿ�жϳ�ʼ��
    MX25L6406_Init();                   //FLASH MX25L6406 SPI��ʼ��
    RTC_Init();                         //RTC��ʼ�� 
     
//    MX25L6406_Erase(0*4096,0xC7);   //������Ƭ
//    delay_ms(300); 
//    printf("Erase Flash OK\r");
//    MX25L6406_Erase(0*4096,0x20);   //����flash�洢����
//    MX25L6406_WriteU32(0,17*4096);  //������¼���ݴ���
    Get_Flash_Parameters();             //��ȡflash�洢����
    delay_ms(300);                       //��flashδ��ʹ��,д��һЩĬ�����ݣ���Ҫ�ӳ�                
    IWDG_Init();                        //�ȴ洢�ڿ����Ź���ֹһֱ��λ
    CH374_Init();                       //U�̶�ȡоƬ��ʼ��
	i = CH374LibInit( );                                      /* ��ʼ��CH375������CH375оƬ,�����ɹ�����0 */
    mStopIfError( i );	
     
    printf("��ŵ�Ƽ�\r"); 
	OSInit();  
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	OSStart();
	 
 }

//��ʼ����
void start_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
	OSStatInit();					//��ʼ��ͳ������.�������ʱ1��������	
 	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)         
    OSTaskCreate(Time_task,(void *)0,(OS_STK*)&Time_TASK_STK[Time_STK_SIZE-1],Time_TASK_PRIO);    
    OSTaskCreate(HIM_task,(void *)0,(OS_STK*)&HIM_TASK_STK[HIM_STK_SIZE-1],HIM_TASK_PRIO);
	 OSTaskCreate(UsartDriverRev_task,(void *)0,(OS_STK*)&UsartDriverRev_TASK_STK[UsartDriverRev_STK_SIZE-1],UsartDriverRev_TASK_PRIO); 
    OSTaskCreate(Flash_task,(void *)0,(OS_STK*)&Flash_TASK_STK[Flash_STK_SIZE-1],Flash_Task_PRIO); 	
    OSTaskCreate(SYS_task,(void *)0,(OS_STK*)&SYS_TASK_STK[SYS_STK_SIZE-1],SYS_TASK_PRIO); 
    OSTaskCreate(Udisk_task,(void *)0,(OS_STK*)&Udisk_TASK_STK[Udisk_STK_SIZE-1],Udisk_task_PRIO);    
    OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
    OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
} 

