#ifndef _USER_DRIVER_
#define _USER_DRIVER_
#include "hmi_user_uart.h"
#define  Green  0xFFE6  //��ɫǰ��ɫ
#define  Bule  0x18DF  //��ɫǰ��ɫ




/*!
*  \brief     ���ÿؼ�ǰ��ɫ
* \details  ֧�ֿؼ���������
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  color ǰ��ɫ
*/
void SetControlForeColor(uint16 screen_id,uint16 control_id,uint16 color);

/*!
*  \brief      ���õ�ǰ����
*  \param  screen_id ����ID
*/
void SetScreen(uint16 screen_id);

/*!
*  \brief     �����ı�ֵ
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  str �ı�ֵ
*/
void SetTextValues(uint16 screen_id,uint16 control_id,uchar *str);

/*!
*  \brief     �����ı�Ϊ����ֵ��Ҫ��FIRMWARE_VER>=908
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  value �ı���ֵ
*  \param  sign 0-�޷��ţ�1-�з���
*  \param  fill_zero ����λ��������ʱ��ಹ��
*/
void SetTextInt32(uint16 screen_id,uint16 control_id,uint32 value,uint8 sign,uint8 fill_zero);

/*!
*  \brief     �����ı������ȸ���ֵ��Ҫ��FIRMWARE_VER>=908
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  value �ı���ֵ
*  \param  precision С��λ��
*  \param  show_zeros Ϊ1ʱ����ʾĩβ0
*/
void SetTextFloat(uint16 screen_id,uint16 control_id,float value,uint8 precision,uint8 show_zeros);

/*!
*  \brief  �����ı��ؼ���˸����
*  \param  screen_id   ����ID
*  \param  control_id  �ؼ�ID
*  \param  Data    ��˸����     ��λ10����     0������˸
*/
void SetTextFlickerCycle(uint16 screen_id,uint16 control_id,uint16 Data);

/*!
*  \brief     ����������
*  \param  ui_lang �û���������0~9
*  \param  sys_lang ϵͳ��������-0���ģ�1Ӣ��
*/
void SetLanguage(uint8 ui_lang,uint8 sys_lang);
#endif  