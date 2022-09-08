#ifndef _USER_DRIVER_
#define _USER_DRIVER_
#include "hmi_user_uart.h"
#define  Green  0xFFE6  //绿色前景色
#define  Bule  0x18DF  //蓝色前景色




/*!
*  \brief     设置控件前景色
* \details  支持控件：进度条
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  color 前景色
*/
void SetControlForeColor(uint16 screen_id,uint16 control_id,uint16 color);

/*!
*  \brief      设置当前画面
*  \param  screen_id 画面ID
*/
void SetScreen(uint16 screen_id);

/*!
*  \brief     设置文本值
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  str 文本值
*/
void SetTextValues(uint16 screen_id,uint16 control_id,uchar *str);

/*!
*  \brief     设置文本为整数值，要求FIRMWARE_VER>=908
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  value 文本数值
*  \param  sign 0-无符号，1-有符号
*  \param  fill_zero 数字位数，不足时左侧补零
*/
void SetTextInt32(uint16 screen_id,uint16 control_id,uint32 value,uint8 sign,uint8 fill_zero);

/*!
*  \brief     设置文本单精度浮点值，要求FIRMWARE_VER>=908
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  value 文本数值
*  \param  precision 小数位数
*  \param  show_zeros 为1时，显示末尾0
*/
void SetTextFloat(uint16 screen_id,uint16 control_id,float value,uint8 precision,uint8 show_zeros);

/*!
*  \brief  设置文本控件闪烁周期
*  \param  screen_id   画面ID
*  \param  control_id  控件ID
*  \param  Data    闪烁周期     单位10毫秒     0代表不闪烁
*/
void SetTextFlickerCycle(uint16 screen_id,uint16 control_id,uint16 Data);

/*!
*  \brief     多语言设置
*  \param  ui_lang 用户界面语言0~9
*  \param  sys_lang 系统键盘语言-0中文，1英文
*/
void SetLanguage(uint8 ui_lang,uint8 sys_lang);
#endif  