#include "gpio.h"
#include "timer.h"
//LED IO��ʼ��
void gpio_Init(void)
{
 
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);	 //ʹ��ABCD�˿�ʱ��

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    // �ı�ָ���ܽŵ�ӳ�� GPIO_Remap_SWJ_Disable SWJ ��ȫ���ã�JTAG+SW-DP��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
    // �ı�ָ���ܽŵ�ӳ�� GPIO_Remap_SWJ_JTAGDisable ��JTAG-DP ���� + SW-DP ʹ��

    //LED�˿�
    GPIO_InitStructure.GPIO_Pin = SYS_LED_PIN;				      
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(SYS_LED_GPIO_Port, &GPIO_InitStructure);					 //�����趨������ʼ��PC4
    
    GPIO_InitStructure.GPIO_Pin = WANING_LED_PIN;				      
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(WANING_LED_GPIO_Port, &GPIO_InitStructure);					 //�����趨������ʼ��PC5

	SYS_LED_OFF;
	WANING_LED_OFF;

    //�����˿�
    GPIO_InitStructure.GPIO_Pin  = KEY1_PIN|KEY2_PIN|KEY3_PIN|KEY4_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //������������
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    
}
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//u8 KEY_Type = 0;
//u8 KEY_Scan(u8 mode)
//{	 
//	static u8 key_up=1;//�������ɿ���־
//    u8 key_Status = 0;
//	if(mode)
//    key_up=1;  //֧������		  
//	if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0))
//	{
//		delay_ms(100);//ȥ���� 
//             
//        if(KEY1==0)
//        {    
//            key_up=0; 
//            key_Status =  KEY2_PRES;
//            KEY4_ucTimer = 1600;
//        }
//		else if(KEY4==0)
//        {
//            key_up=0; 
//            key_Status =  KEY3_PRES;
//            KEY4_ucTimer = 1600;
//        }
//		else if(KEY3==0)
//        {
//            key_up=0; 
//            key_Status  = KEY1_PRES; 
//            KEY4_ucTimer = 1600;
//        }
//        if(KEY2==0 && KEY4_ucTimer > 1000 && KEY4_ucTimer <=1500)
//        {
//                key_up=1; 
//                key_Status = KEY4_PRES ;
//        }
//        else  if( (KEY4_ucTimer <= 1000))
//        {
//                key_up=1; 
//                key_Status =  KEY4_Long_PRES;
//                KEY4_ucTimer= 1600;
//        }       
//	}
//    else if(KEY1==1&&KEY2==1&&KEY3==1&&KEY4==1)
//    {   
//        key_up=1; 
//        key_Status = 0;        
//        KEY4_ucTimer = 1600;                       
//    }    
//	return key_Status;// �ް�������
//}


u8 KEY_Type = 0;
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
    u8 key_Status = 0;
	if(mode)
    key_up=1;  //֧������		  
	if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0))
	{
		OSTimeDly(20);//ȥ���� 
        if(KEY3==0 && KEY4_ucTimer > 8000 && KEY4_ucTimer <=9500)
        {
                key_up=1; 
                key_Status = KEY2_PRES ;
        }
        else  if( KEY1==0 && KEY4_ucTimer <= 8000)
        {
                key_up=1; 
                key_Status =  KEY2_Long_PRES;
                KEY4_ucTimer= 4000;
        } 
        
        if(KEY4==0)
        {
            key_up=0; 
            key_Status =  KEY3_PRES;
            KEY4_ucTimer = 9600;
        }
        
        if(KEY1==0 && KEY4_ucTimer > 8000 && KEY4_ucTimer <=9500)
        {
                key_up=1; 
                key_Status = KEY1_PRES ;
        }
        else  if( (KEY3==0 && KEY4_ucTimer <= 8000))
        {
                key_up=1; 
                key_Status =  KEY1_Long_PRES;
                KEY4_ucTimer= 4000;
        } 
        
        
        if(KEY2==0 && KEY4_ucTimer > 9000 && KEY4_ucTimer <=9500)
        {
                key_up=1; 
                key_Status = KEY4_PRES ;
        }
        else  if( (KEY2==0 && KEY4_ucTimer <= 9000))
        {
                key_up=1; 
                key_Status =  KEY4_Long_PRES;
                KEY4_ucTimer= 9600;
        }       
	}
    else if(KEY1==1&&KEY2==1&&KEY3==1&&KEY4==1)
    {   
        key_up=1; 
        key_Status = 0;        
        KEY4_ucTimer = 9600;                       
    }    
	return key_Status;// �ް�������
}