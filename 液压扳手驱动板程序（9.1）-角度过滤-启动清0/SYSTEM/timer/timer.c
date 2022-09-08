#include "timer.h"
#include "gpio.h"
#include "usart.h"	
#include "hmi_user_uart.h"	
//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3

uint16 usCaptureRisingVal = 0;

void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM3, //TIM3
		TIM_IT_Update,
		ENABLE  //使能
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
							 
}
u16 Timer_count =0;
void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
            UartRxMonitor(1);
            Timer_count++;
            TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
 
		}
}

/*角度脉冲捕获*/
void ExtiGpioInit(void);
void ExtiNvicInit(void);
void ExtiModeInit(void);

void ExtiInit()
{
	ExtiGpioInit();
    ExtiNvicInit();
    ExtiModeInit();
}

void ExtiGpioInit()
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);//外设总线2上使能GPIOA时钟

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void ExtiNvicInit()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	// 配置TIM中断	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;      	// 选择TIM2的中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	// 抢占式中断优先级设置为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		// 响应式中断优先级设置为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			// 中断使能
	NVIC_Init(&NVIC_InitStructure);	
}

void ExtiModeInit()
{
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
	
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	
    EXTI_Init(&EXTI_InitStructure);
}


void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0) == SET){
		usCaptureRisingVal++;    //一千个脉冲为360度           
        EXTI_ClearFlag(EXTI_Line0);
	}	
}

float Get_angle_calculattion(void)
{
    
    float value = 0;
    value = usCaptureRisingVal*0.36;
   // printf("VALUE:%f\r\n",value);
    return value;
}


