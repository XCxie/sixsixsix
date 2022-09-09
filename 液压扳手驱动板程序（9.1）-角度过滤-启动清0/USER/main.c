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

//START ����
//�����������ȼ�
#define START_TASK_PRIO      			10 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				300
//���������ջ�ռ�	
OS_STK START_TASK_STK[START_STK_SIZE];
//�������ӿ�
void start_task(void *pdata);	


//__align(8) static
//SYS��������
//�����������ȼ�
#define SYS_TASK_PRIO       			9
//���������ջ��С
#define SYS_STK_SIZE  		    		300
//�����ջ	
__align(8) static OS_STK SYS_TASK_STK[SYS_STK_SIZE];
//������
void SYS_task(void *pdata);




//flsh�洢����
//�����������ȼ�
#define Flash_Task_PRIO       			7
//���������ջ��С
#define Flash_STK_SIZE  		    		300
//�����ջ	
OS_STK Flash_TASK_STK[Flash_STK_SIZE];
//������
void Flash_task(void *pdata);


//���ڷ�������
//�����������ȼ�
#define UsartSend_Task_PRIO       			6
//���������ջ��С
#define UsartSend_STK_SIZE  		    		300
//�����ջ	
OS_STK UsartSend_TASK_STK[UsartSend_STK_SIZE];
//������
void UsartSend_task(void *pdata);

//����ͨ�Ž��ܷ�������
//�����������ȼ�
#define Usart4RevSend_Task_PRIO       			5
//���������ջ��С
#define Usart4RevSend_STK_SIZE  		    		300
//�����ջ	
OS_STK Usart4RevSend_TASK_STK[Usart4RevSend_STK_SIZE];
//������
void Usart4RevSend_task(void *pdata);


//ADC�ɼ�����
//�����������ȼ�
#define ADC_Task_PRIO       			3
//���������ջ��С
#define ADC_STK_SIZE  		    		1024
//�����ջ	
OS_STK ADC_TASK_STK[ADC_STK_SIZE];
//������
void ADC_task(void *pdata);

//��������
//�����������ȼ�
#define Control_Task_PRIO       			2
//���������ջ��С
#define Control_STK_SIZE  		        300
//�����ջ	
__align(8) static OS_STK Control_TASK_STK[Control_STK_SIZE];
//������
void Control_task(void *pdata);

//��������
//�����������ȼ�
#define Monitor_Task_PRIO       			1
//���������ջ��С
#define Monitor_STK_SIZE  		    		100
//�����ջ	
OS_STK Monitor_TASK_STK[Monitor_STK_SIZE];
//������
void Monitor_task(void *pdata);

 int main(void)
 {
	delay_init();	        //��ʱ��ʼ��	  
	NVIC_Configuration(); 	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	gpio_Init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ�
    Uart1_Init(115200);	 	//����1��ʼ��Ϊ115200
    Uart4Init(115200);      //����4��ʼ��Ϊ115200
    TIM3_Int_Init(1000,72-1);
    ExtiInit();//�ⲿ�жϳ�ʼ��
    ADS8688_Init(CH2_EN|CH3_EN); //���Ե�ʱ��ر�
   //  printf("��ŵ�Ƽ�\r\n");
    Set_CH_Range(CHIR_2,ADS8688_IR_2_5V);//����ͨ�����뷶Χ��0~5.12V   ��1.25*ref  ref=4.096V 
    Set_CH_Range(CHIR_3,ADS8688_IR_2_5V);
    AUTO_RST_Mode();//�����Զ�ɨ��ģʽ
    MX25L6406_Init();       //FLASH��ʼ��
 //  MX25L6406_Erase(12*4096,0x20);
    Get_Flash_Parameters();    //�洢Ĭ�ϲ���
    IWDG_Init();     //�ȴ洢�ڿ����Ź���ֹһֱ��λ
    printf("��ŵ�Ƽ�\r\n");
     
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
    OSTaskCreate(SYS_task,(void *)0,(OS_STK*)&SYS_TASK_STK[SYS_STK_SIZE-1],SYS_TASK_PRIO);
    OSTaskCreate(ADC_task,(void *)0,(OS_STK*)&ADC_TASK_STK[ADC_STK_SIZE-1],ADC_Task_PRIO); 
    OSTaskCreate(Usart4RevSend_task,(void *)0,(OS_STK*)&Usart4RevSend_TASK_STK[Usart4RevSend_STK_SIZE-1],Usart4RevSend_Task_PRIO); 
    OSTaskCreate(Flash_task,(void *)0,(OS_STK*)&Flash_TASK_STK[Flash_STK_SIZE-1],Flash_Task_PRIO);
    OSTaskCreate(Control_task,(void *)0,(OS_STK*)&Control_TASK_STK[Control_STK_SIZE-1],Control_Task_PRIO); 
    OSTaskCreate(UsartSend_task,(void *)0,(OS_STK*)&UsartSend_TASK_STK[UsartSend_STK_SIZE-1],UsartSend_Task_PRIO); 
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
} 
