#include "gpio.h" 
								    	    
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

    GPIO_InitStructure.GPIO_Pin = STAR_LED_PIN;				      
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(STAR_LED_GPIO_Port, &GPIO_InitStructure);	 		         //根据设定参数初始化PB0
	SYS_LED_OFF;
	WANING_LED_OFF;
    STAR_LED_OFF;

    //AD8688标定端口
    GPIO_InitStructure.GPIO_Pin = AD8688_IO_1_PIN;				      
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(AD8688_IO_1_GPIO_Port, &GPIO_InitStructure);					 //根据设定参数初始化PC0
    
    GPIO_InitStructure.GPIO_Pin = AD8688_IO_0_PIN;				      
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(AD8688_IO_0_GPIO_Port, &GPIO_InitStructure);					 //根据设定参数初始化PC1
    
    
    //手柄按键端口
    GPIO_InitStructure.GPIO_Pin  =EKEY1_PIN ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置上拉输入
    GPIO_Init(EKEY1_GPIO_Port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin  =EKEY2_PIN ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置上拉输入
    GPIO_Init(EKEY2_GPIO_Port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin  =EKEY3_PIN ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置上拉输入
    GPIO_Init(EKEY3_GPIO_Port, &GPIO_InitStructure);

    //光耦开关端口
    GPIO_InitStructure.GPIO_Pin = KMCTRL1_PIN|KMCTRL2_PIN|KMCTRL3_PIN|KMCTRL4_PIN;				      
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);	 		         //根据设定参数初始化
    GPIO_InitStructure.GPIO_Pin = KMCTRL5_PIN|KMCTRL6_PIN;				      
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);	 		         //根据设定参数初始化PB0
    KMCTRL1_OFF;
    KMCTRL2_OFF;
    KMCTRL3_OFF;
    KMCTRL4_OFF;
    KMCTRL5_OFF;
    KMCTRL6_OFF;
	
}
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;

u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)
        key_up=1;  //支持连按
	if(key_up&&(EKEY3==0||EKEY2==0||EKEY1==0))
	{
		OSTimeDly(2);//去抖动 
		key_up=0;
		if(EKEY3==1)return KEY3_PRES;     
		else if(EKEY2==1)return KEY2_PRES;
		else if(EKEY1==1)return KEY1_PRES; 
	}
    else if(EKEY1==0&&EKEY2==0&&EKEY3==0)
        key_up=1; 	     
	return KEY_NOPRES;// 无按键按下
}


