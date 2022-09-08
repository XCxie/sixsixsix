#include "him_user_driver.h"

#define   SEND_DATA(P)   SendChar(P)            //����һ���ֽ�
#define   TX_8(P)        SEND_DATA((P)&0xFF)    //���͵����ֽ�
#define   TX_8N(P,N)     SendNU8((P),N)         //����N���ֽ�
#define   TX_16(P)       TX_8((P)>>8);TX_8(P)   //����16λ����
#define   TX_16N(P,N)    SendNU16((P),N)        //����N��16λ����
#define   TX_32(P)       TX_16((P)>>16);TX_16(P)//����32λ����
#define   TX_32N(P,N)     SendNU32((P),N)       //����N��32λ����
#define BEGIN_CMD() TX_8(0XEE)            //֡ͷ
#define END_CMD() TX_32(0XFFFCFFFF)       //֡β

/*! 
*  \brief  ���ڷ������ַ���
*  \param  �ַ���
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
*  \brief  ���ڷ�����N���ֽ�
*  \param  ����
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
*  \brief  ���ڷ�����N��16λ������
*  \param  ����
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
*  \brief  ���ڷ�����N��32λ������
*  \param  ����
*/
void  SendNU32(uint32_t*pData,uint16_t DataLen)
{
    int i = 0;
	for(i=0;i<DataLen;i++)
	{
	
        TX_32(pData[i]);
		
	}
}


//-----------------------------------------------��ָ�����-----------------------------------------------------//
/*!
*  \brief     ���ÿؼ�ǰ��ɫ
* \details  ֧�ֿؼ���������
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  color ǰ��ɫ
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
*  \brief      ���õ�ǰ����
*  \param  screen_id ����ID
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
*  \brief     �����ı�ֵ
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  str �ı�ֵ
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
*  \brief     �����ı�Ϊ����ֵ��Ҫ��FIRMWARE_VER>=908
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  value �ı���ֵ
*  \param  sign 0-�޷��ţ�1-�з���
*  \param  fill_zero ����λ��������ʱ��ಹ��
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
*  \brief     �����ı������ȸ���ֵ��Ҫ��FIRMWARE_VER>=908
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  value �ı���ֵ
*  \param  precision С��λ��
*  \param  show_zeros Ϊ1ʱ����ʾĩβ0
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
	 //��Ҫ���ִ�С��
#if(0)
		TX_8(((uint8 *)&value)[i]);
#else
		TX_8(((uint8 *)&value)[3-i]);
#endif
	}
	END_CMD();
}

/*!
*  \brief  �����ı��ؼ���˸����
*  \param  screen_id   ����ID
*  \param  control_id  �ؼ�ID
*  \param  Data    ��˸����     ��λ10����     0������˸
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
*  \brief     ����������
*  \param  ui_lang �û���������0~9
*  \param  sys_lang ϵͳ��������-0���ģ�1Ӣ��
*/
void SetLanguage(uint8 ui_lang,uint8 sys_lang)
{
    uint8 lang = ui_lang;
    if(sys_lang)   lang |= 0x80;

    BEGIN_CMD();
    TX_8(0xC1);
    TX_8(lang);
    TX_8(0xC1+lang);//У�飬��ֹ�����޸�����
    END_CMD();
}


