#include "him_user_driver.h"

#define   SEND_DATA(P)   SendChar(P)            //发送一个字节
#define   TX_8(P)        SEND_DATA((P)&0xFF)    //发送单个字节
#define   TX_8N(P,N)     SendNU8((P),N)         //发送N个字节
#define   TX_16(P)       TX_8((P)>>8);TX_8(P)   //发送16位整数
#define   TX_16N(P,N)    SendNU16((P),N)        //发送N个16位整数
#define   TX_32(P)       TX_16((P)>>16);TX_16(P)//发送32位整数
#define   TX_32N(P,N)     SendNU32((P),N)       //发送N个32位整数
#define BEGIN_CMD() TX_8(0XEE)            //帧头
#define END_CMD() TX_32(0XFFFCFFFF)       //帧尾

/*! 
*  \brief  串口发送送字符串
*  \param  字符串
*/
void SendStrings(uchar *str)
{
    while(*str)
    {
        TX_8(*str);
        str++;
    }
}
/*! 
*  \brief  串口发送送N个字节
*  \param  个数
*/
void  SendNU8(uint8_t *pData,uint16_t DataLen)

{
	int i ;
  for( i=0;i<DataLen;i++)
	{
		
	  TX_8(pData[i]);
	
	}
}
/*! 
*  \brief  串口发送送N个16位的数据
*  \param  个数
*/
void SendNU16(uint16_t*pData,uint16_t DataLen)
{

  int i=0;
	for(i=0;i<DataLen;i++)
	{
		
		TX_16(pData[i]);
		
	}

}
/*! 
*  \brief  串口发送送N个32位的数据
*  \param  个数
*/
void  SendNU32(uint32_t*pData,uint16_t DataLen)
{
    int i = 0;
	for(i=0;i<DataLen;i++)
	{
	
        TX_32(pData[i]);
		
	}
}


//-----------------------------------------------屏指令函数集-----------------------------------------------------//
/*!
*  \brief     设置控件前景色
* \details  支持控件：进度条
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  color 前景色
*/
void SetControlForeColor(uint16 screen_id,uint16 control_id,uint16 color)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x19);
    TX_16(screen_id);
    TX_16(control_id);
    TX_16(color);
    END_CMD();
}

/*!
*  \brief      设置当前画面
*  \param  screen_id 画面ID
*/
void SetScreen(uint16 screen_id)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x00);
    TX_16(screen_id);
    END_CMD();
}

/*!
*  \brief     设置文本值
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  str 文本值
*/
void SetTextValues(uint16 screen_id,uint16 control_id,uchar *str)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x10);
    TX_16(screen_id);
    TX_16(control_id);
    SendStrings(str);
    END_CMD();
}
/*!
*  \brief     设置文本为整数值，要求FIRMWARE_VER>=908
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  value 文本数值
*  \param  sign 0-无符号，1-有符号
*  \param  fill_zero 数字位数，不足时左侧补零
*/
void SetTextInt32(uint16 screen_id,uint16 control_id,uint32 value,uint8 sign,uint8 fill_zero)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x07);
    TX_16(screen_id);
    TX_16(control_id);
    TX_8(sign?0X01:0X00);
    TX_8((fill_zero&0x0f)|0x80);
    TX_32(value);
    END_CMD();
}

/*!
*  \brief     设置文本单精度浮点值，要求FIRMWARE_VER>=908
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  value 文本数值
*  \param  precision 小数位数
*  \param  show_zeros 为1时，显示末尾0
*/
void SetTextFloat(uint16 screen_id,uint16 control_id,float value,uint8 precision,uint8 show_zeros)
{
	uint8 i = 0;

	BEGIN_CMD();
	TX_8(0xB1);
	TX_8(0x07);
	TX_16(screen_id);
	TX_16(control_id);
	TX_8(0x02);
	TX_8((precision&0x0f)|(show_zeros?0x80:0x00));

	for (i=0;i<4;++i)
	{
	 //需要区分大小端
#if(0)
		TX_8(((uint8 *)&value)[i]);
#else
		TX_8(((uint8 *)&value)[3-i]);
#endif
	}
	END_CMD();
}

/*!
*  \brief  设置文本控件闪烁周期
*  \param  screen_id   画面ID
*  \param  control_id  控件ID
*  \param  Data    闪烁周期     单位10毫秒     0代表不闪烁
*/
void SetTextFlickerCycle(uint16 screen_id,uint16 control_id,uint16 Data)
{

    BEGIN_CMD();
    TX_8(0xB1);
	TX_8(0x15);
	TX_16(screen_id);
	TX_16(control_id);
	TX_16(Data);
    END_CMD();
}

/*!
*  \brief     多语言设置
*  \param  ui_lang 用户界面语言0~9
*  \param  sys_lang 系统键盘语言-0中文，1英文
*/
void SetLanguage(uint8 ui_lang,uint8 sys_lang)
{
    uint8 lang = ui_lang;
    if(sys_lang)   lang |= 0x80;

    BEGIN_CMD();
    TX_8(0xC1);
    TX_8(lang);
    TX_8(0xC1+lang);//校验，防止意外修改语言
    END_CMD();
}


