#include "gpio.h" 
								    	    
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

    GPIO_InitStructure.GPIO_Pin = STAR_LED_PIN;				      
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(STAR_LED_GPIO_Port, &GPIO_InitStructure);	 		         //�����趨������ʼ��PB0
	SYS_LED_OFF;
	WANING_LED_OFF;
    STAR_LED_OFF;

    //AD8688�궨�˿�
    GPIO_InitStructure.GPIO_Pin = AD8688_IO_1_PIN;				      
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(AD8688_IO_1_GPIO_Port, &GPIO_InitStructure);					 //�����趨������ʼ��PC0
    
    GPIO_InitStructure.GPIO_Pin = AD8688_IO_0_PIN;				      
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(AD8688_IO_0_GPIO_Port, &GPIO_InitStructure);					 //�����趨������ʼ��PC1
    
    
    //�ֱ������˿�
    GPIO_InitStructure.GPIO_Pin  =EKEY1_PIN ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //������������
    GPIO_Init(EKEY1_GPIO_Port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin  =EKEY2_PIN ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //������������
    GPIO_Init(EKEY2_GPIO_Port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin  =EKEY3_PIN ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //������������
    GPIO_Init(EKEY3_GPIO_Port, &GPIO_InitStructure);

    //����ض˿�
    GPIO_InitStructure.GPIO_Pin = KMCTRL1_PIN|KMCTRL2_PIN|KMCTRL3_PIN|KMCTRL4_PIN;				      
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);	 		         //�����趨������ʼ��
    GPIO_InitStructure.GPIO_Pin = KMCTRL5_PIN|KMCTRL6_PIN;				      
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);	 		         //�����趨������ʼ��PB0
    KMCTRL1_OFF;
    KMCTRL2_OFF;
    KMCTRL3_OFF;
    KMCTRL4_OFF;
    KMCTRL5_OFF;
    KMCTRL6_OFF;
	
}
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;

u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)
        key_up=1;  //֧������
	if(key_up&&(EKEY3==0||EKEY2==0||EKEY1==0))
	{
		OSTimeDly(2);//ȥ���� 
		key_up=0;
		if(EKEY3==1)return KEY3_PRES;     
		else if(EKEY2==1)return KEY2_PRES;
		else if(EKEY1==1)return KEY1_PRES; 
	}
    else if(EKEY1==0&&EKEY2==0&&EKEY3==0)
        key_up=1; 	     
	return KEY_NOPRES;// �ް�������
}


