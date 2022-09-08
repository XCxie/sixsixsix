#include "gpio.h"
#include "timer.h"
//LED IO初始化
void gpio_Init(void)
{
 
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);	 //使能ABCD端口时钟

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    // 改变指定管脚的映射 GPIO_Remap_SWJ_Disable SWJ 完全禁用（JTAG+SW-DP）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
    // 改变指定管脚的映射 GPIO_Remap_SWJ_JTAGDisable ，JTAG-DP 禁用 + SW-DP 使能

    //LED端口
    GPIO_InitStructure.GPIO_Pin = SYS_LED_PIN;				      
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(SYS_LED_GPIO_Port, &GPIO_InitStructure);					 //根据设定参数初始化PC4
    
    GPIO_InitStructure.GPIO_Pin = WANING_LED_PIN;				      
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(WANING_LED_GPIO_Port, &GPIO_InitStructure);					 //根据设定参数初始化PC5

	SYS_LED_OFF;
	WANING_LED_OFF;

    //按键端口
    GPIO_InitStructure.GPIO_Pin  = KEY1_PIN|KEY2_PIN|KEY3_PIN|KEY4_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置上拉输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    
}
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//u8 KEY_Type = 0;
//u8 KEY_Scan(u8 mode)
//{	 
//	static u8 key_up=1;//按键按松开标志
//    u8 key_Status = 0;
//	if(mode)
//    key_up=1;  //支持连按		  
//	if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0))
//	{
//		delay_ms(100);//去抖动 
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
//	return key_Status;// 无按键按下
//}


u8 KEY_Type = 0;
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
    u8 key_Status = 0;
	if(mode)
    key_up=1;  //支持连按		  
	if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0))
	{
		OSTimeDly(20);//去抖动 
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
	return key_Status;// 无按键按下
}